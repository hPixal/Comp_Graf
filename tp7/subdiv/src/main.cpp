#include <algorithm>
#include <stdexcept>
#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Debug.hpp"
#include "Shaders.hpp"
#include "SubDivMesh.hpp"
#include "SubDivMeshRenderer.hpp"

#define VERSION 20221013
#include <iostream>
#include <map>



// models and settings
std::vector<std::string> models_names = { "cubo", "icosahedron", "plano", "suzanne", "star" };
int current_model = 0;
bool fill = true, nodes = true, wireframe = true, smooth = false, 
	 reload_mesh = true, mesh_modified = false;

// extraa callbacks
void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);

SubDivMesh mesh;
void subdivide(SubDivMesh &mesh);

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Demo",true);
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	view_fov = 60.f;
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); 
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.8f,0.8f,0.9f,1.f);
	Shader shader_flat("shaders/flat"),
	       shader_smooth("shaders/smooth"),
		   shader_wireframe("shaders/wireframe");
	SubDivMeshRenderer renderer;
	
	// main loop
	Material material;
	material.ka = material.kd = glm::vec3{.8f,.4f,.4f};
	material.ks = glm::vec3{.5f,.5f,.5f};
	material.shininess = 50.f;
	
	FrameTimer timer;
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
		if (reload_mesh) {
			mesh = SubDivMesh("models/"+models_names[current_model]+".dat");
			reload_mesh = false; mesh_modified = true;
		}
		if (mesh_modified) {
			renderer = makeRenderer(mesh);
			mesh_modified = false;
		}
		
		if (nodes) {
			shader_wireframe.use();
			setMatrixes(shader_wireframe);
			renderer.drawPoints(shader_wireframe);
		}
		
		if (wireframe) {
			shader_wireframe.use();
			setMatrixes(shader_wireframe);
			renderer.drawLines(shader_wireframe);
		}
		
		if (fill) {
			Shader &shader = smooth ? shader_smooth : shader_flat;
			shader.use();
			setMatrixes(shader);
			shader.setLight(glm::vec4{2.f,1.f,5.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.25f);
			shader.setMaterial(material);
			renderer.drawTriangles(shader);
		}
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			if (ImGui::Combo(".dat (O)", &current_model,models_names)) reload_mesh = true;
			ImGui::Checkbox("Fill (F)",&fill);
			ImGui::Checkbox("Wireframe (W)",&wireframe);
			ImGui::Checkbox("Nodes (N)",&nodes);
			ImGui::Checkbox("Smooth Shading (S)",&smooth);
			if (ImGui::Button("Subdivide (D)")) { subdivide(mesh); mesh_modified = true; }
			if (ImGui::Button("Reset (R)")) reload_mesh = true;
			ImGui::Text("Nodes: %i, Elements: %i",mesh.n.size(),mesh.e.size());
		});
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'D': subdivide(mesh); mesh_modified = true; break;
		case 'F': fill = !fill; break;
		case 'N': nodes = !nodes; break;
		case 'W': wireframe = !wireframe; break;
		case 'S': smooth = !smooth; break;
		case 'R': reload_mesh=true; break;
		case 'O': case 'M': current_model = (current_model+1)%models_names.size(); reload_mesh = true; break;
		}
	}
}

// La struct Arista guarda los dos indices de nodos de una arista
// Siempre pone primero el menor indice, para facilitar la b�squeda en lista ordenada;
//    es para usar con el Mapa de m�s abajo, para asociar un nodo nuevo a una arista vieja
struct Arista {
	int n[2];
	Arista(int n1, int n2) {
		n[0]=n1; n[1]=n2;
		if (n[0]>n[1]) std::swap(n[0],n[1]);
	}
	Arista(Elemento &e, int i) { // i-esima arista de un elemento
		n[0]=e[i]; n[1]=e[i+1];
		if (n[0]>n[1]) std::swap(n[0],n[1]); // pierde el orden del elemento
	}
	const bool operator<(const Arista &a) const {
		return (n[0]<a.n[0]||(n[0]==a.n[0]&&n[1]<a.n[1]));
	}
};

// Mapa sirve para guardar una asociaci�n entre una arista y un indice de nodo (que no es de la arista)
using Mapa = std::map<Arista,int>;

void subdivide(SubDivMesh &mesh) {
	
	/// @@@@@: Implementar Catmull-Clark... lineamientos:
	//  Los nodos originales estan en las posiciones 0 a #n-1 de m.n,
	//  Los elementos orignales estan en las posiciones 0 a #e-1 de m.e
	
	std::vector<Elemento> elementos = mesh.e; // Vector de elementos existentes
	std::vector<Nodo> nodos = mesh.n; // Vector de nodos existentes
	int cant_nodos = nodos.size();
	int cant_elem = elementos.size();
	
	//  1) Por cada elemento, agregar el centroide (nuevos nodos: #n a #n+#e-1)
	
	for(size_t i=0;i<cant_elem;i++) { 
		
		// calculamos las coordenadas del centroide
		glm::vec3 centroide = {0,0,0};
		Elemento cara_actual = elementos[i]; 
		
		for(size_t j=0 ; j<cara_actual.nv ; j++) { 
			centroide += nodos[cara_actual[j]].p; // Acumulo todos los nodos de la cara
		}
		centroide = centroide*(1.f/cara_actual.nv); // Divido por el numero de nodos
		nodos.push_back(Nodo(centroide));
	}
	// Dato : Ahora el centroide de la cara i se ubica en nodos[cant_nodos+i]
	mesh.n = nodos;

	
	
	//  2) Por cada arista de cada cara, agregar un pto en el medio que es
	//      promedio de los vertices de la arista y los centroides de las caras 
	//      adyacentes. Aca hay que usar los elementos vecinos.
	//      En los bordes, cuando no hay vecinos, es simplemente el promedio de los 
	//      vertices de la arista
	//      Hay que evitar procesar dos veces la misma arista (como?)
	//      Mas adelante vamos a necesitar determinar cual punto agregamos en cada
	//      arista, y ya que no se pueden relacionar los indices con una formula simple
	//      se sugiere usar Mapa como estructura auxiliar
	
	Mapa mAristas;
	for(int i=0;i<cant_elem;i++) { // Dato: los vecinos estan ordenados acorde a las aristas
		// Recorro las caras sin un orden especifico

		Elemento cara_actual = elementos[i]; // Agarro una cara del vector de elementos

		for(int j=0; j<cara_actual.nv; j++) { 
			// Recorro los nodos de la cara

			Arista a(cara_actual[j],cara_actual[j+1]); // Si cada valor de j le corresponde el vecino[j]

			if (mAristas.find(a) == mAristas.end()){

				mAristas[a] = nodos.size();
				
				glm::vec3 n_a = {0,0,0};
				n_a += nodos[cara_actual[j]].p + nodos[cara_actual[j+1]].p;
				
				if (cara_actual.v[j] == -1){ // Caso frontera, nomas hace el punto medio de la arista
					n_a = 0.5f*n_a;
				} else { // Caso no frontera, promedio ponderado entre los dos puntos del vertice y los centroides 
					n_a += nodos[cant_nodos+cara_actual.v[j]].p + nodos[cant_nodos+i].p;
					n_a = 0.25f*n_a;
				}
				
				nodos.push_back(Nodo(n_a)); // Meto el nuevo nodo
			}
		}
	}
	
	
	///test
	mesh.n = nodos;
	
	//  3) Armar los elementos nuevos
	//      Los quads se dividen en 4, (uno reemplaza al original, los otros 3 se agregan)
	//      Los triangulos se dividen en 3, (uno reemplaza al original, los otros 2 se agregan)
	//      Para encontrar los nodos de las mAristas usar el mapa que armaron en el paso 2
	//      Ordenar los nodos de todos los elementos nuevos con un mismo criterio (por ej, 
	//      siempre poner primero al centroide del elemento), para simplificar el paso 4.
	
	for(int i=0;i<cant_elem;i++) { // Esquizofrenia de estructura de datos de Novara™
		Elemento actual = elementos[i];
		mesh.reemplazarElemento(i,cant_nodos+i,mAristas[Arista(actual[actual.nv-1],actual[0])],actual[0],mAristas[Arista(actual[0],actual[1])]);
		
		for(int j=0;j<actual.nv-1;j++) {
	
			mesh.agregarElemento(cant_nodos+i,mAristas[Arista(actual[j],actual[j+1])],actual[j+1],mAristas[Arista(actual[j+1],actual[j+2])]);
		}
		
	}
	
	mesh.makeVecinos();
	
	
	
	//  4) Calcular las nuevas posiciones de los nodos originales
	//      Para nodos interiores: (4r-f+(n-3)p)/n
	//         f=promedio de nodos interiores de las caras (los agregados en el paso 1)
	//         r=promedio de los pts medios de las mAristas (los agregados en el paso 2)
	//         p=posicion del nodo original
	//         n=cantidad de elementos para ese nodo
	//      Para nodos del borde: (r+p)/2
	//         r=promedio de los dos pts medios de las mAristas
	//         p=posicion del nodo original
	//      Ojo: en el paso 3 cambio toda la SubDivMesh, analizar donde quedan en los nuevos 
	//      elementos (�de que tipo son?) los nodos de las caras y los de las mAristas 
	//      que se agregaron antes.
	
	
	elementos = mesh.e;
	nodos = mesh.n;
	for(int i=0;i<cant_nodos;i++) { 
		Nodo f({0,0,0}),r({0,0,0});
		Nodo nodo_actual = nodos[i];
		int caras_nodo_presente = nodo_actual.e.size(); // Caras en donde aparece el nodo
		
		if (!nodo_actual.es_frontera) { 
			// Caso de nodo_actual que no sea frontera
			
			for(int j=0;j<caras_nodo_presente;j++) { 
				f.p += nodos[elementos[nodo_actual.e[j]][0]].p; // Por ordenamiento Novara™ cuando recorrer las caras este siempre seran los centroides
				r.p += nodos[elementos[nodo_actual.e[j]][1]].p; // Por ordenamiento Novara™ cuando recorrer las caras este siempre seran los puntos interiores
			}
			
			f.p = (1.f/caras_nodo_presente)*f.p; // Divido por la cantidad de caras en las que aparece el nodo para los
			r.p = (1.f/caras_nodo_presente)*r.p; // dos promedios
			
			nodos[i].p = (1.f/caras_nodo_presente)*(4.f*r.p - f.p + (caras_nodo_presente - 3.f)*nodos[i].p); // aplico la formula
		} else {
			if (caras_nodo_presente==1) {
				r.p += nodos[elementos[nodo_actual.e[0]][1]].p;
				r.p += nodos[elementos[nodo_actual.e[0]][3]].p;
			} else {
				for(int j=0;j<caras_nodo_presente;j++) { 
					Elemento aux = elementos[nodo_actual.e[j]];
					if (nodos[aux[1]].es_frontera) {
						r.p += nodos[aux[1]].p;
					} else if (nodos[aux[3]].es_frontera) {
						
						r.p += nodos[aux[3]].p;
					} else {
						std::cout << "no se rey, no trabajo en cuantico" << std::endl;
					}
				}
			}
			
			r.p = 0.5f*r.p;
			
			nodos[i].p = 0.5f*(r.p + nodos[i].p);
		}
		
		
	}
	
	mesh.n = nodos;
	
	
	
	// tips:
	//   no es necesario cambiar ni agregar nada fuera de este m�todo, (con Mapa como 
	//     estructura auxiliar alcanza)
	//   sugerencia: probar primero usando el cubo (es cerrado y solo tiene quads)
	//               despues usando la piramide (tambien cerrada, y solo triangulos)
	//               despues el ejemplo plano (para ver que pasa en los bordes)
	//               finalmente el mono (tiene mezcla y elementos sin vecinos)
	//   repaso de como usar un mapa:
	//     para asociar un indice (i) de nodo a una arista (n1-n2): elmapa[Arista(n1,n2)]=i;
	//     para saber si hay un indice asociado a una arista:  �elmapa.find(Arista(n1,n2))!=elmapa.end()?
	//     para recuperar el indice (en j) asociado a una arista: int j=elmapa[Arista(n1,n2)];
	
	
	
	
	
}

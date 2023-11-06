#include <algorithm>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "ObjMesh.hpp"
#include "Shaders.hpp"
#include "Texture.hpp"
#include "Window.hpp"
#include "Callbacks.hpp"
#include "Model.hpp"
#include <iostream>

#define VERSION 20230925

Shader *shader_coords_ptr = nullptr;
bool show_wireframe = false, show_coords = false, shader_ok = false;

std::vector<glm::vec2> generateTextureCoordinatesForBottle(const std::vector<glm::vec3> &v) {
	/// @todo: generar el vector de coordenadas de texturas para los vertices de la botella
	float max = 0.f;
	for(auto i = 0; i < v.size() ; i++ ){
		if(max < v[i][1]){
			max = v[i][1];
		}
	}
	
	std::vector<glm::vec2> rtn;
	auto ds = 0.65;
	auto pi = 3.1416;
	auto scale = 2.7f;
	
	for(auto i = 0; i < v.size() ; i++ ){
		double x = v[i][0];
		double y = v[i][1];
		double z = v[i][2];
		
		// s es el que va pa la derecha (S es el angulo)
		auto s = atan2(z,x)/(pi);
		
		// t va de arriba a abajo (T es la altura)
		auto t = y/max;
		
		glm::vec2 aux;
		aux[0] = -s;
		aux[1] = -t*scale+ds;
		
		rtn.push_back(aux);
	}
	
	
	return rtn;
}

std::vector<glm::vec2> generateTextureCoordinatesForLid(const std::vector<glm::vec3> &v) {
	/// @todo: generar el vector de coordenadas de texturas para los vertices de la tapa
	
	float miny = 999.f; float maxy = -999.f;
	float minx = 999.f; float maxx = -999.f;
	float minz = 999.f; float maxz = -999.f;
	
	for(auto i = 0; i < v.size() ; i++ ){
		
		///////////////////// X
		
		if(maxx < v[i][0]){
			maxx = v[i][0];
		}
		
		if(minx > v[i][0]){
			minx = v[i][0];
		}
		
		///////////////////// Y
		
		if(maxy < v[i][1]){
			maxy = v[i][1];
		}
	
		if(miny > v[i][1]){
			miny = v[i][1];
		}
		
		///////////////////// Z
		
		if(maxz < v[i][2]){
			maxz = v[i][2];
		}
		
		if(minz > v[i][2]){
			minz = v[i][2];
		}
		
	}
	
	auto DX = maxx - minx;
	auto DZ = maxz - minz;
	
	std::vector<glm::vec2> rtn;
	auto ds = 0.1f;
	//auto pi = 3.1416;
	auto scale = 1.2f;
	
	for(auto i = 0; i < v.size() ; i++ ){
		double x0 = v[i][0];
		double y0 = v[i][1];
		double z0 = v[i][2];
		
		// s es el que va pa la derecha 
		auto z = z0 - minz;
		auto s = z/DZ;
		
		// t va de arriba a abajo 
		auto x = x0 - minx;
		auto t = x/DX;
		
		glm::vec2 aux;
		aux[0] = s*scale-ds;
		aux[1] = -t*scale+1.f+ds;
		
		rtn.push_back(aux);
	}
	
	return rtn;
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods);

bool reload_shader_coords();

int main() {
	
	// initialize window and setup callbacks
	Window window(win_width,win_height,"CG Texturas");
	setCommonCallbacks(window);
	glfwSetKeyCallback(window, keyboardCallback);
	
	// setup OpenGL state and load shaders
	glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
	glEnable(GL_BLEND); glad_glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.6f,0.6f,0.8f,1.f);
	Shader shader_texture("shaders/texture");
	Shader shader_coords("shaders/texture");
	Shader shader_lines("shaders/lines");
	shader_coords_ptr = &shader_coords;
	reload_shader_coords();
	
	// load model and assign texture
	auto models = Model::load("models/bottle",Model::fKeepGeometry|Model::fTextureDontFlipV);
	Model &bottle = models[0], &lid = models[1];
	bottle.buffers.updateTexCoords(generateTextureCoordinatesForBottle(bottle.geometry.positions),true);
	bottle.texture = Texture("models/label.png", Texture::fClampT|Texture::fY0OnTop);
	lid.buffers.updateTexCoords(generateTextureCoordinatesForLid(lid.geometry.positions),true);
	lid.texture = Texture("models/lid.png", Texture::fClampT|Texture::fClampS|Texture::fY0OnTop);
	
	// main loop
	glPolygonOffset(-1.f,1.f);
	glEnable(GL_POLYGON_OFFSET_LINE);
	do {
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		for(int j=0;j<2;++j) {
			Shader &shader = j ? shader_lines : ( show_coords ? shader_coords : shader_texture );
			glPolygonMode(GL_FRONT_AND_BACK,j?GL_LINE:GL_FILL);
			shader.use();
			setMatrixes(shader);
			shader.setLight(glm::vec4{1.f,-1.f,5.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.15f);
			for(Model &mod : models) {
				mod.texture.bind();
				shader.setMaterial(mod.material);
				shader.setBuffers(mod.buffers);
				mod.buffers.draw();
			}
			if (not show_wireframe) break;
		}
		
		// settings sub-window
		window.ImGuiDialog("CG Example",[&](){
			ImGui::Checkbox("Show wireframe (W)",&show_wireframe);
			ImGui::Checkbox("Use shader coords(C)",&show_coords);
			if (ImGui::Button("Reload shader coords (F5)")) reload_shader_coords();
			if (!shader_ok) ImGui::Text("   Error compiling shader coords");
		});
		
		// finish frame
		glfwSwapBuffers(window);
		glfwPollEvents();
		
	} while( glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS && !glfwWindowShouldClose(window) );
}

void keyboardCallback(GLFWwindow* glfw_win, int key, int scancode, int action, int mods) {
	if (action==GLFW_PRESS) {
		switch (key) {
		case 'W': show_wireframe = !show_wireframe; break;
		case 'C': show_coords = !show_coords; break;
		case GLFW_KEY_F5: reload_shader_coords(); break;
		}
	}
}

bool reload_shader_coords() {
	try {
		Shader new_shader("shaders/coords");
		*shader_coords_ptr = std::move(new_shader);
		return (shader_ok = true);
	} catch (std::runtime_error &e) {
		std::cerr << e.what() << std::endl;
		return (shader_ok = false);
	}
}

#include <glm/ext.hpp>
#include <iostream>
#include "Render.hpp"
#include "Callbacks.hpp"

extern bool wireframe, play, top_view, use_helmet;

// matrices que definen la camara
glm::mat4 projection_matrix, view_matrix;

// funci�n para renderizar cada "parte" del auto
void renderPart(const Car &car, const std::vector<Model> &v_models, const glm::mat4 &matrix, Shader &shader) {
	// select a shader
	for(const Model &model : v_models) {
		shader.use();
		
		// matrixes
		if (play) {
			/// @todo: modificar una de estas matrices para mover todo el auto (todas
			///        las partes) a la posici�n (y orientaci�n) que le corresponde en la pista DONE
			
			// NOTA: la clase autito y el modelo del autito est�n separdos (mala pr�ctica) 
			// la clase autito (Car) solamente mueve un un punto en un plano 2d y te devuelve sus coordenadas
			// el modelo del autito y Car tienen coordenadas distintas, hay que aclararle que cuando
			// arrancas a jugar, queres que las coordenadas del modelo del autito esten en las del autito
			
			float scl = 2.f; // no me gustaba como quedaba con 1.f muy chiquito
			float dx = car.x; float dy = 0.00f; float dz = car.y; // aca digo que la posicion del modelo del autito es donde este el autito (car)
			
			glm::mat4 transform( scl, 0.0f, 0.0f, 0.0f, // nuevo eje x
									0.0f, scl, 0.0f , 0.0f, // nuevo eje y
									0.0f, 0.0f, scl , 0.0f, // nuevo eje z
									dx  , dy  , dz  , 1.0f ); // desplazamiento
			
			
			glm::mat4 model_matrix = glm::rotate(transform,-car.ang,glm::vec3{0.f,1.f,0.f})* //le digo capo, rotame el modelo solamente con pivote en y y usa en car.ang como angulo al que rotar
									matrix;
			
			shader.setMatrixes(model_matrix,view_matrix,projection_matrix);
		} else {
			glm::mat4 model_matrix = glm::rotate(glm::mat4(1.f),view_angle,glm::vec3{1.f,0.f,0.f}) *
						             glm::rotate(glm::mat4(1.f),model_angle,glm::vec3{0.f,1.f,0.f}) *
			                         matrix;
			shader.setMatrixes(model_matrix,view_matrix,projection_matrix);
		}
		
		// setup light and material
		shader.setLight(glm::vec4{20.f,40.f,20.f,0.f}, glm::vec3{1.f,1.f,1.f}, 0.35f);
		shader.setMaterial(model.material);
		
		// send geometry
		shader.setBuffers(model.buffers);
		glPolygonMode(GL_FRONT_AND_BACK,(wireframe and (not play))?GL_LINE:GL_FILL);
		model.buffers.draw();
	}
}

// funci�n que actualiza las matrices que definen la c�mara
void setViewAndProjectionMatrixes(const Car &car) {
	projection_matrix = glm::perspective( glm::radians(view_fov), float(win_width)/float(win_height), 0.1f, 100.f );
	if (play) {
		if (top_view) {
			/// @todo: modificar el look at para que en esta vista el auto siempre apunte hacia arriba DONE
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			// Aca le digo che, poneme la camara en
			// autito.pos + vec3{0,30,0}
			// osea simplemente elevo la camara 30 unidades para arriba
			// OJO: el autito en la guia usa las posiciones de x e y, pero en verdad se mueve en x, z.
			
			// El ultimo vector es en el que voy a hacer magia para que la camara rote con el autito
			// basicamente le digo que el "arriba" de la camara va a ser la direccion a la que esta mirando 
			// el autito.
			view_matrix = glm::lookAt( pos_auto+glm::vec3{0.f,30.f,0.f}, pos_auto, glm::vec3{cos(car.ang),0.f,sin(car.ang)}); 
		} else {
			/// @todo: definir view_matrix de modo que la camara persiga al auto desde "atras" DONE
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			// Aca le digo che, poneme la camara en
			// autito.pos - vec3{-10.f*cos(car.ang),3.f,-10.f*sin(car.ang)}
			// 					 rotacion x^ altura y^   rotacion z^
			// OJO: el autito en la guia usa las posiciones de x e y, pero en verdad se mueve en x, z.
			// Despues LookAt te pide un punto al que la camara va a estar mirando. Si haces que mire
			// directamente al autito no se ve un carajo para adelante, asi que elevo el punto 2 
			// unidades arriba de lo que est� el autito para manejar mejor
			//
			// El ultimo vector solamente le meto que el vector que me define para donde est� el "arriba"
			// es efectivamente arriba (los positivos del eje y) 
			view_matrix = glm::lookAt( pos_auto+glm::vec3{-10.f*cos(car.ang),3.f,-10.f*sin(car.ang)}, glm::vec3{car.x,2.f,car.y}, glm::vec3{0.f,1.f,0.f});
		}
	} else {
		view_matrix = glm::lookAt( glm::vec3{0.f,0.f,3.f}, view_target, glm::vec3{0.f,1.f,0.f} );
	}
}

// funci�n que rendiriza todo el auto, parte por parte
void renderCar(const Car &car, const std::vector<Part> &parts, Shader &shader) {
	const Part &axis = parts[0], &body = parts[1], &wheel = parts[2],
	           &fwing = parts[3], &rwing = parts[4], &helmet = parts[use_helmet?5:6];

	/// @todo: armar la matriz de transformaci�n de cada parte para construir el auto DONE
	
	if (body.show or play) { // Cuerpo del autito
		float scl = 0.5f; float dy = 0.1f;
		
		//notar que est� transpuesto a como lo dimos en la teoria
		
		glm::mat4 m( scl, 0.0f, 0.0f, 0.0f, // nuevo eje x
					0.0f, scl, 0.0f, 0.0f, // nuevo eje y
					0.0f, 0.0f, scl, 0.0f, // nuevo eje z
					0.0f, dy, 0.0f, 1.0f ); // desplazamiento
		renderPart(car,body.models,m,shader);
	}
	
	if (wheel.show or play) { // Rueda del autito
		float scl = 0.09f; 
		float dx = 0.f; float dy = 0.f; float dz = 0.f;
		std::cout << "car.rang1 =" << car.rang1 << " car.rang2 =" << car.rang2 << std::endl;
		// para car.range2 en las ruedas izquierdas le puse un - en frente para que roten acorde
		
		//RUEDA DEL FRENTE DERECHA
		//inviero x y z para que la parte fachera de la rueda quede mirando para afuera
		dx = 0.25f; dz= 0.16f; dy = 0.09f;
		glm::mat4 m(-1.f, 0.0f, 0.0f, 0.0f,   // nuevo eje x
					0.0f, 1.f, 0.0f , 0.0f,   // nuevo eje y
					0.0f, 0.0f,-1.f , 0.0f, 
					dx  , dy  , dz  , 1.0f ); 
		
		glm::mat4 m_YX(cos(-car.rang2)  , -sin(-car.rang2), 0.0f , 0.0f,  // nuevo eje x
					  sin(-car.rang2)  , cos(-car.rang2)  , 0.0f , 0.0f,  // nuevo eje y
					  0.0f            , 0.0f              , 1.f  , 0.0f,  // nuevo eje z
					  0.f             , 0.f               , 0.f  , 1.0f ); // desplazamiento
		
		glm::mat4 m_XZ(cos(car.rang1)  , 0.f             , sin(car.rang1)  , 0.0f, // nuevo eje x
					  0.f             , 1.f             , 0.0f            , 0.0f, // nuevo eje y
					  -sin(car.rang1) , 0.0f            , cos(car.rang1)  , 0.0f, // nuevo eje z
					  0.f             , 0.f             , 0.f             , 1.0f ); // desplazamiento
		
		glm::mat4 mE(scl  , 0.0f , 0.0f , 0.0f, // nuevo eje x
					 0.0f , scl  , 0.0f , 0.0f, // nuevo eje y
					 0.0f , 0.0f , scl  , 0.0f, // nuevo eje z
					 0.0f , 0.0f , 0.0f , 1.0f ); // desplazamiento
		
		//glm::mat4 m_YX = glm::rotate(glm::mat4(1.0f), car.rang2, glm::vec3(0.f,0.f,1.f)); // Rota XZ
		//glm::mat4 m_XZ = glm::rotate(glm::mat4(1.0f), car.rang1, glm::vec3(0.f,1.f,0.f)); // Rota YZ
		//glm::mat4 mE = glm::scale(glm::mat4(1.0f), glm::vec3(scl, scl, scl));
		
		m = m * m_XZ * m_YX * mE; // Primero escalado, rotacion y traslacion
		renderPart(car,wheel.models,m,shader); 
		
		//RUEDA DEL FRENTE IZQUIERDA
		dx = 0.25f; dz= -0.16f;
		m=glm::mat4(1.0f, 0.0f, 0.0f , 0.0f, // nuevo eje x
					0.0f, 1.0f, 0.0f , 0.0f, // nuevo eje y
					0.0f, 0.0f, 1.0f , 0.0f, // nuevo eje z
					dx  , dy  , dz   , 1.0f ); // desplazamiento
		
		m_YX=glm::mat4(cos(car.rang2)  , -sin(car.rang2) , 0.0f , 0.0f, // nuevo eje x
					  sin(car.rang2)  , cos(car.rang2)  , 0.0f , 0.0f, // nuevo eje y
					  0.0f             , 0.0f             , 1.f  , 0.0f, // nuevo eje z
					  0.0f             , 0.0f             , 0.0f , 1.0f ); // desplazamiento
		
		m_XZ=glm::mat4(cos(car.rang1)  , 0.0f            , sin(car.rang1)  , 0.0f, // nuevo eje x
					  0.0f            , 1.0f            , 0.0f            , 0.0f, // nuevo eje y
					  -sin(car.rang1) , 0.0f            , cos(car.rang1)  , 0.0f, // nuevo eje z
					  0.0f            , 0.0f            , 0.0f            , 1.0f ); // desplazamiento
		
		mE=glm::mat4(scl  , 0.0f , 0.0f , 0.0f, // nuevo eje x
					 0.0f , scl  , 0.0f , 0.0f, // nuevo eje y
					 0.0f , 0.0f , scl  , 0.0f, // nuevo eje z
					 0.0f , 0.0f , 0.0f , 1.0f ); // desplazamiento
		
		
		//m_YX = glm::rotate(glm::mat4(1.0f), -car.rang2, glm::vec3(0.f,0.f,1.f)); // Rota XZ
		//m_XZ = glm::rotate(glm::mat4(1.0f), car.rang1, glm::vec3(0.f,1.f,0.f)); // Rota YZ
		//mE = glm::scale(glm::mat4(1.0f), glm::vec3(scl, scl, scl));
		m = m * m_XZ * m_YX * mE;
		renderPart(car,wheel.models,m,shader); 
		
		//RUEDA DEL TRASERA IZQUIERDA
		dx = -0.45f; dz= -0.19f; 
		m=glm::mat4( scl, 0.0f, 0.0f, 0.0f, // nuevo eje x
					0.0f, scl, 0.0f , 0.0f, // nuevo eje y
					0.0f, 0.0f, scl , 0.0f, // nuevo eje z
					dx  , dy  , dz  , 1.0f ); // desplazamiento
		
		m_YX=glm::mat4(cos(car.rang2)  , -sin(car.rang2) , 0.0f , 0.0f, // nuevo eje x
					  sin(car.rang2)  , cos(car.rang2)  , 0.0f , 0.0f, // nuevo eje y
					  0.0f             , 0.0f             , 1.f  , 0.0f, // nuevo eje z
					  0.f              , 0.f              , 0.f  , 1.0f ); // desplazamiento
		
		//m = glm::rotate(m,-car.rang2,glm::vec3{0.f,0.f,1.f});
		m = m * m_YX;
		renderPart(car,wheel.models,m,shader); 
		
		//RUEDA DEL FRENTE DERECHA
		//inviero x y z para que la parte fachera de la rueda quede mirando para afuera
		dx = -0.45f; dz= 0.19f; 
		m=glm::mat4(-scl, 0.0f, 0.0f, 0.0f, // nuevo eje x
					0.0f, scl, 0.0f , 0.0f, // nuevo eje y
					0.0f, 0.0f,-scl , 0.0f, // nuevo eje z
					dx  , dy  , dz  , 1.0f ); // desplazamiento
		
		m_YX=glm::mat4(cos(-car.rang2)   , -sin(-car.rang2)  , 0.0f , 0.0f, // nuevo eje x
					  sin(-car.rang2)   , cos(-car.rang2)   , 0.0f , 0.0f, // nuevo eje y
					  0.0f             , 0.0f             , 1.f  , 0.0f, // nuevo eje z
					  0.f              , 0.f              , 0.f  , 1.0f ); // desplazamiento
		m = m * m_YX;
		//m = glm::rotate(m,car.rang2,glm::vec3{0.f,0.f,1.f});
		renderPart(car,wheel.models,m,shader); 
	}
	
	if (fwing.show or play) { // Aleron delantero del autito
		float scl = 0.23f;
		float dx = 0.44f; float dy = 0.05f; float dz = 0.f;
		
		glm::mat4 m(0.0f, 0.0f, scl , 0.0f, // nuevo eje x
					0.0f, scl , 0.0f, 0.0f, // nuevo eje y
					-scl, 0.0f, 0.0f, 0.0f, // nuevo eje z
					dx  , dy  , dz  , 1.0f ); // desplazamiento
		renderPart(car,fwing.models,m,shader);
	}
	
	if (rwing.show or play) { //  Aleron trasero del autito 
		float scl = 0.20f;
		float dx = -0.42f; float dy = 0.23f; float dz = 0.f;
		
		glm::mat4 m(0.0f, 0.0f, scl , 0.0f, // nuevo eje x
					0.0f, scl , 0.0f, 0.0f, // nuevo eje y
					-scl, 0.0f, 0.0f, 0.0f, // nuevo eje z
					dx  , dy  , dz  , 1.0f ); // desplazamiento
		renderPart(car,rwing.models,m,shader);
	}
	
	if (helmet.show or play) { // Cascote
		float scl = 0.05f;
		float dx = 0.f; float dy = 0.17f; float dz = 0.f;
		
		glm::mat4 m(0.0f, 0.0f, scl , 0.0f, // nuevo eje x
					0.0f, scl , 0.0f, 0.0f, // nuevo eje y
					-scl, 0.0f, 0.0f, 0.0f, // nuevo eje z
					dx  , dy  , dz  , 1.0f ); // desplazamiento
		renderPart(car,helmet.models,m,shader);
	}
	
	if (axis.show and (not play)) renderPart(car,axis.models,glm::mat4(1.f),shader);
}

// funci�n que renderiza la pista
void renderTrack() {
	static Model track = Model::loadSingle("track",Model::fDontFit);
	static Shader shader("shaders/texture");
	shader.use();
	shader.setMatrixes(glm::mat4(1.f),view_matrix,projection_matrix);
	shader.setMaterial(track.material);
	shader.setBuffers(track.buffers);
	track.texture.bind();
	static float aniso = -1.0f;
	if (aniso<0) glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	track.buffers.draw();
}

void renderShadow(const Car &car, const std::vector<Part> &parts) {
	static Shader shader_shadow("shaders/shadow");
	glEnable(GL_STENCIL_TEST); glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_EQUAL,0,~0); glStencilOp(GL_KEEP,GL_KEEP,GL_INCR);
	renderCar(car,parts,shader_shadow);
	glDisable(GL_STENCIL_TEST);
}

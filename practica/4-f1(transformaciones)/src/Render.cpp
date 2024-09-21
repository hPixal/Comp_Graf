#include <glm/ext.hpp>
#include "Render.hpp"
#include "Callbacks.hpp"

extern bool wireframe, play, top_view, use_helmet;

// matrices que definen la camara
glm::mat4 projection_matrix, view_matrix;

// función para renderizar cada "parte" del auto
void renderPart(const Car &car, const std::vector<Model> &v_models, const glm::mat4 &matrix, Shader &shader) {
	// select a shader
	for(const Model &model : v_models) {
		shader.use();
		
		// matrixes
		if (play) {
			/// @todo: modificar una de estas matrices para mover todo el auto (todas
			///        las partes) a la posición (y orientación) que le corresponde en la pista
			glm::mat4 trans(1.0f, 0.0f , 0.0f, 0.0f,
							0.0f, 1.0f , 0.0f, 0.0f,
							0.0f, 0.0f , 1.0f, 0.0f,
							car.x,0.0f, car.y, 1.0f);
			
			
			float gamma = car.ang;
			
			glm::mat4 rotsides (cos(gamma), 0.0f , sin(gamma), 0.0f,
								0.0f, 1.0f , 0.0f, 0.0f,
								-1*sin(gamma), 0.0f ,cos(gamma), 0.0f,
								0.0f, 0.0f, 0.0f, 1.0f);
			
			glm::mat4 model_matrix = trans*rotsides*matrix;
			
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

// función que actualiza las matrices que definen la cámara
void setViewAndProjectionMatrixes(const Car &car) {
	projection_matrix = glm::perspective( glm::radians(view_fov), float(win_width)/float(win_height), 0.1f, 100.f );
	if (play) {
		if (top_view) {
			/// @todo: modificar el look at para que en esta vista el auto siempre apunte hacia arriba
			glm::vec3 pos_auto = {car.x, 0.f, car.y};
			view_matrix = glm::lookAt( pos_auto+glm::vec3{0.f,20.f,0.f}, pos_auto, glm::vec3{0.f,0.f,1.f} );
		} else {
			/// @todo: definir view_matrix de modo que la camara persiga al auto desde "atras"
			glm::vec3 pos_auto = {car.x, 1.35f, car.y};
			auto rad = 4.5f;
			view_matrix = glm::lookAt( glm::vec3{car.x-rad*cos(car.ang),2.f,car.y-rad*sin(car.ang)}, pos_auto, glm::vec3{0.f,1.f,0.f} );
		}
	} else {
		view_matrix = glm::lookAt( glm::vec3{0.f,0.f,3.f}, view_target, glm::vec3{0.f,1.f,0.f} );
	}
}

// función que rendiriza todo el auto, parte por parte
void renderCar(const Car &car, const std::vector<Part> &parts, Shader &shader) {
	const Part &axis = parts[0], &body = parts[1], &wheel = parts[2],
	           &fwing = parts[3], &rwing = parts[4], &helmet = parts[use_helmet?5:6];

	/// @todo: armar la matriz de transformación de cada parte para construir el auto
	
	if (body.show or play) {
		float res = 1.0f;
		
		glm::mat4 mres (res , 0.0f, 0.0f, 0.0f,
						0.0f, res , 0.0f, 0.0f,
						0.0f, 0.0f, res , 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
		
		glm::mat4 mpos (1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						0.0f, 0.22f, 0.0f, 1.0f);
		
		renderPart(car,body.models,mpos*mres,shader);
	}
	
	if (wheel.show or play) {
		
		// Wheel Rotation 1
		
		float gamma = car.rang1;
		
		glm::mat4 rotsides (cos(gamma), 0.0f , sin(gamma), 0.0f,
					   0.0f, 1.0f , 0.0f, 0.0f,
					   -1*sin(gamma), 0.0f ,cos(gamma), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
		
		// Wheel Rotation 2
		
		float omega = car.rang2;
		
		glm::mat4 rotfor (cos(omega),-sin(omega) , 0.0f, 0.0f,
					   sin(omega), cos(omega) , 0.0f, 0.0f,
					   0.0f, 0.0f , 1.0f, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
		
		
		float res = 0.2f;
		
		glm::mat4 scale (res , 0.0f, 0.0f, 0.0f,
						0.0f, res , 0.0f, 0.0f,
						0.0f, 0.0f, res , 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f);
		
		// Pos Front-left
		glm::mat4 trans(1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						0.5f, 0.18f, -0.35f, 1.0f);
		
		
		renderPart(car,wheel.models,trans*rotsides*rotfor*scale,shader); 
		
		// Pos Back-left
		trans= glm::mat4(1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						-0.9f, 0.18f , -0.42f, 1.0f);
		
		renderPart(car,wheel.models,trans*rotfor*scale,shader); 
		
		// Pos Back-Right
		
		trans= glm::mat4(1.0f, 0.0f , 0.0f, 0.0f,
						 0.0f, 1.0f , 0.0f, 0.0f,
						 0.0f, 0.0f , 1.0f, 0.0f,
						 -0.9f, 0.18f , 0.42f, 1.0f);
		
		glm::mat4 rot (-1.0f, 0.0f , 0.0f, 0.0f,
						 0.0f, 1.0f , 0.0f, 0.0f,
						 0.0f, 0.0f ,-1.0f, 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);
		
		glm::mat4 finalMatrix = trans * rotfor * rot * scale;
		
		renderPart(car,wheel.models,finalMatrix,shader); 
		
		// Pos Front-Right
		
		trans=glm::mat4(1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						0.5f, 0.18f, 0.35f, 1.0f);
		
		rot=glm::mat4(-1.0f, 0.0f , 0.0f, 0.0f,
					   0.0f, 1.0f , 0.0f, 0.0f,
					   0.0f, 0.0f ,-1.0f, 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
		
		finalMatrix = trans * rotsides * rotfor * rot * scale;
		
		renderPart(car,wheel.models,finalMatrix,shader); 
	}
	
	if (fwing.show or play) {
		float res = 0.3f;
		
		glm::mat4 scale (res , 0.0f, 0.0f, 0.0f,
						 0.0f, res , 0.0f, 0.0f,
						 0.0f, 0.0f, res , 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);
		
		glm::mat4 trans(1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						0.9f, 0.14f, 0.0f, 1.0f);
		
		float theta = 90.f;
		theta = theta*2*3.14159265359*1/360;
		
		glm::mat4 rot (cos(theta), 0.0f , sin(theta), 0.0f,
					   0.0f, 1.0f , 0.0f, 0.0f,
					   -1*sin(theta), 0.0f ,cos(theta), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
		
		renderPart(car,fwing.models,trans*rot*scale,shader);
	}
	
	if (rwing.show or play) {
		float res = 0.30f;
		glm::mat4 scale (res , 0.0f, 0.0f, 0.0f,
						 0.0f, res , 0.0f, 0.0f,
						 0.0f, 0.0f, res , 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);
		
		glm::mat4 trans(1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						-0.9f, 0.4f, 0.0f, 1.0f);
		
		float theta = 90.f;
		theta = theta*2*3.14159265359*1/360;
		
		glm::mat4 rot (cos(theta), 0.0f , sin(theta), 0.0f,
					   0.0f, 1.0f , 0.0f, 0.0f,
					   -1*sin(theta), 0.0f ,cos(theta), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
		
		renderPart(car,rwing.models,trans*rot*scale,shader);
	}
	
	if (helmet.show or play) {
		float res = 0.10f;
		glm::mat4 scale (res , 0.0f, 0.0f, 0.0f,
						 0.0f, res , 0.0f, 0.0f,
						 0.0f, 0.0f, res , 0.0f,
						 0.0f, 0.0f, 0.0f, 1.0f);
		
		glm::mat4 trans(1.0f, 0.0f , 0.0f, 0.0f,
						0.0f, 1.0f , 0.0f, 0.0f,
						0.0f, 0.0f , 1.0f, 0.0f,
						0.0f, 0.35f, 0.0f, 1.0f);
		
		float theta = 90.f;
		theta = theta*2*3.14159265359*1/360;
		
		glm::mat4 rot (cos(theta), 0.0f , sin(theta), 0.0f,
					   0.0f, 1.0f , 0.0f, 0.0f,
					   -1*sin(theta), 0.0f ,cos(theta), 0.0f,
					   0.0f, 0.0f, 0.0f, 1.0f);
		
		renderPart(car,helmet.models,trans*rot*scale,shader);
	}
	
	if (axis.show and (not play)) renderPart(car,axis.models,glm::mat4(1.f),shader);
}

// función que renderiza la pista
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

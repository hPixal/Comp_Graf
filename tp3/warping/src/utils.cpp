#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "utils.hpp"
#include "Debug.hpp"

BoundingBox::BoundingBox(glm::vec3 &p1, glm::vec3 &p2) 
	: pmin({std::min(p1.x,p2.x),std::min(p1.y,p2.y),std::min(p1.z,p2.z)}),
      pmax({std::max(p1.x,p2.x),std::max(p1.y,p2.y),std::max(p1.z,p2.z)}) 
{
	
}
	
bool BoundingBox::contiene(glm::vec3 &p) const {
	return p.x>=pmin.x && p.x<=pmax.x &&
		p.y>=pmin.y && p.y<=pmax.y &&
		p.z>=pmin.z && p.z<=pmax.z;
}

Pesos calcularPesos(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2, glm::vec3 &x) {
	/// @todo: implementar
	
	glm::vec3 x01 = x1-x0;
	glm::vec3 x12 = x2-x1;
	glm::vec3 x20 = x0-x2;
	
	glm::vec3 x0x = x-x0;
	glm::vec3 x1x = x-x1;
	glm::vec3 x2x = x-x2;
	
	float areaTotal2 = x01[0]*(-x20[1])-x01[1]*(-x20[0]);
	
	float alpha2 = (x01[0]*x0x[1]-x01[1]*x0x[0])/areaTotal2;
	float alpha0 = (x12[0]*x1x[1]-x12[1]*x1x[0])/areaTotal2;
	float alpha1 = (x20[0]*x2x[1]-x20[1]*x2x[0])/areaTotal2;
	
	//cg_error("debe implementar la funcion calcularPesos (utils.cpp)");
	std::cout << alpha0 << alpha1 << alpha2 ;
	return {alpha0,alpha1,alpha2};
}

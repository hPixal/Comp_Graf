#include <forward_list>
#include <iostream>
#include <GL/gl.h>
#include <cmath>
#include "RasterAlgs.hpp"

/*
	paintPixel dibuja el pixel en la coordenada que le digas.
	p0 es el punto inicial
	p1 es el punto final
*/
void bresenhamBigM(paintPixelFunction paintPixel,float x0, float y0,float x1, float y1){
	auto dx = x1 - x0;
	auto dy = y1 - y0;
	auto xi = 1;
	
	//Check if slope is negative
	if( dx < 0 ){
		xi = -1; // if it is, we should go down each step
		dx = - dx;
	}
	auto D = (2 * dx) - dy;
	auto x = x0;
	
	for (auto y = y0; y < y1; y++){
		glm::vec2 n_pixel(x,y);
		paintPixel(n_pixel);
		if ( D > 0 ) {
			x = x + xi; // If the difference in the midpoint here is bigger than
			// 0 it means its above  the midpoint so we should paint
			// the pixel above
			D = D + (2 * (dx - dy)); // sum a new difference 
		}else{
			D = D + 2*dx;
		}
	}
}
	
void bresenhamSmallM(paintPixelFunction &paintPixel,float x0, float y0,float x1, float y1){
	auto dx = x1 - x0;
	auto dy = y1 - y0;
	auto yi = 1;
		
		//Check if slope is negative
		if( dy < 0 ){
			yi = -1; // if it is, we should go down each step
			dy = - dy;
		}
		auto D = (2 * dy) - dx;
		auto y = y0;
		
		for (auto x = x0; x < x1; x++){
			glm::vec2 n_pixel(x,y);
			paintPixel(n_pixel);
			if ( D > 0 ) {
				y = y + yi; // If the difference in the midpoint here is bigger than
				// 0 it means its above  the midpoint so we should paint
				// the pixel above
				D = D + (2 * (dy - dx)); // sum a new difference 
			}else{
				D = D + 2*dy;
			}
		}
}

void drawSegment(paintPixelFunction paintPixel, glm::vec2 p0, glm::vec2 p1) {
	/// @todo: implementar algun algoritmo de rasterizacion de segmentos 
	
	// Bresenham Line algorithm
	
	//std::cout << "x0 , x1 : " << p0.x << " , " << p1.x << std::endl;
	//std::cout << "y0 , y1 : " << p0.x << " , " << p1.x << std::endl;
	
	// 1 - First check if the order is correct
	
	if(p0.x > p1.x){
		auto aux = p0;
		p0 = p1;
		p1 = aux;
	}
	
	
	// 2 - Get slope
	
	auto dx = p1.x - p0.x;
	auto dy = p1.y - p0.y;
	auto m = dy/dx;
	
	// 3 - Generate lerp
	std::cout << "m : " << m << std::endl;
	if(abs(m) >= 1)
	{
		if(p0.y > p1.y){
			auto aux = p0;
			p0 = p1;
			p1 = aux;
		}
		std::cout << "m big : "<< m << std::endl;
		bresenhamBigM(paintPixel,p0.x, p0.y,p1.x,p1.y);
	}else
	{
		std::cout << "m small : "<< m << std::endl;
		bresenhamSmallM(paintPixel,p0.x,p0.y,p1.x,p1.y);
	}
}



/*
void drawSegment(paintPixelFunction paintPixel, glm::vec2 p0, glm::vec2 p1) {
	/// @todo: implementar algun algoritmo de rasterizacion de segmentos 
	
	// Bresenham Line algorithm
	
	// y = dy/dx x + b
	// - dy/dx x = - y + b
	// - dy x = - dx y + dx b
	// 0 = (dy)x - (dx)y + (dx)b
	
	// dy = y1 - y0
	// dx = x1 - x0
	
	// 0 = (dy)x + (-dx)y + (dx)b
	
	// A = dy = y1 - y0
	// B = (-dx) = x0 - x1
	// C = (dx)b
	
	// y - y0 = dy/dx ( x - x0 )
	// y = dy/dx x + y0 - dy/dx x0
	// b = y0 - dy/dx x0
	
	if (p0.x > p1.x){
		auto aux = p0;
		p0 = p1;
		p1 = aux;
	}
	
	auto dy = p1.y - p0.y;
	auto dx = p1.x - p0.x;
	auto m = dy/dx;
	auto b = (dy/dx)*p0.x;
	int incr = 1;
	
	if(dy < 0){
		incr = -1;
		dy = -dy;
	}
	
	auto A = dy;
	auto B = -dx;
	auto C = dx*b;
	auto D = 2*dy - dx;
	
	auto midpointX = (p0.x + p1.x)/2;
	auto midpointY = (p0.y + p1.y)/2;
	
	float y = p0.y;
	
	for ( auto x = p0.x ; x < p1.x ; x++){
		
		if (D > 0){
			D = D + (2 * (dy - dx));
			if(round(m)>1){
				for( auto i = 1 ; i < round(m) ; i++){
					paintPixel(glm::vec2(x,y+i));
				}
			}
			if(round(m)<-1){
				for( auto i = round(m)+1 ; i < 0 ; i++){
					paintPixel(glm::vec2(x,y+i));
				}
			}
			y = y + round(m);
		}
		else{
			D = D + 2*dy;
		}
		
		std::cout << "x , y : " << x << " , " << y << std::endl;
		paintPixel(glm::vec2(x,y));
	}
	
	
}
*/
#if 1

/*
paintPixel dibuja el pixel en la coordenada
evalCurve es una funcion paramétrica que devuelve una coordenada 2d
*/
void drawCurve(paintPixelFunction paintPixel, curveEvalFunction evalCurve) {
	/// @todo: implementar algun algoritmo de rasterizacion de curvas
	//DDA de curvas
	float t=0.f;
	
	//Bandera para evitar repintar
	bool pintar = true;
	
	/*
	t: parametro que recibe la funcion parametrica de la curva, devuelve un par <punto, derivada>
	t = [0.0, 1.0]
	t=0.0 punto inicial
	t=1.0 punto final
	*/
	while(t <= 1.f){
		
		//p = par (punto, derivada) del punto evaluado
		auto p = evalCurve(t);
		
		//Pintar pixel donde cae p (recondeado)
		if(pintar)
			paintPixel(glm::vec2(round(p.p.x),round(p.p.y)));
		
		pintar = true;
		
		//t2: proximo t
		float t2 = t;
		//dt: incremento en t (t2 - t)
		float dt = 0.f;
		
		//Evaluamos hacia donde crece mas la curva (p.d es la derividada p.p de la curva)
		if(fabs(p.d.x)>fabs(p.d.y)){
			//Candidato a incremento
			dt = 1.f/fabs(p.d.x);
		}
		else{
			dt = 1.f/fabs(p.d.y);
		}
		
		///Correccion para no saltear pixeles
		//Se ejecuta hasta que el incremento es lo suficientemente chico
		//Suficientemente chico:
		//- No crece 2 o mas pixeles en x
		//- No crece 2 o mas pixeles en y
		do{
			//Definimos t2
			t2 = t + dt;
			
			//Refinamos dt (se aplica si se ejecuta una iteracion mas)
			dt *= 0.5f;
			
			//Se vuelve a ejecutar si la diferencia entre el redondeo de p.p en t y en t2 es mayor a 1 (se saltea un pixel en x y/o y)
		}while(abs(round(evalCurve(t2).p.x) - round(p.p.x)) > 1 || abs(round(evalCurve(t2).p.y) - round(p.p.y)) > 1);
		
		///Correccion de repintado (si el redonde de p.p en t y en t2 son iguales)
		if(glm::vec2(round(evalCurve(t2).p.x),round(evalCurve(t2).p.y)) == glm::vec2(round(p.p.x),round(p.p.y))){
			pintar = false;
		}
		
		t = t2;
	}
	
}

#else

///Subdivision (corregir)
void drawCurve(paintPixelFunction paintPixel, curveEvalFunction evalCurve, float t0, float t1){
	//Punto medio de la secante
	glm::vec2 secMed = (evalCurve(t0).p + evalCurve(t1).p) * 0.5f;
	
	//Punto medio de la curva
	float tmed = (t0+t1)/2.f;
	glm::vec2 curveMed = evalCurve(tmed).p;
	
	glm::vec2 secm= normalize(evalCurve(t1).p - evalCurve(t0).p);
	glm::vec2 tmedm= normalize(evalCurve(tmed).d);
	//Ver si es lo suficientemente copada(la distancia entre los puntos medio es menor a 1 y la pendiente en tmed es parecida a la pendiente de la secante)
	if(dot(secm,tmedm) < 0.9f){
		drawCurve(paintPixel, evalCurve, t0, tmed);
		drawCurve(paintPixel, evalCurve, tmed, t1);
	}else if(distance(secMed, curveMed) >= 1){
		drawCurve(paintPixel, evalCurve, t0, tmed);
		drawCurve(paintPixel, evalCurve, tmed, t1);
	}else{
		drawSegment(paintPixel, evalCurve(t0).p, evalCurve(t1).p);
	}
}
	
	//Wrapper para subdivision
	void drawCurve(paintPixelFunction paintPixel, curveEvalFunction evalCurve) {
		drawCurve(paintPixel, evalCurve, 0.f, 1.f);
	}
	
#endif
	

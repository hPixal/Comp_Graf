#version 330 core

// propiedades del material
uniform vec3 objectColor;
uniform float shininess;
uniform float specularStrength;

// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;
uniform vec4 lightPosition;

// propiedades de la camara
uniform vec3 cameraPosition;

// propiedades del fragmento interpoladas por el vertex shader
in vec3 fragNormal;
in vec3 fragPosition;

// resultado
out vec4 fragColor;

// phong simplificado
void main() {
	
	// ambient
	vec3 ambient = lightColor * ambientStrength * objectColor ;
	
	// diffuse
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(vec3(lightPosition)); // luz directional (en el inf.)
	float ang = dot(norm,lightDir);
	vec3 diffuse = lightColor * objectColor * (step(0.6,ang)/3 + step(0.15,ang)/2);
	/*
	if (ang <= 0) {
		diffuse *= 0;
	} else if (ang < 0.5) {
		diffuse *= 0.4;
	} else {
		diffuse *= 0.8;
	}
	*/
	
	
	// specular
	vec3 specularColor = specularStrength * vec3(1.f,1.f,1.f);
	vec3 viewDir = normalize(cameraPosition-fragPosition);
	vec3 halfV = normalize(lightDir + viewDir); // blinn
	vec3 specular = lightColor * specularColor * pow(max(dot(norm,halfV),0.f),shininess);
	
	// result
	fragColor = vec4(ambient+diffuse+specular,1.f);
}


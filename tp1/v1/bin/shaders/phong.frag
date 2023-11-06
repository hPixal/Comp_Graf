#version 330 core

// propiedades del material
uniform vec3 color;
uniform float shininess;
uniform float specular_strength;

// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;

// propiedades del fragmento interpoladas por el vertex shader
in vec3 fragNormal;
in vec3 fragPosition;
in vec4 lightVSPosition;

// resultado
out vec4 fragColor;

// phong simplificado
void main() {
	
	// ambient
	vec3 ambient = lightColor * ambientStrength;
	
	// diffuse
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(vec3(lightVSPosition)); // directional
	vec3 diffuse = max(dot(norm,lightDir),0.f) * lightColor;
	
	// specular
	vec3 cameraPosition = vec3(0.f,0.f,0.f);
	vec3 viewDir = normalize(cameraPosition-fragPosition);
	vec3 reflectDir = reflect(-lightDir,norm);
	vec3 halfV = normalize(lightDir + viewDir);
	vec3 specular = pow(max(dot(norm,halfV),0.f),shininess) * lightColor; // blinn
	
	// result
	vec3 white = vec3(1.f,1.f,1.f);
	fragColor = vec4(color*(ambient+diffuse)+specular*specular_strength*white,1.f);
}


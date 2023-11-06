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
	
	vec3 norm = normalize(fragNormal); //Define la normal del objeto con modulo uno.
	vec3 lightDir = normalize(vec3(lightPosition)); // luz directional (en el inf.) con modulo uno. En el infinito debido a que toda la luz incide con el mismo angulo
	vec3 diffuse = lightColor * objectColor *  
	( (max(dot(norm,lightDir),0.f) < 0.5) ? ( (max(dot(norm,lightDir),0.f) < 0.05)  ? 0.1 : 0.5  ) : 0.9);
	// Lightdir (l_i) y la normal (n) del objeto como estan normalizadas su producto punto es queivalente a cos(theta)
	// ObjectColor viene a ser color de la difusa K_d
	// LightColor viene a ser la luz incidente L_i
	// La ecuacion final es K_d*L_d* ( n . l_i) que es equivalente a la ecuacion de la difusa lambertiana ideal
	
	// specular
	vec3 specularColor = specularStrength * vec3(1.f,1.f,1.f);
	vec3 viewDir = normalize(cameraPosition-fragPosition);
	vec3 halfV = normalize(lightDir + viewDir); // blinn
	vec3 specular = lightColor * specularColor * pow(max(dot(norm,halfV),0.f),shininess);
	// halfV (l_i) y la normal (n) del objeto como estan normalizadas su producto punto es queivalente a cos(theta)
	// shininess viene a ser q
	// specularColor es K_s que es el color especular
	// viewDir viene a ser v que se va a usar por triangulos semejantes para definir halfV
	// La ecuacion final es L_s = K_s * L_i * (n . h)^q
	
	// result
	fragColor = vec4(ambient+diffuse+specular,1.f);
}


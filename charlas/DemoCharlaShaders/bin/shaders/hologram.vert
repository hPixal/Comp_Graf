#version 330 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec4 lightPosition;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec4 lightVSPosition;

// user parameters
uniform float time;

void main() {
	
	// 1) Modify VertexPosition to apply a sine movement to Y coord
	// 2) Rotate all vertex around Y axis
	// 3) define a new variable using a float Uniform to controll the animation speed
	
	mat4 vm = viewMatrix * modelMatrix;
	vec4 vmp = vm * vec4(vertexPosition, 1.f);
	
	gl_Position = projectionMatrix * vmp;
	fragPosition = vec3(vmp);
	fragNormal = mat3(transpose(inverse(vm))) * vertexNormal;
	lightVSPosition = viewMatrix * lightPosition;
	fragTexCoords = vertexTexCoords;
}

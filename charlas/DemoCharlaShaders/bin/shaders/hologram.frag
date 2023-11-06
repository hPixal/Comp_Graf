# version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoords;
in vec4 lightVSPosition;

// propiedades del material
uniform vec3 ambientColor;
uniform vec3 specularColor;
uniform vec3 diffuseColor;
uniform vec3 emissionColor;
uniform float opacity;
uniform float shininess;
// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;


//User shader parameters
uniform float time;
uniform float freq;
uniform float tresh;
uniform float sine_speed;
uniform float test04;

uniform sampler2D cookity_tex;
uniform sampler2D noise_tex;

uniform vec2 uv_scale;
uniform vec2 uv_scroll;

uniform vec4 color01;

// Shader output
out vec4 fragColor;

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}
	
void main() {

	// 1) Sample textures
	//vec4 chockityColor = texture(cho)
	// 1.5) Scroll texture
	// 2) Interpolate colors
	// 3) Draw sine
	// 4) Discard pixels using sine value
	// 5) Use rand() function to add noise
	
	fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
}


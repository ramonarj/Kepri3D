#version 330 core 

// Variables de entrada
in DATA
{
	vec2 TexCoords; 
} data_in;

// Textura con la escena
uniform sampler2D RTT0;

// Depth buffer
uniform sampler2D depthBuf;
#include utils.glsl

// Niebla
uniform vec4 fog_color = { 0.5, 0.2, 1.0, 1.0};
const float fog_start = 1.0f;
const float fog_end = 100.0f;

out vec4 fragColor;

void main()
 {
	// Leer el color
	vec4 sceneColor = texture(RTT0, data_in.TexCoords);
	// Leer la profundidad
	float depthValue = texture(depthBuf, data_in.TexCoords).r;
	
	// Calcular la niebla
	float fogAmount = LinearizeDepth(depthValue, fog_start, fog_end) / fog_end; //perspective
	fragColor = mix(sceneColor, fog_color, fogAmount);
 }
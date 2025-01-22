#version 330 core 

// Variables de entrada
in DATA
{
	vec2 TexCoords; 
} data_in;

// Textura con la escena
uniform sampler2D RTT0;

out vec4 fragColor;

void main()
 {
	fragColor = texture(RTT0, data_in.TexCoords);
 }
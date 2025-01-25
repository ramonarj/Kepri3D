#version 330 core 

// Variables de entrada
in DATA
{
	vec2 TexCoords; 
} data_in;

// Textura con la escena
uniform sampler2D RTT0;

out vec4 fragColor;

// Parámetros de la onda
// Con valores de 0.01 y 2.0 (respect.), sirve para simular cristal
const float amplitudOnda = 0.003f;
const float frecuenciaOnda = 0.5f;

void main()
 {
	vec2 offset = { amplitudOnda * sin(gl_FragCoord.y * frecuenciaOnda), amplitudOnda * sin(gl_FragCoord.x * frecuenciaOnda)};
	
	vec4 pxColor = texture(RTT0, data_in.TexCoords + offset);
	fragColor = pxColor;
 }
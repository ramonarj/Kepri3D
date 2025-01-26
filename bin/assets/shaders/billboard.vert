#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 1) in vec4 aColores;
layout(location = 2) in vec2 uv0;

// Variables uniform
uniform dmat4 model;
uniform dmat4 view;
uniform dmat4 projection;

uniform vec2 ancla;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec4 colores;
} data_out;

void main()
{
	// Muy similar al de 'particle.vert'. Aquí sí que usamos la matriz de modelado, pero la última columna (posición)
	gl_Position = vec4(projection * (view * model[3] + vec4(vertex.xy - ancla, 0, 0))); 
	
	data_out.TexCoords = uv0;
	data_out.colores = aColores;
}
#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 1) in vec4 aColores;
layout(location = 2) in vec2 uv0;

uniform dmat4 model;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec4 colores;
} data_out;

void main()
{
	//gl_Position = vec4(model * vec4(vertex.xyz * 0.5, 1.0));
	gl_Position = vec4(model * vertex);
	
	data_out.TexCoords = uv0;
	data_out.colores = aColores;
}
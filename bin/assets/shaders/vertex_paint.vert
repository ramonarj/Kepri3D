#version 430 core

// Este shader no usa las normales, sino que usa el color de los vértices para 
// mezclarlo con la textura aplicada, simulando iluminación 

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 uv0;

// Uniform block
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};
uniform dmat4 model;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec4 color;
} data_out;

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.TexCoords = uv0;
	data_out.color = color;
}
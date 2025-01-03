#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
} data_out;

void main()
{
	gl_Position = vertex;
	
	data_out.TexCoords = uv0;
}
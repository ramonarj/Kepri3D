#version 400 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec3 aNormals;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
} data_out;

// No hace absolutamente nada, solo pasar valores
void main()
{
	gl_Position = vertex;
	data_out.TexCoords = uv0;
	data_out.normals = aNormals;
}
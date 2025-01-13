#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec3 aNormals;

// Variables uniform
uniform dmat4 model;
uniform dmat4 view;
uniform dmat4 projection;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
	vec3 fragPos;
} data_out;

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.TexCoords = uv0;
	data_out.normals = aNormals;
	// Posición del fragmento en el espacio global
	data_out.fragPos = vec3(model * vertex);
}
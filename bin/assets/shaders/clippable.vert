#version 430 core

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

// Variables uniform
uniform dmat4 model;
uniform dmat4 view;
uniform dmat4 projection;

// Planos de corte
const int MAX_PLANOS = 8;
uniform vec4 planoCorte[MAX_PLANOS];

// Redeclarar el tamaño del array
out float gl_ClipDistance[MAX_PLANOS];

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.TexCoords = uv0;
	data_out.normals = aNormals;
	
	// Distancia al plano de corte
	for(int i = 0; i < MAX_PLANOS; i++)
		gl_ClipDistance[i] = dot(planoCorte[i], vertex);
}
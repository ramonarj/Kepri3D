#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec3 aNormals;

// Variables uniform
uniform dmat4 mvpMat;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
} data_out;

void main()
{
	gl_Position = vec4(mvpMat * vertex);
	
	data_out.TexCoords = uv0;
	data_out.normals = aNormals;
}
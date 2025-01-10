#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 1) in vec4 aColores;
layout(location = 2) in vec2 uv0;

layout(location = 10) in vec3 position;

// Variables uniform
uniform dmat4 viewProjMat;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec4 colores;
} data_out;

void main()
{
	gl_Position = vec4(viewProjMat * (vertex + vec4(position.xyz, 0.0)));
	//gl_Position = vec4(mvpMat * vertex);
	
	data_out.TexCoords = uv0;
	data_out.colores = aColores;
	//data_out.colores = aColores * vec4(gl_InstanceID / 1728.0);
}
#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec3 aNormals;

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
	vec3 normals;
} data_out;

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.TexCoords = uv0;
	data_out.normals = aNormals;
}
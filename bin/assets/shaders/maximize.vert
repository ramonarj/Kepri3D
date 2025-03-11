#version 430 core

in vec4 vertex;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec3 aNormals;

out DATA
{	
	vec2 TexCoords;
	vec3 normals;
} data_out;

// Uniform block
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};
uniform dmat4 model;

void main()
{
	gl_Position = vec4(projection * view * model * vec4(vertex.x * 2.0, vertex.y * 2.0, vertex.z * 2.0, 1.0));
	data_out.TexCoords = uv0;
	data_out.normals = aNormals;
}
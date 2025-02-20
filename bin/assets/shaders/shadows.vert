#version 430 core

in vec4 vertex;

// Uniform block
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};
uniform dmat4 model;

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
}
#version 430 core

in vec4 vertex;

uniform dmat4 model;

void main()
{
	gl_Position = vec4(model * vertex);
}
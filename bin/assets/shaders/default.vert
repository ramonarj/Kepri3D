#version 430 core

in vec4 vertex;
uniform dmat4 mvpMat;

void main()
{
	gl_Position = vec4(mvpMat * vertex);
}
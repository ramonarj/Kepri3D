#version 430 core

in vec4 vertex;
uniform dmat4 mvpMat;

void main()
{
	gl_Position = vec4(mvpMat * vec4(vertex.x * 2.0, vertex.y * 2.0, vertex.z * 2.0, 1.0));
}
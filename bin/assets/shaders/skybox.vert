#version 430 core

in vec4 vertex;

uniform dmat4 mvpMat;

out vec3 textureDir; 

void main()
{
	gl_Position = vec4(mvpMat * vertex);
	
	textureDir = vec3(vertex.x, -vertex.y, -vertex.z);
}
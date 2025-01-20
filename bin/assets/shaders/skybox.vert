#version 430 core

in vec4 vertex;

// No necesitamos la matriz de modelado del cubemap
uniform dmat4 projViewMat;
uniform vec3 viewPos;

out vec3 textureDir; 

void main()
{
	gl_Position = vec4(projViewMat * vec4(viewPos + vertex.xyz, 1.0));
	
	textureDir = vec3(vertex.x, -vertex.y, -vertex.z);
}
#version 430 core

in vec4 vertex;
layout(location = 2) in vec2 uv0;

uniform dmat4 mvpMat;

out vec2 TexCoords; 

void main()
{
	gl_Position = vec4(mvpMat * vec4(vertex.x * 2.0, vertex.y * 2.0, vertex.z * 2.0, 1.0));
	TexCoords = uv0;
}
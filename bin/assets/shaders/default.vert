#version 430 core

in vec4 vertex;
layout(location = 2) in vec2 uv0;

uniform dmat4 mvpMat;

out vec2 TexCoords; 

void main()
{
	gl_Position = vec4(mvpMat * vertex);
	TexCoords = uv0;
}
#version 330 core
in vec4 vertex;
uniform mat4 modelMat;
void main()
{
gl_Position = vertex;
}
#version 330 core

in vec4 gl_FragCoord;
out vec4 FragColor;

void main()
{
	FragColor = vec4(0.3f, 0.8f, 0.9f, 1.0 - gl_FragCoord.z);
}
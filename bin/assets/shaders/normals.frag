#version 330 core

in vec4 gl_FragCoord;

out vec4 FragColor;

void main()
{
	// Le damos un color naranja a la línea
	vec3 color = vec3(0.8, 0.4, 0.0);
	
	FragColor = vec4(color, 1.0f);
}
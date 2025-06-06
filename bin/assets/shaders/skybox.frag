#version 330 core

// Variables que nos llegan desde el shader anterior
in vec3 textureDir;

// Se coge automáticamente
uniform samplerCube cubeTex;
uniform vec3 color = {1, 1, 1};

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

void main()
{
	// Bastante simple
	FragColor = vec4(texture(cubeTex, textureDir).rgb * color, 1.0);
}
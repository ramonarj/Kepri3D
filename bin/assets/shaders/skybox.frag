#version 330 core

// Variables que nos llegan desde el shader anterior
in vec3 textureDir;

// Se coge automáticamente
uniform samplerCube cubeTex;

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

void main()
{
	// Bastante simple
	FragColor = texture(cubeTex, textureDir);
}
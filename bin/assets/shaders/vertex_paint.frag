#version 330 core

#include material.glsl

// Entrada
in DATA
{	
	vec2 TexCoords;
	vec4 color;
} data_in;

// Uniforms
uniform sampler2D textura;
uniform vec2 tiling = {1.0, 1.0};
uniform Material material;

// Salida
out vec4 FragColor;

// Forma "sencilla" de ver si nos han pasado una textura desde el material o no
bool usingTexture(int tex) {
	return ((material.mapsMask & tex) == tex);
}	

void main()
{
	// Mezclamos el color del vértice con la textura
	if(usingTexture(DIFFUSE_MAP))
		FragColor = texture(textura, data_in.TexCoords * tiling) * data_in.color;
	// Solo el color del vértice
	else
		FragColor = data_in.color;
}
#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec4 colores;
} data_in;

uniform sampler2D textura;

uniform bool text = false;

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

void main()
{
	vec4 color;
	// Texto
	if(text)
		color = data_in.colores;
	// Imágenes, botones, etc
	else
		color = texture(textura, data_in.TexCoords);

	FragColor = color;
}
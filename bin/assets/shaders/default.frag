#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
} data_in;

// Se coge automáticamente
uniform sampler2D textura;
uniform vec2 tiling = {1.0, 1.0};

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

void main()
{
	//Cogemos el color de la textura correspondiente al fragmento actual
	vec4 color = texture(textura, data_in.TexCoords * tiling);

	FragColor = color;
}
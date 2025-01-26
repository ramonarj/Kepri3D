#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec4 colores;
} data_in;

// Se coge automáticamente
uniform sampler2D textura;

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

void main()
{
	//Cogemos o bien el color o bien la textura dada
	vec4 texColor = texture(textura, data_in.TexCoords);
	FragColor = texColor;
		
	//FragColor = vec4(data_in.colores.rgb, 0.2);
}
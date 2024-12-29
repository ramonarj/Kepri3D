#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
} data_in;

// 
uniform sampler2D textura;
uniform float tiempo;

//
out vec4 FragColor;

void main()
{
	// Variamos la coordenada X de la textura en función del tiempo que nos pasan
	vec3 color = vec3(texture(textura, vec2(data_in.TexCoords.x - tiempo, data_in.TexCoords.y)));

	FragColor = vec4(color, 1.0f);
}
#version 330 core

in vec2 TexCoords;

// Se coge automáticamente
uniform sampler2D textura;

out vec4 FragColor;

void main()
{
	//Cogemos el color de la textura correspondiente al fragmento actual
	vec3 color = vec3(texture(textura, TexCoords));

	FragColor = vec4(color, 1.0f);
}
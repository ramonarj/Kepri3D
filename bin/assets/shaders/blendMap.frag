#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
} data_in;

// - - Texturas - - //
uniform sampler2D textura;

// - - Material - - //
uniform sampler2D map1;
uniform sampler2D map2;
uniform sampler2D map3;
uniform sampler2D map4;

uniform vec2 tiling = {1.0, 1.0};

out vec4 FragColor;

void main()
{
	vec4 colorBlend = texture(textura, data_in.TexCoords);

	// Fondo (canal negro)
	vec4 c1 = texture(map1, data_in.TexCoords * tiling) * (1 - (colorBlend.r + colorBlend.g + colorBlend.b));
	// Las otras 3 texturas (canales R, G, B)
	vec4 c2 = colorBlend.r * texture(map2, data_in.TexCoords * tiling);
	vec4 c3 = colorBlend.g * texture(map3, data_in.TexCoords * tiling);
	vec4 c4 = colorBlend.b * texture(map4, data_in.TexCoords * tiling);

	FragColor = c1 + c2 + c3 + c4;
}
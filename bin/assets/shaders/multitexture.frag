#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
} data_in;

// Se coge automáticamente
uniform sampler2D texture1;
// Esta ya no (llamar a glActiveTexture)
uniform sampler2D texture2;

uniform float mix = 0.5;

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

void main()
{
	//FragColor = vec4(color, 1.0f);

	// Hacemos un mix entre las 2 texturas (simple)
	//FragColor = mix(texture(texture1, data_in.TexCoords), texture(texture2, data_in.TexCoords), 0.5);
	
	// - - Algunos efectos más interesantes - - //
	
	// a) La textura cambia al acercarse/alejarse
	/*
	if(gl_FragCoord.z < 0.8)
		FragColor = texture(texture1, data_in.TexCoords);
	else
		FragColor = texture(texture2, data_in.TexCoords);
	*/
		
	// b) En una mitad (horiz/vertical) de la pantalla se ve una, y en la otra, otra
	// (Interesante para juegos multijugador de pantalla partida) -> Cada jugador ve una textura distinta
	/*
	if(gl_FragCoord.x < 400)
		FragColor = texture(texture1, data_in.TexCoords);
	else
		FragColor = texture(texture2, data_in.TexCoords);
	*/
	
	// c) Se va alternando entre una textura y la otra, usando un seno
	// Para eso está el uniform 'mix'
	FragColor = mix(texture(texture1, data_in.TexCoords), texture(texture2, data_in.TexCoords), mix);
	
	// d) Mezcla usando un color específico de la segunda textura (negro en este caso) como transparencia
	/*
	vec4 color1 = texture(texture1, data_in.TexCoords);
	vec4 color2 = texture(texture2, data_in.TexCoords);
	if(length(vec3(color2)) == 0) //negro
		FragColor = color1;
	else
		FragColor = color2;
	*/
	
	// e) Blend map; pintar un terreno usando distintas texturas (tierra, pasto, piedras, flores...)
	// y una mapa pintado con colores primarios que indique en qué posición se debe usar cuál (TODO)
}
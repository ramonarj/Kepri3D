#version 330 core

// Estructuras necesarias
struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float brillo;
};

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
} data_in;

// Textura normal y mapa de desplazamiento
uniform sampler2D textura;
uniform sampler2D dispMap;

// Velocidad a la que desplazamos cada textura
uniform vec2 velTex;
uniform vec2 velDisp;

// CLK (milisegundos)
uniform float tiempo;
uniform Material material;

// Entre 0 y 1 (con 0.1 queda bien)
const float maxDistorsion = 0.1;

out vec4 FragColor;

void main()
{
	// Valor del pixel que queremos en el mapa de desplazamiento
	float claridad = texture(dispMap, (data_in.TexCoords - tiempo * velDisp)).r;
	
	// Variación de las coordenadas de la textura ppal en función de este pixel (+blanco = abajo a la derecha, +negro = arriba a la izquierda)
	vec2 desp = vec2(-0.5 + claridad, 0.5 - claridad);
	desp *= maxDistorsion;
		
	// Esto da un efecto muy interesante, de manchas de pintura (y el resto transparente)
	//if(length(claridad) < 0.5) {
	//	discard; return;
	//}

	// Aplicamos
	// a) Movimiento en la textura (dado por velTex)
	// b) Desplazamiento extra (dado por dispMap)
	vec4 color = texture(textura, data_in.TexCoords - desp - tiempo * velTex);

	FragColor = color * vec4(material.diffuse, 1.0);
}
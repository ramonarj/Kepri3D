#version 330 core

// Estructuras necesarias
struct Material
{
	vec3 ambient;
	vec4 diffuse;
	vec3 specular;
	float brillo;
};

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
	vec3 fragPos;
} data_in;

const int MAX_LIGHTS = 5;
layout (std140) uniform Lights
{
	vec3 camPos;
	//bool blinn; // No hace falta ahora mismo
	//Light luces[MAX_LIGHTS];
};

// - - Texturas - - //
uniform sampler2D textura;
uniform sampler2D dispMap;
uniform samplerCube skybox;

// - - Material - - //
uniform Material material;
uniform bool fresnel; 
const float liso = 0.8; // [0 - 1] cómo de lisa es la superficie (más lisa -> más efecto fresnel)

// Velocidad a la que desplazamos cada textura
uniform vec2 velTex;
uniform vec2 velDisp;
const float maxDistorsion = 0.1; // Entre 0 y 1 (con 0.1 queda bien)
 
// CLK (milisegundos)
uniform float tiempo;

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
	if(!fresnel) { FragColor = color * material.diffuse; return; }
	
	// - - - - - Efecto fresnel - - - - - //
	vec3 normal = normalize(data_in.normals);	
	vec3 viewDir = normalize(camPos - data_in.fragPos);
	float reflejo = 1 - dot(normal, viewDir);
	reflejo = pow(reflejo, 2); // atenuarlo
	
	// Rayo incidente y rayo reflejado
	vec3 I = normalize(data_in.fragPos - camPos);
	vec3 R = reflect(I, normal);
	R = vec3(R.x, -R.y, -R.z);

	// Mezclar el color de la textura con el reflejo dependiendo del ángulo de visión
	FragColor = mix(color * material.diffuse, textureLod(skybox, R, 3), reflejo);
}
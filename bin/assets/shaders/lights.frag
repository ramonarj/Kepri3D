#version 330 core

// Materiales, luces y shadowmaps
#include structs.glsl

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
	vec3 fragPos;
	mat3 TBN;
	// Para sombras
	vec4 FragPosLightSpace;
} data_in;

// - - Luces - - //
// Debe coincidir con Scene::MAX_LUCES
const int MAX_LIGHTS = 5;
// Uniform block
layout (std140) uniform Lights
{
	vec3 camPos;
	bool blinn; // 0 = Phong, 1 = Bling-Phong
	Light luces[MAX_LIGHTS];
};
// Shadowmaps
uniform Shadowmap shadowMaps[MAX_LIGHTS];
const int filter_size = 3;
const int filter_size_point = 2;

// - - Material - - //
uniform Material material;

uniform sampler2D textura;
uniform sampler2D normalMap;

uniform bool use_diff_map;
uniform bool use_spec_map;
uniform bool use_normal_map;
uniform bool receive_shadows;

// Variables globales
const float PI = 3.141593;
vec4 diffColor;
vec3 specColor;
vec3 ambColor;

// Funciones de iluminación
#include lighting.glsl
// Funciones de sombra
#include shadows.glsl

out vec4 FragColor;

void main()
{
	// 1) Calcular variables comunes a todas las luces
	// Color difuso del material en el fragmento actual
	if(use_diff_map) { diffColor = texture(textura, data_in.TexCoords); }
	else {diffColor = material.diffuse; }
	
	// Usamos el valor del specular map / no
	if(use_spec_map) { specColor = vec3(texture(material.specular_map, data_in.TexCoords)); }
	else {specColor = material.specular; }
	
	// Color ambient
	ambColor = material.ambient * diffColor.rgb;
	
	// Vector que va del fragmento a la cámara. NOTA: todos los vectores salen del fragmento (N, V, L, R...)
	vec3 viewDir = normalize(camPos - data_in.fragPos);
	
	vec3 normal;
	// Usamos el valor del normal map y no del vértice
	if(use_normal_map)
	{
		// 1) Cogemos el valor RGB correspondiente al fragmento actual
		normal = texture(normalMap, data_in.TexCoords).rgb;
		// 2) Pasamos del valor RGB a un vector en Tangent Space
		normal = normalize(normal * 2.0 - 1.0);
		// 3) Pasamos de espacio tangente a espacio global, con la matriz TBN
		normal = normalize(data_in.TBN * normal);
	}
	// La interpolación de normales por el FS genera un acortamiento en éstas, provocando que el triángulo 
	// sea más brillante en los bordes. Por eso, hay que volver a normalizarla aquí.
	else
		normal = normalize(data_in.normals);

	// 2) Iteramos todas las luces y vamos sumando el color que aportan al fragmento.
	vec3 colorTotal = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		// Está apagada
		if(!luces[i].on)
			continue;
		vec3 color = vec3(0,0,0);
		// 0 = Direccionales, 1 = Puntuales, 2 = Focos
		if(luces[i].type == 0)
		{
			color = CalcDirLight(luces[i], normal, viewDir);
			// La entidad recibe sombras del shadowmap
			if(receive_shadows)
			{
				float sombra = 1.0 - ShadowCalculation(shadowMaps[i], luces[i].dir, data_in.FragPosLightSpace);
				color *= min(1.0, sombra + 0.25); // si quitamos el ambient, la sombra es totalmente negra
			}
		}
		else if(luces[i].type == 1)
		{
			color = CalcPointLight(luces[i], normal, viewDir);
			if(receive_shadows)
			{
				float sombra = 1.0 - PointShadowCalculation(shadowMaps[i], luces[i].dir, data_in.fragPos);
				color *= min(1.0, sombra + 0.25);
			}
		}
		else if(luces[i].type == 2)
			color = CalcSpotlight(luces[i], normal, viewDir);
		
		colorTotal += color;
	}

	// Asignar el color al fragmento, incluyendo la transparencia
	FragColor = vec4(colorTotal, diffColor.a);
}
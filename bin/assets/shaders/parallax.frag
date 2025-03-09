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

uniform sampler2D dispMap;

// Variables globales
const float PI = 3.141593;
vec4 diffColor;
vec3 specColor;
vec3 ambColor;

// Funciones de iluminación
#include lighting.glsl
// Funciones de sombra
#include shadows.glsl

// - -  Parallax Mapping - - //
uniform int parallaxType = 1; // 0 = PM, 1 = Steep PM, 2 = Occlusion PM
// Entre 0.01 - 0.1 para que se vea bien
uniform float height_scale = 0.1f;

out vec4 FragColor;

vec2 ParallaxMapping(sampler2D dispMap, vec2 texCoords, vec3 viewDirTanSpace);

void main()
{
	vec2 texCoords = data_in.TexCoords;
	// 1) Calcular variables comunes a todas las luces
	// Vector que va del fragmento a la cámara. NOTA: todos los vectores salen del fragmento (N, V, L, R...)
	vec3 viewDir = normalize(camPos - data_in.fragPos);
	
	// Coordenadas de textura desplazadas por el Parallax Map
	mat3 inverseTBN = inverse(data_in.TBN);
	vec3 viewDirTanSpace   = normalize(inverseTBN * camPos - inverseTBN * data_in.fragPos);
    texCoords = ParallaxMapping(dispMap, texCoords, viewDirTanSpace);
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) { discard; } // quitar fragmentos que se salgan de la textura

	// Color difuso del material en el fragmento actual
	if(use_diff_map) { diffColor = texture(textura, texCoords); }
	else {diffColor = material.diffuse; }
	
	// Usamos el valor del specular map / no
	if(use_spec_map) { specColor = vec3(texture(material.specular_map, texCoords)); }
	else {specColor = material.specular; }
	
	// Color ambient
	ambColor = material.ambient * diffColor.rgb;
	
	vec3 normal;
	// Usamos el valor del normal map y no del vértice
	if(use_normal_map)
	{
		// 1) Cogemos el valor RGB correspondiente al fragmento actual
		normal = texture(normalMap, texCoords).rgb;
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

vec2 ParallaxMapping(sampler2D dispMap, vec2 texCoords, vec3 viewDirTanSpace)
{ 
	float height = 1 - texture(dispMap, texCoords).r; 
	// Normal
	if(parallaxType == 0)
	{  
		vec2 p = viewDirTanSpace.xy / viewDirTanSpace.z * (height * height_scale);
		return texCoords - p;   
	}
	// Steep
	else if(parallaxType == 1)
	{
		// number of depth layers
		const float minLayers = 8.0;
		const float maxLayers = 32.0;
		float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDirTanSpace), 0.0));
		//const float numLayers = 10;
		
		// calculate the size of each layer
		float layerDepth = 1.0 / numLayers;
		// depth of current layer
		float currentLayerDepth = 0.0;
		// the amount to shift the texture coordinates per layer (from vector P)
		vec2 P = viewDirTanSpace.xy * height_scale; 
		vec2 deltaTexCoords = P / numLayers; 
		
		// get initial values
		vec2  currentTexCoords     = texCoords;
		float currentDepthMapValue = height;
		  
		while(currentLayerDepth < currentDepthMapValue)
		{
			// shift texture coordinates along direction of P
			currentTexCoords -= deltaTexCoords;
			// get depthmap value at current texture coordinates
			currentDepthMapValue = 1 - texture(dispMap, currentTexCoords).r;  
			// get depth of next layer
			currentLayerDepth += layerDepth;  
		}

		return currentTexCoords;
	}
	// P.O.M.
	else if(parallaxType == 2)
	{
		return vec2(0,0);
	}
	return vec2(0,0);
} 
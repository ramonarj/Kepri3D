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
uniform sampler2D dispMap;

uniform bool receive_shadows;

// - - Niebla - - //
uniform bool fog = true;
uniform vec3 fog_color = { 0.5, 0.9, 1.0};
const float fog_start = 150.0f;
const float fog_end = 15000.0f;

// Variables globales
const float PI = 3.141593;
vec4 diffColor;
vec3 specColor;
vec3 ambColor;

// Funciones de iluminación
#include lighting.glsl
// Funciones de sombra
#include shadows.glsl
// Parallax mapping
#include parallax.glsl

out vec4 FragColor;

// Forma "sencilla" de ver si nos han pasado una textura desde el material o no
bool usingTexture(int tex)
{
	return ((material.mapsMask & tex) == tex);
}	

void main()
{
	// 1) Calcular variables comunes a todas las luces
	// Vector que va del fragmento a la cámara. NOTA: todos los vectores salen del fragmento (N, V, L, R...)
	vec3 viewDir = normalize(camPos - data_in.fragPos);
	
	// Usar un mapa de alturas/parallax para desplazar las coords. de textura
	vec2 texCoords;
	if(usingTexture(DISPLACEMENT_MAP))
	{
		// Hay que pasar de espacio global a espacio tangente
		mat3 inverseTBN = inverse(data_in.TBN);
		vec3 viewDirTanSpace = normalize(inverseTBN * camPos - inverseTBN * data_in.fragPos);
		texCoords = ParallaxMapping(dispMap, data_in.TexCoords, viewDirTanSpace, parallaxType);
		if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) { discard; } // quitar fragmentos que se salgan de la textura
	}
	else { texCoords = data_in.TexCoords; }
	
	// Color difuso del material en el fragmento actual
	if(usingTexture(DIFFUSE_MAP)) { diffColor = texture(textura, texCoords); }
	else {diffColor = material.diffuse; }
	
	// Usamos el valor del specular map / no
	if(usingTexture(SPECULAR_MAP)) { specColor = vec3(texture(material.specular_map, texCoords)); }
	else {specColor = material.specular; }
	
	// Color ambient
	ambColor = material.ambient * diffColor.rgb;
	
	vec3 normal;
	// Usamos el valor del normal map y no del vértice
	if(usingTexture(NORMAL_MAP))
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
	
	// Emisión (con/sin mapa)
	if(usingTexture(EMISSION_MAP)){ colorTotal = max(colorTotal, texture(material.emission_map, texCoords).rgb); }
	else { colorTotal = max(colorTotal, material.emission); }
	
	// Niebla lineal por distancia
	if(fog)
	{
		float fragDistance = length(camPos - data_in.fragPos);
		float fogAmount = clamp((fragDistance - fog_start) / (fog_end - fog_start), 0, 1);
		colorTotal = mix(colorTotal, fog_color, fogAmount);
	}

	// Asignar el color al fragmento, incluyendo la transparencia
	FragColor = vec4(colorTotal, diffColor.a);
}
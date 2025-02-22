#version 330 core

// Material con sus 3 componentes
struct Material
{
	vec3 ambient;
	vec4 diffuse;
	vec3 specular;
	sampler2D specular_map;
	float brillo;
};

// Struct multifunción para los 3 tipos de luces
struct Light 
{
	int type; 
	
	vec3 dir;
	// Componentes de la luz
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	// Para luces no direccionales
	float constant;
	float linear;
	float quadratic;

	// Para focos
	vec3 spotDir;
	float spotCutoff;
	float spotExp;
	
	bool on;
};

// Shadow maps
struct Shadowmap
{
	// Luces direccionales
	sampler2D directionalMap;
	// Luces puntuales
	samplerCube pointMap;
	float far_plane;
	// PCF
	bool soft_shadows;
};

// - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - //

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

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

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

// Prototipos para las funciones
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcSpotlight(Light light, vec3 normal, vec3 viewDir);
float SpecFactor(vec3 lightDir, vec3 viewDir, vec3 normal, float brillo, bool useBlinn);
float ShadowCalculation(Shadowmap shMap, vec3 lightPos, vec4 fragPosLightSpace);
float PointShadowCalculation(Shadowmap shMap, vec3 lightPos, vec3 fragPos);
float LinearizeDepth(float depth, float near_plane, float far_plane);

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

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	// Dirección normalizada de la luz
	vec3 lightDir = normalize(light.dir);
	
	// - - Componentes ambient  y diffuse -- //
	vec3 ambient = light.ambient * ambColor; // material.ambient ¿?
	// Producto escalar de la normal con la dirección de la luz
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * diffColor.rgb;

	// - - Componente especular - - //
	float spec = SpecFactor(lightDir, viewDir, normal, material.brillo, blinn);
    vec3 specular = light.specular * spec * specColor;
	
	// Devolver la suma de las 3 componentes
	return (ambient + diffuse + specular);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir)
{
	// Dirección normalizada de la luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Componente difusa - - //
    float diff = max(dot(normal, lightDir), 0.0);
	
    // - - Componente especular - - //
	float spec = SpecFactor(lightDir, viewDir, normal, material.brillo, blinn);
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados
	vec3 ambient = light.ambient * ambColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor.rgb;
    vec3 specular = light.specular * spec * specColor;

	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcSpotlight(Light light, vec3 normal, vec3 viewDir)
{
	// Dirección normalizada de cada luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Componente difusa - - //
	// Ángulo que forma la dirección del foco con el fragmento actual
	float diff = 1.0f;
	float cutoffRad = light.spotCutoff * PI / 180.0;
	float angle = acos(dot(-lightDir, light.spotDir));
	// Cortar en el límite
	if(angle > cutoffRad)
		diff = 0;
	// Aplicar la difuminación del foco
	else
	{
		//diff = pow(cos(angle), light.spotExp);
		diff = 1.0 - angle / cutoffRad;
	}
		
	// Tener en cuenta también las normales	
	diff *= max(dot(normal, -light.spotDir), 0.0);
	
    // - - Componente especular - - //
	float spec = SpecFactor(lightDir, viewDir, normal, material.brillo, blinn);
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados
	vec3 ambient = light.ambient * ambColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor.rgb;
	vec3 specular = light.specular * spec * diff * specColor;
	
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}

float SpecFactor(vec3 lightDir, vec3 viewDir, vec3 normal, float brillo, bool useBlinn)
{
	// Evitar reflejos que vengan desde detrás de la superficie
	if(dot(normal, lightDir) <= 0)
		return 0;
	// Blinn-Phong
	float spec = 0.0;
	if(useBlinn)
	{
		vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), brillo * 3.0); // Para conseguir los mismos efectos que con Blinn, el brillo debe ser entre 2 y 4 veces mayor
	}
	// Phong
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), brillo);
	}
	return spec;
}

/* Devuelve 0 si el fragmento no está en sombra, 1 si lo está */
float ShadowCalculation(Shadowmap shMap, vec3 lightPos, vec4 fragPosLightSpace)
{	
    // División de perspectiva
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
    // Pasar de rango [-1, 1] a [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
	
	// Si el fragmento está fuera de los límites del shadow map, no aplicamos sombra
	if(projCoords.z > 1.0)
        return 0.0;
	
	// Profundidad de este fragmento visto desde la luz
	float currentDepth = projCoords.z;
	
	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(data_in.normals);
    vec3 lightDir = normalize(lightPos - data_in.fragPos);
	float maxBias = 0.02; float minBias = 0.008;
	float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
 
	// Usar varios samples del shadowMap (sombras "suaves")
	float shadow = 0.0;
	if(shMap.soft_shadows)
	{
		vec2 texelSize = 1.0 / textureSize(shMap.directionalMap, 0);
		int halfFilter = filter_size / 2;
		for(int x = -halfFilter; x < -halfFilter + filter_size; ++x)
		{
			for(int y = -halfFilter; y < -halfFilter + filter_size; ++y)
			{
				float pcfDepth = texture(shMap.directionalMap, projCoords.xy + vec2(x, y) * texelSize).r; 
				shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
			}    
		}	
		shadow /= (filter_size * filter_size);
	}
	// Solo 1 sample (sombras "duras")
	else
	{
		// Profundidad mínima desde la perspectiva de la luz [0, 1]
		float closestDepth = texture(shMap.directionalMap, projCoords.xy).r; 
		// Comprobar si el fragmento está en sombra o no
		shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0; 
	}

    return shadow;
} 

// Para luces puntuales
float PointShadowCalculation(Shadowmap shMap, vec3 lightPos, vec3 fragPos)
{	
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
	
	// Si el fragmento está fuera de los límites del shadow map, no aplicamos sombra
	if(length(fragToLight) > shMap.far_plane)
		return 0.0;
	
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
	
	// calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(data_in.normals);
    vec3 lightDir = normalize(lightPos - data_in.fragPos);
	float maxBias = 0.5; float minBias = 0.02;
	float bias = max(maxBias * (1.0 - dot(normal, lightDir)), minBias);
	
	//FragColor = vec4(vec3(closestDepth / far_plane), 1.0); //depuracion
	
	float shadow = 0.0;
	if(shMap.soft_shadows)
	{
		float offset  = 0.1;
		for(float x = -offset; x < offset; x += offset / (filter_size_point * 0.5))
		{
			for(float y = -offset; y < offset; y += offset / (filter_size_point * 0.5))
			{
				for(float z = -offset; z < offset; z += offset / (filter_size_point * 0.5))
				{
					float pcfDepth = texture(shMap.pointMap, fragToLight + vec3(x, y, z)).r; 
					pcfDepth *= shMap.far_plane;   // undo mapping [0;1]
					shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
				}
			}
		}
		shadow /= (filter_size_point * filter_size_point * filter_size_point);
	}
	// Solo 1 sample (sombras "duras")
	else
	{
		// use the light to fragment vector to sample from the depth map    
		float closestDepth = texture(shMap.pointMap, fragToLight).r;
		// it is currently in linear range between [0,1]. Re-transform back to original value
		closestDepth *= shMap.far_plane;
		
		shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
	}

    return shadow;
} 

// required when using a perspective projection matrix
float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}
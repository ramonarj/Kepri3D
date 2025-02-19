#version 330 core

// Material con sus 3 componentes
struct Material
{
	vec3 ambient;
	vec3 diffuse;
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

// - - Material - - //
uniform Material material;

uniform sampler2D textura;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;

uniform bool use_diff_map;
uniform bool use_spec_map;
uniform bool use_normal_map;
uniform bool receive_shadows;

// Variables globales
const float PI = 3.141593;
vec3 diffColor;
vec3 specColor;
vec3 ambColor;

// Prototipos para las funciones
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcSpotlight(Light light, vec3 normal, vec3 viewDir);
float SpecFactor(vec3 lightDir, vec3 viewDir, vec3 normal, float brillo, bool useBlinn);
float ShadowCalculation(sampler2D shMap, vec4 fragPosLightSpace);
float LinearizeDepth(float depth, float near_plane, float far_plane);

void main()
{
	// 1) Calcular variables comunes a todas las luces
	// Color difuso del material en el fragmento actual
	if(use_diff_map) { diffColor = vec3(texture(textura, data_in.TexCoords)); }
	else {diffColor = material.diffuse; }
	
	// Usamos el valor del specular map / no
	if(use_spec_map) { specColor = vec3(texture(material.specular_map, data_in.TexCoords)); }
	else {specColor = material.specular; }
	
	// Color ambient
	ambColor = material.ambient * diffColor;
	
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

	// 2) Iteramos todas las luces y vamos sumando el color que aportan al fragmento
	vec3 color = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		// Está apagada
		if(!luces[i].on)
			continue;
		// 0 = Direccionales, 1 = Puntuales, 2 = Focos
		if(luces[i].type == 0)
			color += CalcDirLight(luces[i], normal, viewDir);
		else if(luces[i].type == 1)
			color += CalcPointLight(luces[i], normal, viewDir);
		else if(luces[i].type == 2)
			color += CalcSpotlight(luces[i], normal, viewDir);
	}

	// De momento no hay transparencias
	//FragColor = vec4(color, 1.0);
	//float depthValue = texture(shadowMap, data_in.TexCoords).r;
    //FragColor = vec4(vec3(LinearizeDepth(depthValue, 1.0, 200.0) / 200.0), 1.0); // perspective
	
	// La entidad recibe sombras del shadowmap
	if(receive_shadows)
	{
		float sombra = 1.0 - ShadowCalculation(shadowMap, data_in.FragPosLightSpace);
		FragColor = vec4(color * sombra + ambColor, 1.0); // si quitamos el ambient, la sombra es totalmente negra
	}
	// Sombreado normal
	else
		FragColor = vec4(color, 1.0);
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
	vec3 diffuse = light.diffuse * diff * diffColor;

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
    vec3 diffuse  = light.diffuse  * diff * diffColor;
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
    vec3 diffuse  = light.diffuse  * diff * diffColor;
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

float ShadowCalculation(sampler2D shMap, vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	// para paliar el acné
	float bias = 0.001;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
} 

// required when using a perspective projection matrix
float LinearizeDepth(float depth, float near_plane, float far_plane)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}
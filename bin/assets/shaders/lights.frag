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
};

// - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - //

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
	vec3 fragPos;
} data_in;

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

// Se coge automáticamente
uniform sampler2D textura;

// - - Luces - - //
// Tipo de iluminación | 0 = Phong, 1 = Bling-Phong
uniform bool blinn;
// Permitiremos un máximo de 5 luces por el momento
const int MAX_LIGHTS = 5;
uniform Light luces[MAX_LIGHTS];

// - - Material - - //
uniform Material material;
uniform vec3 viewPos;

uniform bool use_spec_map;

// Prototipos para las funciones
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcSpotlight(Light light, vec3 normal, vec3 viewDir);

float SpecFactor(vec3 lightDir, vec3 viewDir, vec3 normal, float brillo, bool useBlinn);

const float PI = 3.141593;

void main()
{
	// Cogemos el color de la textura correspondiente al fragmento actual
	vec4 texColor = texture(textura, data_in.TexCoords);
	
	// Variables comunes a todas las luces. NOTA: todos los vectores salen del fragmento (N, V, L, R...)
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
	// La interpolación de normales por el FS genera un acortamiento en éstas, provocando que el triángulo 
	// sea más brillante en los bordes. Por eso, hay que volver a normalizarla aquí.
	vec3 normal = normalize(data_in.normals);

	// Iteramos todas las luces para conocer la iluminación del fragmento
	vec3 luzTotal = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		// a) Luces direccionales
		if(luces[i].type == 0)
			luzTotal += CalcDirLight(luces[i], normal, viewDir);
		// b) Luces puntuales
		else if(luces[i].type == 1)
			luzTotal += CalcPointLight(luces[i], normal, viewDir);
		// c) Focos
		else if(luces[i].type == 2)
			luzTotal += CalcSpotlight(luces[i], normal, viewDir);
	}
	
	// Aplicamos la iluminación al color de la textura (* = MODULATE, + = ADD, ...)
	vec3 result = luzTotal * vec3(texColor);

	FragColor = vec4(result, texColor.a);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	// Obtener la dirección normalizada de cada luz
	vec3 lightDir = normalize(light.dir);
	
	// - - Calcular la componente ambient -- //
	vec3 ambient = material.ambient * light.ambient;

	// - - Calcular la componente difusa - - //
	// Producto escalar de la normal con la dirección de la luz
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = material.diffuse * diff * light.diffuse;

	// - - Calcular la componente especular - - //
	float spec = SpecFactor(lightDir, viewDir, normal, material.brillo, blinn);
    vec3 specular = light.specular * spec;
	
	// Usar una textura/no para la componente especular
	if(use_spec_map)
		specular *= vec3(texture(material.specular_map, data_in.TexCoords));
	else
		specular *= material.specular;
	
	// Devolver la suma de las 3 componentes
	return (ambient + diffuse + specular);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcPointLight(Light light, vec3 normal, vec3 viewDir)
{
	// Obtener la dirección normalizada de cada luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Calcular la componente difusa - - //
    float diff = max(dot(normal, lightDir), 0.0);
	
    // - - Calcular la componente especular - - //
	float spec = SpecFactor(lightDir, viewDir, normal, material.brillo, blinn);
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados
	vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
    vec3 specular = light.specular * spec;
	// Usar una textura/no para la componente especular
	if(use_spec_map)
		specular *= vec3(texture(material.specular_map, data_in.TexCoords));
	else
		specular *= material.specular;
	
	// Atenuación por la distancia
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcSpotlight(Light light, vec3 normal, vec3 viewDir)
{
	// Obtener la dirección normalizada de cada luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Calcular la componente difusa - - //
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
	
    // - - Calcular la componente especular - - //
	float spec = SpecFactor(lightDir, viewDir, normal, material.brillo, blinn);
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados
	vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff * material.diffuse;
	vec3 specular = light.specular * spec * diff;
	
	// Usar una textura/no para la componente especular
	if(use_spec_map)
		specular *= vec3(texture(material.specular_map, data_in.TexCoords));
	else
		specular *= material.specular;
		
	// Atenuación por la distancia
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
        spec = pow(max(dot(normal, halfwayDir), 0.0), brillo);
	}
	// Phong
	else
	{
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(viewDir, reflectDir), 0.0), brillo);
	}
	return spec;
}
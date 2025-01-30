#version 330 core

/* Prácticamente idéntico a "lights.frag" */

// Estructuras necesarias
struct Material
{
	float brillo; // = 10;
	vec3 ambient;
};

struct Light // (direccional)
{
	int type; 

	vec3 dir;
	
	// Componentes de la luz
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	// Para luces puntuales
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
// Mapa especular; reemplaza a 'material.specStrength'
uniform sampler2D specMap;

// - - Luces - - //
// Permitiremos un máximo de 5 luces por el momento
const int MAX_LIGHTS = 5;
uniform Light luces[MAX_LIGHTS];

// - - Material - - //
uniform Material material;
uniform vec3 viewPos;

// Prototipos para las funciones
vec3 CalcDirLight(Light light);
vec3 CalcPointLight(Light light);
vec3 CalcSpotlight(Light light);

void main()
{
	// Cogemos el color de la textura correspondiente al fragmento actual
	vec4 texColor = texture(textura, data_in.TexCoords);

	// Iteramos todas las luces para conocer la iluminación del fragmento
	vec3 luzTotal = vec3(0.0);
	for(int i = 0; i < MAX_LIGHTS; i++)
	{
		// a) Luces direccionales
		if(luces[i].type == 0)
		{
			luzTotal += CalcDirLight(luces[i]);
		}
		// b) Luces puntuales
		else if(luces[i].type == 1)
		{
			luzTotal += CalcPointLight(luces[i]);
		}
		// c) Focos
		else if(luces[i].type == 2)
		{
			luzTotal += CalcSpotlight(luces[i]);
		}
	}
	// Aplicamos las luces al color de la textura (* = MODULATE, + = ADD, ...)
	vec3 result = luzTotal * vec3(texColor);

	FragColor = vec4(result, 1.0);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcDirLight(Light light)
{
	// Obtener la dirección normalizada de cada luz
	vec3 lightDir = normalize(light.dir);

	// - - La componente ambiente - - //
	vec3 ambient = material.ambient * light.ambient;
	
	// - - Calcular la componente difusa - - //
	// Producto escalar de la normal con la dirección de la luz
	float diff = max(dot(data_in.normals, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;

	// - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
	// 'reflect' calcula el vector de reflexión
	vec3 reflectDir = reflect(-lightDir, data_in.normals);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
	// Ahora la textura/mapa especular es la que influye en el color del reflejo
	vec3 specMapColor = vec3(texture(specMap, data_in.TexCoords));
    vec3 specular = specMapColor * spec * light.specular;
	
	// Devolver la suma de las 3 componentes
	return (ambient + diffuse + specular);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcPointLight(Light light)
{
	// Obtener la dirección normalizada de cada luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Calcular la componente difusa - - //
    float diff = max(dot(data_in.normals, lightDir), 0.0);
	
    // - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, data_in.normals);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
	vec3 specMapColor = vec3(texture(specMap, data_in.TexCoords));
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados y devolver la suma
	vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff; // * material.diffuse;
    vec3 specular = light.specular * spec * specMapColor;
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}


/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcSpotlight(Light light)
{
	const float PI = 3.141593;
	// Obtener la dirección normalizada de cada luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Calcular la componente difusa - - //
    float diff = max(dot(data_in.normals, -light.spotDir), 0.0);

	// Ángulo que forma la dirección del foco con el fragmento actual
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
	
	
    // - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, data_in.normals);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
	vec3 specMapColor = vec3(texture(specMap, data_in.TexCoords));
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados y devolver la suma
	vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff; // * material.diffuse;
    vec3 specular = light.specular * spec * diff * specMapColor;
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}
#version 330 core

// Estructuras necesarias
struct Material
{
	// Por ahora nos da igual el diffuse del material
	vec3 ambient;
	vec3 specular;
	float brillo;
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

// GL_TEXTURE0
uniform sampler2D textura;
// GL_TEXTURE1
uniform sampler2D normalMap;
vec3 fragNormal;

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
	// Cogemos la normal del fragmento actual
	fragNormal = texture(normalMap, data_in.TexCoords).rgb;
	// Pasar del valor RGB a la normal
	fragNormal = normalize(fragNormal * 2.0 - 1.0);
	
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
	
	// Aplicamos la iluminación al color de la textura (* = MODULATE, + = ADD, ...)
	vec3 result = luzTotal * vec3(texColor);

	FragColor = vec4(result, texColor.a);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcDirLight(Light light)
{
	// Obtener la dirección normalizada de cada luz
	vec3 lightDir = normalize(light.dir);
	
	// - - Calcular la componente ambient -- //
	vec3 ambient = material.ambient * light.ambient;

	// - - Calcular la componente difusa - - //
	// Producto escalar de la normal con la dirección de la luz
	float diff = max(dot(fragNormal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;

	// - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
	// 'reflect' calcula el vector de reflexión
	vec3 reflectDir = reflect(-lightDir, fragNormal);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
    vec3 specular = material.specular * spec * light.specular;
	
	// Devolver la suma de las 3 componentes
	return (ambient + diffuse + specular);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcPointLight(Light light)
{
	// Obtener la dirección normalizada de cada luz
    vec3 lightDir = normalize(light.dir - data_in.fragPos);
	
    // - - Calcular la componente difusa - - //
    float diff = max(dot(fragNormal, lightDir), 0.0);
	
    // - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados
	vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff; // * material.diffuse;
    vec3 specular = light.specular * spec * material.specular;
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
	// Ángulo que forma la dirección del foco con el fragmento actual
	float diff = 1.0f;
	float cutoffRad = light.spotCutoff * PI / 180.0;
	float angle = acos(dot(-lightDir, light.spotDir));
	// Cortamos totalmente la luz
	if(angle > cutoffRad)
		diff = 0;
	// Aplicar la difuminación del foco
	else
	{
		//diff = pow(cos(angle), light.spotExp);
		diff = 1.0 - angle / cutoffRad;
	}
		
	// Tener también en cuenta las normales	(hay que hacerlo en este orden)
	diff *= max(dot(fragNormal, -light.spotDir), 0.0);
	
    // - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
    vec3 reflectDir = reflect(light.spotDir, fragNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
	
    // - - Atenuación por la distancia - - //
    float distance = length(light.dir - data_in.fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
				 
    // Combinar los resultados
	vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse  = light.diffuse  * diff; // * material.diffuse;
    vec3 specular = light.specular * spec * diff * material.specular;
	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (ambient + diffuse + specular);
}
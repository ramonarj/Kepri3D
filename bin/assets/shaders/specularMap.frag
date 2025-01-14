#version 330 core

/* Prácticamente idéntico a "lights.frag" */

// Estructuras necesarias
struct Material
{
	float brillo; // = 10;
};

struct Light // (direccional)
{
	int type; 

	vec3 dir;
	vec3 diffuse; 
	vec3 specular; 
	
	// Para luces puntuales
	float constant;
    float linear;
    float quadratic; 
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
/* Ambient */
vec3 globalAmbient = { 0.2, 0.2, 0.2 };

/* Diffuse */
uniform Light luzSol;

/* Specular */
uniform Material material;
uniform vec3 viewPos;

// Prototipos para las funciones
vec3 CalcDirLight(Light light);
vec3 CalcPointLight(Light light);

void main()
{
	// Cogemos el color de la textura correspondiente al fragmento actual
	vec4 texColor = texture(textura, data_in.TexCoords);

	// Por ahora solo usamos una luz para el cálculo
	vec3 luzTotal = vec3(0.0);
	// a) Luces direccionales
	if(luzSol.type == 0)
	{
		luzTotal += CalcDirLight(luzSol);
	}
	// b) Luces puntuales
	else
	{
		luzTotal += CalcPointLight(luzSol);
	}
	
	// Aplicamos las luces al color de la textura (* = MODULATE, + = ADD, ...)
	vec3 result = (globalAmbient + luzTotal) * vec3(texColor);

	FragColor = vec4(result, 1.0);
}

/* Calcula la cantidad de luz que recibe el fragmento de una luz direccional */
vec3 CalcDirLight(Light light)
{
	// Obtener la dirección normalizada de cada luz
	vec3 lightDir = normalize(light.dir);

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
	return (diffuse + specular);
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
				 
    // Combinar los resultados
    vec3 diffuse  = light.diffuse  * diff; // * material.diffuse;
    vec3 specular = light.specular * spec * specMapColor;
    diffuse  *= attenuation;
    specular *= attenuation;
	
    return (diffuse + specular);
}
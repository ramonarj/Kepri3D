// Cálculo de iluminación para un fragmento

/* Calcula la cantidad de componente especular que emite un fragmento */
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

/* Calcula la cantidad de luz que recibe el fragmento de una luz puntual */
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

/* Calcula la cantidad de luz que recibe el fragmento de una luz tipo foco */
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
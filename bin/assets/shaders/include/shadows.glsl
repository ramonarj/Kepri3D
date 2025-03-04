// Cálculo de sombras para un fragmento

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
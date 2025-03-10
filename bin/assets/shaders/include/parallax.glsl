// Tipo de paralaje
uniform int parallaxType = 1; // 0 = PM, 1 = Steep PM, 2 = Occlusion PM
// Entre 0.01 - 0.1 para que se vea bien
uniform float height_scale = 0.1f;


/* Obtiene las coordenadas de textura desplazadas por paralaje. "viewDir" debe estar en ESPACIO TANGENCIAL. */
vec2 ParallaxMapping(sampler2D dispMap, vec2 texCoords, vec3 viewDir, int type)
{ 
	float height = 1 - texture(dispMap, texCoords).r; 
	// Normal
	if(type == 0)
	{  
		vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
		return texCoords - p;   
	}
	// Steep
	else if(type >= 1)
	{
		// number of depth layers
		const float minLayers = 8.0;
		const float maxLayers = 32.0;
		float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));
		//const float numLayers = 10;
		
		// calculate the size of each layer
		float layerDepth = 1.0 / numLayers;
		// depth of current layer
		float currentLayerDepth = 0.0;
		// the amount to shift the texture coordinates per layer (from vector P)
		vec2 P = viewDir.xy * height_scale; 
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
		
		// P.O.M.
		if(type == 2)
		{
			// get texture coordinates before collision (reverse operations)
			vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

			// get depth after and before collision for linear interpolation
			float afterDepth  = currentDepthMapValue - currentLayerDepth;
			float beforeDepth = 1 - texture(dispMap, prevTexCoords).r - currentLayerDepth + layerDepth;
			 
			// interpolation of texture coordinates
			float weight = afterDepth / (afterDepth - beforeDepth);
			vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

			return finalTexCoords;
		}

		return currentTexCoords;
	}
	return vec2(0,0);
} 
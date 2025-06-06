#version 330 core

in DATA
{	
	vec2 TexCoords;
	vec3 normals;
	vec3 fragPos;
} data_in;

out vec4 FragColor;

uniform vec3 viewPos;

uniform sampler2D textura;
uniform sampler2D reflectionMap;
uniform samplerCube skybox;

#include material.glsl
#include light.glsl

uniform Material material;

// Forma "sencilla" de ver si nos han pasado una textura desde el material o no
bool usingTexture(int tex)
{
	return ((material.mapsMask & tex) == tex);
}

void main()
{   
	// Por defecto, refleja el 100%
	float mix = 1.0;
	if(usingTexture(REFLECTION_MAP))
	{
		// ¿Cuánto refleja la entidad? (0-1)
		vec4 color = texture(reflectionMap, data_in.TexCoords);
		if(color.a < 0.1) { discard; return; } //descartar fragmentos transparentes
		mix = color.r;
	}

	// Rayo incidente y rayo reflejado
	vec3 I = normalize(data_in.fragPos - viewPos);
	vec3 R = reflect(I, normalize(data_in.normals));
	
	// Correción por tema coordenadas
	R = vec3(R.x, -R.y, -R.z);
	
	// Samplear la textura en función de esa dirección, y mezclarla con la propia textura de la entidad en la medida 
	// que indique el "mapa de reflexión"
	/* Usamos el LOD (level-of-detail) para hacerla un poco borrosa; cuanto más grande el número, más pequeño el mipmap usado */
	FragColor = mix(texture(textura, data_in.TexCoords), textureLod(skybox, R, 3), mix);
}
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
// Material con sus 3 componentes
struct Material
{
	vec3 ambient;
	vec4 diffuse;
	vec3 specular;
	sampler2D specular_map;
	
	float brillo;
		
	vec3 emission;
	sampler2D emission_map;

	//
	int mapsMask;
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

// IDs para las texturas del material
#define DIFFUSE_MAP 1
#define SECONDARY_MAP 2
#define SPECULAR_MAP 4
#define NORMAL_MAP 8
#define DISPLACEMENT_MAP 16
#define REFLECTION_MAP 32
#define SKYBOX_MAP 64
#define EMISSION_MAP 128
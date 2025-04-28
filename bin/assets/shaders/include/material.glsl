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

// IDs para las texturas del material
#define DIFFUSE_MAP 1
#define SECONDARY_MAP 2
#define SPECULAR_MAP 4
#define NORMAL_MAP 8
#define DISPLACEMENT_MAP 16
#define REFLECTION_MAP 32
#define SKYBOX_MAP 64
#define EMISSION_MAP 128
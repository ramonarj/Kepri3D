#version 330 core

// Estructuras necesarias
struct Material
{
	vec3 specStrength; // = {0.5, 0.5, 0.5};
	float brillo; // = 10;
};

struct Light // (direccional)
{
	vec3 dir;
	vec3 color; // = {1, 1, 1};
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

void main()
{
	// Cogemos el color de la textura correspondiente al fragmento actual
	vec4 texColor = texture(textura, data_in.TexCoords);

	// Por ahora solo usamos una luz para el cálculo
	vec3 luzTotal = CalcDirLight(luzSol);
	
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
	vec3 diffuse = diff * light.color;

	// - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
	// 'reflect' calcula el vector de reflexión
	vec3 reflectDir = reflect(-lightDir, data_in.normals);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.brillo);
    vec3 specular = material.specStrength * spec * light.color;
	
	// Devolver la suma de las 3 componentes
	return (diffuse + specular);
}
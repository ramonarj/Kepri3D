#version 330 core

// Variables que nos llegan desde el shader anterior
in DATA
{	
	vec2 TexCoords;
	vec3 normals;
	vec3 fragPos;
} data_in;

// Se coge automáticamente
uniform sampler2D textura;

// Obligatorio darle un valor al fragmento actual
out vec4 FragColor;

// - - Luces - - //
/* Ambient */
vec3 ambient = { 0.2, 0.2, 0.2 };

/* Diffuse */
uniform vec3 lightDir;
uniform vec3 lightColor = {1, 1, 1}; // los uniforms pueden tener valor por defecto

/* Specular */
uniform vec3 viewPos;
// Atributos del material
uniform vec3 specStrength = {0.5, 0.5, 0.5};
uniform float brillo = 10;

void main()
{
	//Cogemos el color de la textura correspondiente al fragmento actual
	vec4 texColor = texture(textura, data_in.TexCoords);
	
	// Obtener la dirección normalizada de cada luz
	vec3 normLightDir = normalize(lightDir);

	// - - Calcular la componente difusa - - //
	// Producto escalar de la normal con la dirección de la luz
	float diff = max(dot(data_in.normals, normLightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// - - Calcular la componente especular - - //
	vec3 viewDir = normalize(viewPos - data_in.fragPos);
	// 'reflect' calcula el vector de reflexión
	vec3 reflectDir = reflect(-normLightDir, data_in.normals);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), brillo);
    vec3 specular = specStrength * spec * lightColor;  

	// - - Aplicamos las luces al color de la textura (* = MODULATE, + = ADD, ...) - - //
	vec3 result = (ambient + diffuse + specular) + vec3(texColor);

	FragColor = vec4(result, 1.0);
}
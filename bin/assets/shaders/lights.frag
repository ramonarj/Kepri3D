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

void main()
{
	//Cogemos el color de la textura correspondiente al fragmento actual
	vec4 texColor = texture(textura, data_in.TexCoords);

	// Obtener la dirección normalizada de cada luz
	vec3 normLightDir = normalize(lightDir);

	// Calcular la luz difusa con el producto escalar de la normal
	float diff = max(dot(data_in.normals, normLightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Aplicamos las luces al color de la textura (* = MODULATE)
	vec3 result = (ambient + diffuse) * vec3(texColor);

	FragColor = vec4(result, 1.0);
}
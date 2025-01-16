#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 1) in vec4 aColores;
layout(location = 2) in vec2 uv0;

layout(location = 10) in vec3 position;

// Variables uniform
uniform dmat4 view;
uniform dmat4 projection;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec4 colores;
} data_out;

void main()
{
	// Hacer el efecto 'billboard' (orientar la partícula mirando a cámara)
	// Ejemplo de rotación en el eje Y (90º)
	dmat4 rotacion = mat4(1.0);
	rotacion[0] = vec4(0, 0, -1, 0);
	rotacion[2] = vec4(1, 0, 0, 0);
	//gl_Position = vec4(projection * view * (vertex + vec4(position.xyz, 0.0)));

	/* Línea encontrada en StackOverflow (https://stackoverflow.com/questions/31119653/billboarding-vertices-in-the-vertex-shader/31121743#31121743) */
	//gl_Position = projectionMatrix * (modelViewMatrix * vec4(labelpos, 1) + vec4(position.xy, 0, 0));
	gl_Position = vec4(projection * (view * vec4(position, 1) + vec4(vertex.xy, 0, 0))); // view * position es como un 'modelView'

	
	data_out.TexCoords = uv0;
	data_out.colores = aColores;
}
#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 1) in vec4 aColores;
layout(location = 2) in vec2 uv0;

layout(location = 10) in vec3 position;

// Uniform block
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec4 colores;
} data_out;

void main()
{
	/* Línea encontrada en StackOverflow (https://stackoverflow.com/questions/31119653/billboarding-vertices-in-the-vertex-shader/31121743#31121743) */
	//gl_Position = projectionMatrix * (modelViewMatrix * vec4(labelpos, 1) + vec4(position.xy, 0, 0));
	gl_Position = vec4(projection * (view * vec4(position, 1) + vec4(vertex.xy, 0, 0))); // view * position es como un 'modelView'
	
	data_out.TexCoords = uv0;
	data_out.colores = aColores;
}
#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 3) in vec3 aNormals;

// Uniform block
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};
uniform dmat4 model;

// Para el siguiente shader
out DATA
{
	vec4 normalEnd;
} data_out;

// Se podría hacer de otra forma; pasándole la posición en 'world space' al GS y dejando
// que él calcule el inicio y final de los vértices con la matriz de vista+proyeccion
void main()
{
	// Inicio y fin del vector normal (ya en 'clip space' para el Geometry Shader)
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.normalEnd = vec4(projection * view * model * (vertex + vec4(aNormals * 0.25, 0.0)));
}
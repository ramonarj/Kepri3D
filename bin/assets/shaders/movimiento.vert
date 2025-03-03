#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;
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
	vec2 TexCoords; 
	vec3 normals;
	vec3 fragPos;
} data_out;

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.TexCoords = uv0;
	data_out.fragPos = vec3(model * vertex);
	
	// Aplicar la rotación del modelo a las normales
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	data_out.normals = normalMatrix * aNormals;
}
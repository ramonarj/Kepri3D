#version 430 core

// Variables que nos llegan de los vértices
in vec4 vertex;
layout(location = 2) in vec2 uv0;
layout(location = 3) in vec3 aNormals;
layout(location = 4) in vec3 aTangents;

// Variables uniform
uniform dmat4 model;
uniform dmat4 view;
uniform dmat4 projection;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
	vec3 fragPos;
	mat3 TBN;
} data_out;

void main()
{
	gl_Position = vec4(projection * view * model * vertex);
	
	data_out.TexCoords = uv0;
	// Posición del fragmento en el espacio global
	data_out.fragPos = vec3(model * vertex);
	// Aplicar la rotación del modelo a las normales (copiado de https://learnopengl.com/Lighting/Basic-Lighting)
	data_out.normals = mat3(transpose(inverse(model))) * aNormals;
	
	// Tangente, bitangente y normal
	mat3 normalMatrix = transpose(inverse(mat3(model))); // correct normal transform as learned in previous tutorials here
	vec3 N = normalize(normalMatrix * aNormals);
	vec3 T = normalize(normalMatrix * aTangents);
	vec3 B = normalize(cross(N, T));
	data_out.TBN = mat3(T, B, N);
}
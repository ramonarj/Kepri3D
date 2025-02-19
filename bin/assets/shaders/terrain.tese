#version 400 core

layout (quads, equal_spacing, ccw) in;

// Desde el TCS
in DATA
{
	vec2 TexCoords; 
	vec3 normals;
} data_in[];

// Para el FS
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
	vec3 fragPos;
	mat3 TBN;
	vec4 FragPosLightSpace;
} data_out;

// Uniform block
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};
uniform dmat4 model;

// Reutilizamos este nombre aunque lo usemos para alturas
uniform sampler2D dispMap;

uniform float elevacion = 1.0f;

void main()
{
	// Posición de este vértice en el parche abstracto
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
	
	// Interpolar bilinealmente las coordenadas de textura
    vec2 t0 = data_in[0].TexCoords;
    vec2 t1 = data_in[1].TexCoords;
    vec2 t2 = data_in[2].TexCoords;
    vec2 t3 = data_in[3].TexCoords;

    vec2 leftCoord = t0 + v * (t3 - t0);
    vec2 rightCoord = t1 + v * (t2 - t1);
    vec2 coord = leftCoord + u * (rightCoord - leftCoord);

	data_out.TexCoords = coord;
	
	// Lo mismo con la posición de los vértices
    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;
	
    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 pos = leftPos + u * (rightPos - leftPos);
	
	// Sumarle la altura del Height Map
	float y = texture(dispMap, coord).r;
	pos.y += (y * elevacion);

    gl_Position = vec4(projection * view * model * pos);
	
	data_out.fragPos = vec3(model * pos);
	
	// Aplicar la rotación del modelo a las normales
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	data_out.normals = normalMatrix * vec3(0, 1, 0);
	
	// Tangente, bitangente y normal
	vec3 N = normalize(normalMatrix * vec3(0, 1, 0));
	vec3 T = normalize(normalMatrix * vec3(1, 0, 0));
	vec3 B = normalize(cross(N, T));
	data_out.TBN = mat3(T, B, N);
}

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
} data_out;

void main()
{
	// Posición de este vértice en el parche abstracto
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
	
	// Posición de los 4 vértices que forman el parche
    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;
	
	// Interpolar
    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 pos = leftPos + u * (rightPos - leftPos);

    gl_Position = pos; // ya se multiplica por MVP en el VS
	data_out.TexCoords = vec2(0,0);
}

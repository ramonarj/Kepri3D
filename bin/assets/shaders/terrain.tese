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
	
	// Interpolar bilinearmente
    vec4 leftPos = pos0 + v * (pos3 - pos0);
    vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 pos = leftPos + u * (rightPos - leftPos);

    gl_Position = pos; // ya se multiplica por MVP en el VS
	
	// Lo mismo con las coordenadas de textura
    vec2 t0 = data_in[0].TexCoords;
    vec2 t1 = data_in[1].TexCoords;
    vec2 t2 = data_in[2].TexCoords;
    vec2 t3 = data_in[3].TexCoords;

    vec2 leftCoord = t0 + v * (t3 - t0);
    vec2 rightCoord = t1 + v * (t2 - t1);
    vec2 coord = leftCoord + u * (rightCoord - leftCoord);

	data_out.TexCoords = coord;
}

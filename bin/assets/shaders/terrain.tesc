#version 400 core

// Entrada del VS
in DATA
{
	vec2 TexCoords; 
	vec3 normals;
} data_in[];

// Para el TES
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
} data_out[];

// Tiene que coincidir con el 'glPatchParameteri()'
layout (vertices=4) out;

// Número de subdivisiones
int numDivExt = 1;
int numDivInt = 1;

uniform int subdivisions = 1;


void main()
{
	// Pasar la salida del VS al TES (posición y coordenadas de textura)
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	data_out[gl_InvocationID].TexCoords = data_in[gl_InvocationID].TexCoords;
	
	// ----------------------------------------------------------------------
    // invocation zero controls tessellation levels for the entire patch
    if (gl_InvocationID == 0)
    {
		// Divisiones exteriores (4)
        gl_TessLevelOuter[0] = subdivisions;
        gl_TessLevelOuter[1] = subdivisions;
        gl_TessLevelOuter[2] = subdivisions;
        gl_TessLevelOuter[3] = subdivisions;

		// Divisiones interiores (2)
        gl_TessLevelInner[0] = subdivisions;
        gl_TessLevelInner[1] = subdivisions;
    }
}

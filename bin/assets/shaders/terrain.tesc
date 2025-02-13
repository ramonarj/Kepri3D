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

// Variables uniforms
uniform dmat4 model;
uniform vec3 viewPos;
uniform vec3 camFW;
uniform bool use_eyeDir = false;

// Constantes
const int MIN_TESS = 1;
const int MAX_TESS = 6;

void main()
{
	// Pasar la salida del VS al TES (posición y coordenadas de textura)
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	data_out[gl_InvocationID].TexCoords = data_in[gl_InvocationID].TexCoords;
	
	// El primer vértice de cada parche establece los niveles de teselación para los sí mismo y los demás
    if (gl_InvocationID == 0)
    {
		// Centro del parche
		vec3 cArr = vec3(model * (gl_in[0].gl_Position + (gl_in[3].gl_Position - gl_in[0].gl_Position) / 2.0)); // top side
		vec3 cAba = vec3(model * (gl_in[1].gl_Position + (gl_in[2].gl_Position - gl_in[1].gl_Position) / 2.0)); // bot side
		vec3 centro = (cAba + (cArr - cAba) / 2.0).xyz;
	
		// Se usa la dirección en que está mirando la cámara, y no su posición
		int subd;
		if(use_eyeDir)
		{
			vec3 eyetoCenter = normalize(centro - viewPos);
			vec3 camDir = normalize(camFW);
			subd = int(max(dot(MAX_TESS * eyetoCenter, camDir), MIN_TESS));
		}
		// Distancia al centro del parche
		else
		{
			float dist = length(viewPos - centro);
			subd = int(max(min(25.0 - dist, MAX_TESS), MIN_TESS));
		}
		
		// Se podría hacer una 'mezcla'; si estamos dentro del parche, usamos la máxima teselación, 
		// y si no, usamos la dirección de la cámara
	
		// Divisiones exteriores (4)
        gl_TessLevelOuter[0] = subd;
        gl_TessLevelOuter[1] = subd;
        gl_TessLevelOuter[2] = subd;
        gl_TessLevelOuter[3] = subd;

		// Divisiones interiores (2)
        gl_TessLevelInner[0] = subd;
        gl_TessLevelInner[1] = subd;
    }
}

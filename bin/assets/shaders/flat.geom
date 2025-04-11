#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Variables que nos llegan del vertex shader
in DATA
{
	vec2 TexCoords; 
	vec3 normals;
	vec3 fragPos;
	mat3 TBN;
	// Para sombras
	vec4 FragPosLightSpace;
} data_in[];

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
	vec3 normals;
	vec3 fragPos;
	mat3 TBN;
	// Para sombras
	vec4 FragPosLightSpace;
} data_out;


void main() 
{    
	// Normal del triángulo (flat shading)
	vec3 normalTri = (data_in[0].normals + data_in[1].normals + data_in[2].normals) / 3.0;
	
	// Ponerle esa normal a los 3 vérticess
	for(int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position; 
		data_out.TexCoords = data_in[i].TexCoords;
		data_out.normals = normalTri;
		data_out.fragPos = data_in[i].fragPos;
		data_out.TBN = data_in[i].TBN;
		data_out.FragPosLightSpace = data_in[i].FragPosLightSpace; // Para sombras
		
		EmitVertex();
	}
    
    EndPrimitive();
}
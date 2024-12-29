#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Variables que nos llegan del vertex shader
in DATA
{
	vec2 TexCoords;
	vec3 normals;
} data_in[];

// Para el siguiente shader
out DATA
{
	vec2 TexCoords;
	vec3 normals;
} data_out;

// Deja los triángulos tal y como están
void main() 
{    
	// Cada vez que se llama a EmitVertex(), todas las subsiguientes funciones se refieren al siguiente vértice
    gl_Position = gl_in[0].gl_Position; 
	data_out.TexCoords = data_in[0].TexCoords;
    EmitVertex();
	
    gl_Position = gl_in[1].gl_Position;
	data_out.TexCoords = data_in[1].TexCoords;
    EmitVertex();
	
	gl_Position = gl_in[2].gl_Position;
	data_out.TexCoords = data_in[2].TexCoords;
    EmitVertex();
    
    EndPrimitive();
}
#version 430 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

// Variables que nos llegan del vertex shader
in DATA
{
	vec4 normalEnd;
} data_in[];

// El fragment shader no necesita nada de nosotros

// Dibuja una cruz en cada vértice
void main() 
{    
	// Los 3 vértices
	for(int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position; 
		EmitVertex();
		gl_Position = data_in[i].normalEnd;
		EmitVertex();
		
		EndPrimitive();
	}
	// En principio no hay problema con usar un bucle 'for' (de 3 iteraciones) en vez 
	// de repetir el código de arriba 3 veces
}
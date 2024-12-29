#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

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

// Dibuja una cruz en cada vértice
void main() 
{    
	// Raya horizontal
	
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
	data_out.TexCoords = data_in[0].TexCoords;
    EmitVertex();
	
    gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	data_out.TexCoords = data_in[0].TexCoords;
    EmitVertex();
	
	EndPrimitive();
	
	// Raya vertical
	
	gl_Position = gl_in[0].gl_Position + vec4(0.0, -0.1, 0.0, 0.0); 
	data_out.TexCoords = data_in[0].TexCoords;
    EmitVertex();
	
    gl_Position = gl_in[0].gl_Position + vec4(0.0, 0.1, 0.0, 0.0);
	data_out.TexCoords = data_in[0].TexCoords;
    EmitVertex();
    
    EndPrimitive();
}
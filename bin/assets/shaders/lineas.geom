#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

// Necesario para que fluyan los datos del VS al FS
out vec2 TexCoords;

// Dibuja pequeñas líneas paralelas a la cámara en cada vértice
void main() 
{    
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
	
	gl_Position = gl_in[0].gl_Position + vec4(0.0, -0.1, 0.0, 0.0); 
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + vec4( 0.0, 0.1, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
}
#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

// Dibuja pequeñas líneas paralelas a la cámara en cada vértice
void main() 
{    
    gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0); 
    EmitVertex();

    gl_Position = gl_in[0].gl_Position + vec4( 0.1, 0.0, 0.0, 0.0);
    EmitVertex();
    
    EndPrimitive();
}
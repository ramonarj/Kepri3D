#version 430 core

// Un vertex shader muy sencillo para usarlo en cualquier efecto 
// de post-processing, ya que no solemos necesitar más que la 
// posición del fragmento y la coordenada de textura correspondiente

in vec4 vertex;
layout(location = 2) in vec2 uv0;

// Para el siguiente shader
out DATA
{
	vec2 TexCoords; 
} data_out;

void main()
{
	gl_Position = vertex;
	
	data_out.TexCoords = uv0;
}
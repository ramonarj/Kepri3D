#version 330 core

in vec2 TexCoords;
in vec4 gl_FragCoord;

// Se coge automáticamente
uniform sampler2D textura;

out vec4 FragColor;

void main()
{
	//Cogemos el color de la textura
	vec3 color = vec3(texture(textura, TexCoords));
	
	FragColor = vec4(color, 1.0 - gl_FragCoord.z);
}
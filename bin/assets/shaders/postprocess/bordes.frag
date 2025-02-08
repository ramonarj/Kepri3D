#version 330 core 

// Variables de entrada
in DATA
{
	vec2 TexCoords; 
} data_in;

// Textura con la escena
uniform sampler2D RTT0;

out vec4 fragColor;

// Con 0.5 queda muy chulo
const float limite = 0.5;
const vec4 colorBorde = vec4(0, 1, 0, 1);
const vec4 colorFondo = vec4(0.2, 0.1, 0.2, 1.0);

void main()
 {
	 // Color de la textura
	vec4 colorTex = texture(RTT0, data_in.TexCoords);
	// Cómo varía el color de la textura respecto a los fragmentos adyacentes
	vec4 incrY = dFdy(colorTex);
	vec4 incrX = dFdx(colorTex);

	vec4 pxColor;
	// Interpolación lineal
	pxColor = mix(colorFondo, colorBorde, (length(incrY) + length(incrX)) / 1.5);
	/*
	// Sin interpolación lineal (más "retro")
	// Con 'or' (||) quedan bordes más gruesos, con 'and' (&&), más finos
	if(length(incrY) > limite && length(incrX) > limite)
		pxColor = colorBorde;
	else
		pxColor = texColor;
	*/
	fragColor = pxColor;
 }
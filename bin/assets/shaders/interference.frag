#version 330 core 

// Variables de entrada
in DATA
{
	vec2 TexCoords; 
} data_in;

// Textura con la escena
uniform sampler2D RTT0;

out vec4 fragColor;

void main()
 {
	vec4 sceneColor = texture(RTT0, data_in.TexCoords);

	//Coloreamos 3 líneas en escala de grises...
	int linea = int(gl_FragCoord.y) % 6;
	if(linea < 3)
	{
		fragColor = vec4(sceneColor.r * 0.6, sceneColor.g * 0.6, sceneColor.b * 0.6, sceneColor.a);
	} 
	//... y 3 líneas las dejamos normales
	else
	{
		fragColor = sceneColor;
	}
 }
#version 430 core

in vec4 vertex;

// No necesitamos la matriz de modelado del cubemap (va pegado a la cámara)
layout (std140) uniform Matrices
{
    dmat4 projection;
    dmat4 view;
};
uniform vec3 viewPos;

out vec3 textureDir; 

void main()
{
	vec4 pos = vec4(projection * view * vec4(viewPos + vertex.xyz, 1.0));
	
	textureDir = vec3(vertex.x, -vertex.y, -vertex.z);
	// División de perspectiva; para que el valor Z siempre valga 1 (w / w = 1) y se pinte solo cuando no haya nada ya pintado
	gl_Position = pos.xyww;
	//gl_Position = pos;
}
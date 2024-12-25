#version 330 core
in vec4 vertex;
uniform mat4 modelMat;
void main()
{
	mat4 View = mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        1, 0, 0, 1
    );
gl_Position = View * vec4(vertex.x / 5.0, vertex.y / 5.0, vertex.z/5.0, 1.0);
}
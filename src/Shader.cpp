#include "Shader.h"

#include <glew.h>
#include <iostream>

void Shader::load(const char* vertexShaderSrc, const char* fragmentShaderSrc)
{
	if (!GLEW_ARB_shading_language_100 || !GLEW_ARB_vertex_shader || 
		!GLEW_ARB_fragment_shader || !GLEW_ARB_shader_objects)
	{
		std::cerr << "Shaders not available" << std::endl;
	}
	// Identificadores
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Pasarle el código fuente a los identificadores
	glShaderSource(vertexShader, 1, &vertexShaderSrc, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSrc, NULL);

	// Compilarlos
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	// Comprobar que la compilación ha sido exitosa
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	// Crear un programa de shaders y asignarle el VS y FS
	programId = glCreateProgram();
	//glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);

	// Comprobar que ha sido exitoso
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
	}

	// Una vez enlazados, podemos borrar los VS y FS
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}
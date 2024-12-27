#include "Shader.h"

#include <glew.h>
#include <iostream>

Shader::Shader() : programId(0)
{
	if (!GLEW_ARB_shading_language_100 || !GLEW_ARB_vertex_shader ||
		!GLEW_ARB_fragment_shader || !GLEW_ARB_shader_objects)
	{
		std::cerr << "Shaders not available" << std::endl;
	}

	// Inicializas los IDs a un valor erróneo (0)
	for (int i = 0; i < 3; i++)
		shadersIds[i] = 0;
}

void Shader::load(const char* vertexShaderSrc, const char* fragmentShaderSrc)
{

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
	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);

	// Comprobar que ha sido exitoso
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success) 
	{
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "ERROR: shader program linking went wrong" << std::endl;
	}

	// Una vez enlazados, podemos borrar los VS y FS
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::load(GLenum shaderType, const char* shaderSrc)
{
	// Identificador y crear el shader
	unsigned int shaderId = glCreateShader(shaderType);

	// Pasarle el código fuente
	glShaderSource(shaderId, 1, &shaderSrc, NULL);

	// Compilarlo y comprobar errores
	glCompileShader(shaderId);
	int  success;
	char infoLog[512];
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Guardarlo en el array de IDs
	switch(shaderType)
	{
	case GL_VERTEX_SHADER:
		shadersIds[0] = shaderId;
		break;
	case GL_GEOMETRY_SHADER:
		shadersIds[1] = shaderId;
		break;
	case GL_FRAGMENT_SHADER:
		shadersIds[2] = shaderId;
		break;
	default:
		break;
	}
}

void Shader::link()
{
	// Crear un programa de shaders y asignarle los shaders creados
	programId = glCreateProgram();
	for(int i = 0; i < 3; i++)
	{
		if(shadersIds[i] != 0)
			glAttachShader(programId, shadersIds[i]);
	}	

	// Compilarlo y comprobar que ha sido exitoso
	glLinkProgram(programId);

	int  success;
	char infoLog[512];
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programId, 512, NULL, infoLog);
		std::cout << "ERROR: shader program linking went wrong" << std::endl;
	}

	// Una vez enlazados, podemos borrar los shaders
	for (int i = 0; i < 3; i++)
	{
		if (shadersIds[i] != 0)
			glDeleteShader(shadersIds[i]);
	}
}
#include "Shader.h"

#include <glew.h>
#include <iostream>

#include <gtc/type_ptr.hpp>

Shader::Shader() : programId(0)
{
	// Por si no est�n disponibles los shaders
	if (!GLEW_ARB_shading_language_100 || !GLEW_ARB_vertex_shader ||
		!GLEW_ARB_fragment_shader || !GLEW_ARB_shader_objects)
	{
		std::cerr << "ERROR: Shaders not available" << std::endl;
	}

	// Inicializar los IDs a un valor err�neo (0)
	for (int i = 0; i < 5; i++)
		shadersIds[i] = 0;
}


void Shader::load(GLenum shaderType, const char* shaderSrc)
{
	// Identificador y crear el shader
	unsigned int shaderId = glCreateShader(shaderType);

	// Pasarle el c�digo fuente
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
	case GL_TESS_CONTROL_SHADER:
		shadersIds[1] = shaderId;
		break;
	case GL_TESS_EVALUATION_SHADER:
		shadersIds[2] = shaderId;
		break;
	case GL_GEOMETRY_SHADER:
		shadersIds[3] = shaderId;
		break;
	case GL_FRAGMENT_SHADER:
		shadersIds[4] = shaderId;
		break;
	default:
		break;
	}
}

void Shader::link()
{
	// Crear un programa de shaders y asignarle los shaders creados
	programId = glCreateProgram();
	for(int i = 0; i < 5; i++)
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
		std::cout << infoLog << std::endl;
	}

	// Una vez enlazados, podemos borrar los shaders
	for (int i = 0; i < 5; i++)
	{
		if (shadersIds[i] != 0)
			glDeleteShader(shadersIds[i]);
	}
}


// - - - - - - - - - - - - - - - - - 

void Shader::setInt(const std::string& name, int value) const
{
	GLint loc = glGetUniformLocation(programId, name.c_str());
	glUniform1i(loc, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	GLint loc = glGetUniformLocation(programId, name.c_str());
	glUniform1f(loc, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2 value) const
{
	GLint loc = glGetUniformLocation(programId, name.c_str());
	glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3 value) const
{
	GLint loc = glGetUniformLocation(programId, name.c_str());
	glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, const glm::vec4 value) const
{
	GLint loc = glGetUniformLocation(programId, name.c_str());
	glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Shader::setMat4d(const std::string& name, const glm::dmat4 matValue) const
{
	GLint loc = glGetUniformLocation(programId, name.c_str());
	glUniformMatrix4dv(loc, 1, GL_FALSE, glm::value_ptr(matValue));
}
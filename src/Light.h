#ifndef __LIGHT__
#define __LIGHT__
#include <glew.h>
#include <glm.hpp>

class Light
{
public:
	/* Constructora por defecto */
	Light(glm::fvec4 diffuse = {1, 1, 1, 1});

	/* Destructora */
	~Light();

	/* Carga las propiedades de la luz en OpenGL */
	virtual void load(glm::dmat4 viewMat);

	/* Enciende/apaga la luz */
	void setActive(bool active);

	/* Cambia la luz ambiente */
	inline void setAmbient(glm::fvec4 ambient) { this->ambient = ambient; }

	// Getters
	/* Devuelve la posici�n */
	inline glm::fvec4 getPosition() const { return posDir; }

	// Setters
	/* Cambia la posici�n de la luz, y la convierte en puntual */
	inline void setPosition(glm::fvec3 pos) { posDir = glm::fvec4(pos, 1.0f); }

	/* Cambia la direcci�n de la luz, y la convierte en direccional */
	inline void setDirection(glm::fvec3 dir) { posDir = glm::fvec4(-dir, 0.0f); } //-dir

protected:
	/* N�mero de luces totales en la escena */
	static GLuint cont;

	/* Identificador de la luz actual */
	GLuint id;

	/* Posici�n / direcci�n de la luz; si la componente w = 1, es una posici�n. Si w = 0, es un vector.
	(coordenadas homog�neas) */
	glm::fvec4 posDir;

	//Usamos floats para las distintas componentes de la luz
	/* Componente ambiente de la luz */
	glm::fvec4 ambient;
	/* Componente difusa de la luz */
	glm::fvec4 diffuse;
	/* Componente especular de la luz */
	glm::fvec4 specular;
};

#endif
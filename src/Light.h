#ifndef __LIGHT__
#define __LIGHT__

#include <glew.h>
#include <glm.hpp>

enum LightType
{
	POINT_LIGHT = 0, // luz puntual; emite en todas direcciones
	SPOT_LIGHT = 1, // foco; tiene un ángulo concreto de emisión
	DIRECTIONAL_LIGHT = 2 // direccional; simula una luz remota uniforme (Sol/Luna)
};

class Light
{
public:
	/* Constructora por defecto */
	Light(LightType type, glm::fvec4 diffuse = {1, 1, 1, 1});

	/* Destructora */
	~Light();

	/* Carga las propiedades de la luz en OpenGL */
	virtual void load(glm::dmat4 viewMat);


	// Getters
	/* Devuelve 'true' si la luz está encendida, false e.o.c. */
	inline bool isActive() const { return m_active; }

	/* Devuelve la posición */
	inline glm::fvec3 getPosition() const { return posDir; }

	// Setters
	/* Enciende/apaga la luz */
	void setActive(bool active);

	/* Cambia la luz ambiente */
	inline void setAmbient(glm::fvec4 ambient) { this->ambient = ambient; }

	/* Cambia la posición de la luz, y la convierte en puntual */
	inline void setPosition(glm::fvec3 pos) { posDir = glm::fvec4(pos, 1.0f); }

	/* Cambia la dirección de la luz, y la convierte en direccional */
	inline void setDirection(glm::fvec3 dir) { posDir = glm::fvec4(-dir, 0.0f); } //-dir

	/* Establece los factores de atenuación de esta luz (EXCLUSIVO PARA PUNTUALES) */
	void setAttenuationFactors(float constant, float linear, float quadratic);

	// Exclusivo para focos
	/* Establece la dirección del foco de luz */
	void setSpotlightDir(glm::fvec3 dir);

	/* Establece el ángulo de apertura del foco */
	void setSpotlightAngle(GLfloat angle);

	/* Establece la 'dureza' del foco (0 - 128). Cuanto mayor sea, menos diferencia de luz entre el centro y los bordes */
	void setSpotlightHardness(GLfloat angle);

protected:
	/* Número de luces totales en la escena */
	static GLuint cont;

	/* Tipo de luz que es (puntual / foco / direccional) */
	LightType type;

	/* Identificador de OpengGL asociado a esta luz */
	GLuint id;

	/* Si la luz está encendida o no */
	bool m_active;

	/* Posición / dirección de la luz; si la componente w = 1, es una posición. Si w = 0, es un vector.
	(coordenadas homogéneas) */
	glm::fvec4 posDir;

	//Usamos floats para las distintas componentes de la luz
	/* Componente ambiente de la luz */
	glm::fvec4 ambient;
	/* Componente difusa de la luz */
	glm::fvec4 diffuse;
	/* Componente especular de la luz */
	glm::fvec4 specular;

	/* Factores de atenuación de la luz (solo para puntuales/focos) */
	float constantAtt;
	float linearAtt;
	float quadraticAtt;

	// SOLO PARA FOCOS
	/* Dirección del foco de luz */
	glm::fvec3 spotDir;

	/* Ángulo del foco */
	GLfloat spotAngle;

	/* Intensidad en la distribución de luz del foco */
	GLfloat spotExp;
};

#endif
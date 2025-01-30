#ifndef __LIGHT__
#define __LIGHT__

#include <glew.h>
#include <glm.hpp>

enum LightType
{
	DIRECTIONAL_LIGHT = 0, // direccional; simula una luz remota uniforme (Sol/Luna)
	POINT_LIGHT = 1, // luz puntual; emite en todas direcciones
	SPOT_LIGHT = 2 // foco; tiene un ángulo concreto de emisión
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

	/* Devuelve el tipo de luz que es */
	inline LightType getType() const { return type; }

	/* Devuelve la posición */
	inline const glm::fvec3& getPosition() const { return posDir; }

	/* Devuelve la componente ambient de la luz */
	inline const glm::fvec3& getAmbient() const { return ambient; }

	/* Devuelve la componente difusa de la luz*/
	inline const glm::fvec3& getDiffuse() const { return diffuse; }

	/* Devuelve la componente especular de la luz */
	inline const glm::fvec3& getSpecular() const { return specular; }

	/* Devuelve el factor de atenuación de la luz (0 = constante, 1 = lineal, 2 = cuadrático) */
	float getAttenuation(GLuint i) const;

	// Setters
	/* Enciende/apaga la luz */
	void setActive(bool active);

	/* Cambia la luz difusa */
	inline void setDiffuse(glm::fvec4 diffuse) { this->diffuse = diffuse; }

	/* Cambia la luz ambiente */
	inline void setAmbient(glm::fvec4 ambient) { this->ambient = ambient; }

	/* Cambia la luz difusa */
	inline void setSpecular(glm::fvec4 specular) { this->specular = specular; }

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
	void setSpotlightCutoff(GLfloat angle);

	/* Establece la 'dureza' del foco (0 - 128). Cuanto mayor sea, menos diferencia de luz entre el centro y los bordes */
	void setSpotlightHardness(GLfloat angle);
	
	// Getters
	inline glm::vec3 getSpotDirection() const { return spotDir; }
	inline float getSpotCutoff() const { return spotCutoff; }
	inline float getSpotExponent() const { return spotExp; }


protected:
	/* Número de luces totales en la escena */
	static GLuint cont;

	/* Tipo de luz que es (direccional / puntual / foco) */
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
	GLfloat spotCutoff;

	/* Intensidad en la distribución de luz del foco */
	GLfloat spotExp;
};

#endif
#ifndef __LIGHT__
#define __LIGHT__

#include <glew.h>
#include <glm.hpp>
#include "Component.h"

class Framebuffer;
class Shader;
class Uniformbuffer;

const unsigned int SHADOW_SIZE = 1024;

enum LightType
{
	DIRECTIONAL_LIGHT = 0, // direccional; simula una luz remota uniforme (Sol/Luna)
	POINT_LIGHT = 1, // luz puntual; emite en todas direcciones
	SPOT_LIGHT = 2 // foco; tiene un ángulo concreto de emisión
};

// - - - - - - - - - - - - - - - - - - - - 

struct Shadowmap
{
	Shadowmap() : depthBuf(nullptr), shader(nullptr), nearPlane(1.0), farPlane(15.0) {}
	Shadowmap(Shader* shader, unsigned int width, unsigned int height, 
		float nearPlane, float farPlane, bool omnidirectional);

	~Shadowmap();

	void clean() { if (depthBuf != nullptr) delete depthBuf; }

	// El buffer con la textura donde ese guardan los datos de profundidad
	Framebuffer* depthBuf;
	// El shader con el que debe hacerse el pase de sombras
	Shader* shader;
	// La resolución del mapa
	unsigned int width;
	unsigned int height;
	// Planos cercano y lejano
	float nearPlane;
	float farPlane;
	// PCF. Esto conlleva un golpe importante al rendimiento, sobre todo en luces puntuales 
	// si el tam.filtro es >= 3 (~-40 fps)
	bool softShadows = true;

	// Solo para luces direccionales //
	float distOrigen = 60.0f;
	glm::dmat4 lightView, lightProj;
};

// - - - - - - - - - - - - - - - - - - - - 

class Light : public Component
{
public:
	/* Constructora por defecto */
	Light(LightType type, glm::fvec4 diffuse = {1, 1, 1, 1});

	/* Destructora */
	~Light();

	void update(GLuint deltaTime) override;

	/* Carga las propiedades de la luz en OpenGL */
	virtual void load(glm::dmat4 viewMat);

	/* Activa / desactiva las sombras emitidas por esta luz */
	void emitShadows(bool b);

	// Getters
	/* Devuelve 'true' si la luz está encendida, false e.o.c. */
	inline bool isActive() const { return m_active; }

	/* Devuelve el tipo de luz que es */
	inline LightType getType() const { return type; }

	/* Componentes de la luz */
	inline const glm::fvec3& getAmbient() const { return ambient; }
	inline const glm::fvec3& getDiffuse() const { return diffuse; }
	inline const glm::fvec3& getSpecular() const { return specular; }

	/* Devuelve el factor de atenuación de la luz (0 = constante, 1 = lineal, 2 = cuadrático) */
	float getAttenuation(GLuint i) const;

	/* Devuelve el mapa de sombras */
	inline Shadowmap* getShadowMap() { return m_shadowMap; }

	/* Manda los uniforms */
	void sendShadowUniforms(Uniformbuffer* uboMatrices);

	/* Rellena parte del UBO con la información de esta luz */
	void loadToUBO(unsigned int offset);

	// Setters
	/* Enciende/apaga la luz */
	void setActive(bool active);

	/* Componentes de la luz */
	inline void setDiffuse(glm::fvec4 diffuse) { this->diffuse = diffuse; }
	inline void setAmbient(glm::fvec4 ambient) { this->ambient = ambient; }
	inline void setSpecular(glm::fvec4 specular) { this->specular = specular; }

	/* Cambia la dirección de la luz, y la convierte en direccional */
	inline void setDirection(glm::fvec3 dir) { direction = glm::fvec4(-dir, 0.0f); } //-dir
	inline const glm::fvec3& getDirection() const { return direction; }

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

	/* Número de luces totales en la escena */
	static GLuint cont;

protected:
	/* Tipo de luz que es (direccional / puntual / foco) */
	LightType type;

	/* Identificador de OpengGL asociado a esta luz */
	GLuint id;

	/* Si la luz está encendida o no */
	bool m_active;

	/* Dirección de la luz, para direccionales */
	glm::fvec3 direction;

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

	// MAPA DE SOMBRAS
	Shadowmap* m_shadowMap;
};

#endif
#ifndef __RIGID__
#define __RIGID__

#include "Component.h"
#include <glm.hpp>

class Rigid : public Component
{
public:
	/* Gravedad global */
	static glm::dvec3 s_gravity;

	Rigid(const glm::dmat4& modelMat);
	~Rigid(){}

	/* Actualiza la posición y velocidad del Rigid */
	void update(GLuint deltaTime) override;

	/* Indica si el objeto será afectado por la fuerza de la Gravedad */
	inline void useGravity(bool use) { m_useGravity = use; }
private:
	/* Puntero a la posición del Rigid */
	glm::dvec3* m_position;

	/* Velocidad del Rigid */
	glm::dvec3 m_velocity;

	/* Aceleración del Rigid */
	glm::dvec3 m_acceleration;

	/* ¿Le afecta la gravedad? */
	bool m_useGravity;

	/* Rozamiento (=resistencia a moverse) del Rigid */
	float m_drag;
};

#endif
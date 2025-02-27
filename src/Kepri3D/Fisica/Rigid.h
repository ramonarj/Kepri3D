#ifndef __RIGID__
#define __RIGID__

#include "Component.h"
#include <glm.hpp>

class Collider;

class Rigid : public Component
{
public:
	/* Gravedad global */
	static glm::dvec3 s_gravity;

	Rigid(const glm::dmat4& modelMat);
	~Rigid(){}

	/* Actualiza la posici�n y velocidad del Rigid */
	void update(GLuint deltaTime) override;

	// Aplicaci�n de fuerzas
	/* Ejerce una fuerza con la direcci�n y magnitud dadas sobre el Rigid */
	void addForce(const glm::vec3& force);

	/* Ejerce una fuerza sobre el Rigid en el punto dado (coords. locales respecto al centro de masa) */
	void addForce(const glm::vec3& force, const glm::vec3& point);

	/* Aplica un torque al Rigid (t = r x F) */
	void addTorque(const glm::vec3& torque);

	// Setters
	/* Indica si el objeto ser� afectado por la fuerza de la Gravedad */
	inline void useGravity(bool use) { m_useGravity = use; }

	/* Cambia la masa del Rigidbody */
	inline void setMass(float mass) { m_mass = mass; }

	/* Cambia el rozamiento est�tico/din�mico del Rigidbody */
	inline void setDrag(float drag) { m_drag = drag; }

	/* Cambia el rozamiento angular del Rigidbody */
	inline void setAngularDrag(float angDrag) { m_angularDrag = angDrag; }

	/* Enlaza el rigid a su collider */
	inline void setCollider(Collider* col) { m_collider = col; }
private:
	friend class PhysicsSystem;

	/* Puntero a la posici�n del Rigid */
	glm::dvec3* m_position;

	/* Velocidad del Rigid */
	glm::dvec3 m_velocity;

	/* Aceleraci�n del Rigid */
	glm::dvec3 m_acceleration;

	/* Torque acumulado del Rigid */
	glm::dvec3 m_torque;

	/* �Le afecta la gravedad? */
	bool m_useGravity;

	/* Rozamiento (=resistencia a moverse) del Rigid */
	float m_drag;

	/* Rozamiento angular (=resistencia a rotar) del Rigid */
	float m_angularDrag;

	/* Masa del Rigid */
	float m_mass;

	/* Puntero al collider de la entidad */
	Collider* m_collider;
};

#endif
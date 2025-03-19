#ifndef __RIGID__
#define __RIGID__

#include "Component.h"
#include <glm.hpp>

class Collider;

enum RigidType
{
	Dynamic = 0,
	Static = 1
};

class Rigid : public Component
{
public:
	/* Gravedad global */
	static glm::dvec3 s_gravity;

	Rigid(const glm::dmat4& modelMat, RigidType type = Dynamic);
	~Rigid(){}

	/* Actualiza la posición y velocidad del Rigid */
	void update(GLuint deltaTime) override;

	// Aplicación de fuerzas
	/* Ejerce una fuerza con la dirección y magnitud dadas sobre el Rigid */
	void addForce(const glm::vec3& force);

	/* Ejerce una fuerza sobre el Rigid en el punto dado (coords. locales respecto al centro de masa) */
	void addForce(const glm::vec3& force, const glm::vec3& point);

	/* Aplica un torque al Rigid (t = r x F) */
	void addTorque(const glm::vec3& torque);

	// Setters
	/* Indica si el objeto será afectado por la fuerza de la Gravedad */
	inline void useGravity(bool use) { m_useGravity = use; }

	/* Cambia la masa del Rigidbody */
	inline void setMass(float mass) { m_mass = mass; }

	/* Cambia el rozamiento estático/dinámico del Rigidbody */
	inline void setDrag(float drag) { m_drag = drag; }

	/* Cambia el rozamiento angular del Rigidbody */
	inline void setAngularDrag(float angDrag) { m_angularDrag = angDrag; }

	/* Enlaza el rigid a su collider */
	inline void setCollider(Collider* col) { m_collider = col; }

	/* Cambia el tipo del Rigidbody entre dinámico y estático */
	inline void setType(RigidType type) { m_type = type; }

	/* Cambia la velocidad del Rigid */
	void setVelocity(const glm::dvec3& vel);

	/* Despierta al Rigid */
	void wakeUp();
	/* Manda el Rigid a dormir */
	void sleep();

private:
	friend class PhysicsSystem;

	/* Tipo de Rigidbody */
	RigidType m_type;

	/* Puntero a la posición del Rigid */
	glm::dvec3* m_position;

	/* Velocidad del Rigid */
	glm::dvec3 m_velocity;

	/* Aceleración del Rigid */
	glm::dvec3 m_acceleration;

	/* Suma de las fuerzas ejercidas durante este frame */
	glm::vec3 m_accumForces;

	/* Velocidad angular del Rigid */
	glm::dvec3 m_angularVel;

	/* Aceleración angular del Rigid */
	glm::dvec3 m_angularAcc;

	/* Suma de las fuerzas de torque ejercidas durante este frame */
	glm::vec3 m_accumTorque;

	/* ¿Le afecta la gravedad? */
	bool m_useGravity;

	/* Rozamiento (=resistencia a moverse) del Rigid */
	float m_drag;

	/* Rozamiento angular (=resistencia a rotar) del Rigid */
	float m_angularDrag;

	/* Masa del Rigid */
	float m_mass;

	/* Puntero al collider de la entidad */
	Collider* m_collider;

	/* ¿Está dormido? */
	bool m_sleeping;

	/* Los frames que lleva con una velocidad ínfima */
	unsigned int m_framesInactivo;
};

#endif
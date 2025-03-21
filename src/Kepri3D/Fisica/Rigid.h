#ifndef __RIGID__
#define __RIGID__

#include "CorePhysics.h"
#include "Component.h"

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

	/* Actualiza la posici�n y velocidad del Rigid */
	void update(GLuint deltaTime) override;

	// Aplicaci�n de fuerzas
	/* Ejerce una fuerza con la direcci�n y magnitud dadas sobre el Rigid */
	void addForce(const vector3& force);

	/* Ejerce una fuerza sobre el Rigid en el punto dado (coords. locales respecto al centro de masa) */
	void addForce(const vector3& force, const vector3& point);

	/* Aplica un torque al Rigid (t = r x F) */
	void addTorque(const vector3& torque);

	// Setters
	/* Indica si el objeto ser� afectado por la fuerza de la Gravedad */
	inline void useGravity(bool use) { m_useGravity = use; }

	/* Cambia la masa del Rigidbody */
	inline void setMass(real mass) { m_mass = mass; }

	/* Cambia el rozamiento est�tico/din�mico del Rigidbody */
	inline void setDrag(real drag) { m_drag = drag; }

	/* Cambia el rozamiento angular del Rigidbody */
	inline void setAngularDrag(real angDrag) { m_angularDrag = angDrag; }

	/* Enlaza el rigid a su collider */
	inline void setCollider(Collider* col) { m_collider = col; }

	/* Cambia el tipo del Rigidbody entre din�mico y est�tico */
	inline void setType(RigidType type) { m_type = type; }

	/* Cambia la velocidad del Rigid */
	void setVelocity(const vector3& vel);

	/* Despierta al Rigid */
	void wakeUp();
	/* Manda el Rigid a dormir */
	void sleep();

private:
	friend class PhysicsSystem;

	/* Tipo de Rigidbody */
	RigidType m_type;

	/* Puntero a la posici�n del Rigid */
	glm::dvec3* m_position;

	/* Velocidad del Rigid */
	vector3 m_velocity;

	/* Aceleraci�n del Rigid */
	vector3 m_acceleration;

	/* Suma de las fuerzas ejercidas durante este frame */
	vector3 m_accumForces;

	/* Velocidad angular del Rigid */
	vector3 m_angularVel;

	/* Aceleraci�n angular del Rigid */
	vector3 m_angularAcc;

	/* Suma de las fuerzas de torque ejercidas durante este frame */
	vector3 m_accumTorque;

	/* �Le afecta la gravedad? */
	bool m_useGravity;

	/* Rozamiento (=resistencia a moverse) del Rigid */
	real m_drag;

	/* Rozamiento angular (=resistencia a rotar) del Rigid */
	real m_angularDrag;

	/* Masa del Rigid */
	real m_mass;

	/* Puntero al collider de la entidad */
	Collider* m_collider;

	/* �Est� dormido? */
	bool m_sleeping;

	/* Los frames que lleva con una velocidad �nfima */
	unsigned int m_framesInactivo;
};

#endif
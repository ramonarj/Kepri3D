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
	Rigid(const glm::dmat4& modelMat, RigidType type = Dynamic);
	~Rigid(){}

	void update(float deltaTime) override {}

	// Aplicación de fuerzas
	/* Ejerce una fuerza con la dirección y magnitud dadas sobre el Rigid */
	void addForce(const vector3& force);

	/* Ejerce una fuerza sobre el Rigid en el punto dado (coords. locales respecto al centro de masa) */
	void addForce(const vector3& force, const vector3& point);

	/* Aplica un torque al Rigid (t = r x F) */
	void addTorque(const vector3& torque);

	/* Aplica un impulso al Rigid (I = m * v), cambiando su velocidad instantáneamente */
	void addImpulse(const vector3& impulse);

	// Setters
	inline void setMass(real mass) { m_mass = mass; }
	inline void setDrag(real drag) { m_drag = drag; }
	inline void setAngularDrag(real angDrag) { m_angularDrag = angDrag; }
	inline void setCollider(Collider* col) { m_collider = col; }
	inline void setType(RigidType type) { m_type = type; }

	/* Indica si el objeto será afectado por la fuerza de la Gravedad */
	inline void useGravity(bool use) { m_useGravity = use; }

	/* Cambia la velocidad del Rigid */
	void setVelocity(const vector3& vel);

	// Otros
	/* Despierta al Rigid */
	void wakeUp();
	/* Manda el Rigid a dormir */
	void sleep();

private:
	friend class PhysicsSystem;
	friend class Liquido;
	friend class Muelle;
	friend class Articulacion;

	/* Tipo de Rigidbody */
	RigidType m_type;

	/* Puntero a la posición del Rigid */
	glm::dvec3* m_position;

	/* Velocidades y aceleraciones */
	vector3 m_velocity;
	vector3 m_acceleration;
	vector3 m_angularVel;
	vector3 m_angularAcc;

	/* Sumatorio de las fuerzas/torques ejercidas durante este frame */
	vector3 m_accumForces;
	vector3 m_accumTorque;

	/* ¿Le afecta la gravedad? */
	bool m_useGravity;

	/* Rozamiento lineal (=resistencia a moverse) y angular (=resistencia a rotar) */
	real m_drag;
	real m_angularDrag;

	/* Masa */
	real m_mass;

	/* Puntero al collider de la entidad */
	Collider* m_collider;

	/* ¿Está dormido? */
	bool m_sleeping;

	/* Los frames que lleva con una velocidad ínfima */
	float m_tiempoInactivo;

	// Métodos auxiliares
	/* Actualiza la posición y velocidad del Rigid */
	void integrate(real t);
	// Actualiza un paso en la simulación
	void updateStep(real deltaTime);
};

#endif
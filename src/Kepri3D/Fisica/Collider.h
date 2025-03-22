#ifndef __COLLIDER__
#define __COLLIDER__

#include "Component.h"
#include "CorePhysics.h"

class Shader;

class Collider : public Component
{
public:
	enum Shape
	{
		Esfera = 0,
		Cubo = 1
	};
public:
	// Constructora para colliders esféricos
	Collider(real radio);
	// Constructora para colliders ortoedros
	Collider(const vector3& size);
	~Collider();

	void update(float deltaTime) override {};
	void render(Shader* sh);

	// Getters
	const vector3& getHalfExtents() { return halfExtents; }
	real getRadio() { return radio; }

	// Setters
	inline void setTrigger(bool trigger) { m_trigger = trigger; }
	inline void setVisible(bool visible) { m_visible = visible; }

	// - - Estáticos - - // (Detección de colisiones)
	static bool sphereOverlap(const Collider* c1, const Collider* c2);
	static bool aabbOverlap(const Collider* c1, const Collider* c2);
	static bool sphereCubeOverlap(const Collider* sphere, const Collider* cube);

	static bool pointInCube(const vector3& point, const Collider* cube);
	static bool pointInSphere(const vector3& point, const Collider* sphere);

private:
	friend class PhysicsSystem;

	/* Forma (tipo de collider) */
	Shape shape;
	union
	{
		// Específico de esferas
		real radio;
		// Epecífico de cubos
		vector3 halfExtents;
	};

	/* ¿Es atravesable? */
	bool m_trigger;

	/* Componente renderer para depurar */
	Renderer* m_renderer;
	bool m_visible;
};

#endif
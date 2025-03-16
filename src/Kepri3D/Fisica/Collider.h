#ifndef __COLLIDER__
#define __COLLIDER__

#include "Component.h"

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
	Collider(float radio);
	// Constructora para colliders ortoedros
	Collider(const glm::dvec3& size);
	~Collider();

	void update(GLuint deltaTime) override {};
	void render(Shader* sh);

	// Setters
	inline void setTrigger(bool trigger) { m_trigger = trigger; }
	inline void setVisible(bool visible) { m_visible = visible; }
private:
	friend class PhysicsSystem;

	/* Forma (tipo de collider) */
	Shape shape;
	union
	{
		// Específico de esferas
		float radio;
		// Epecífico de cubos
		glm::dvec3 halfExtents;
	};

	/* ¿Es atravesable? */
	bool m_trigger;

	/* Componente renderer para depurar */
	Renderer* m_renderer;
	bool m_visible;
};

#endif
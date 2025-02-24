#ifndef __COLLIDER__
#define __COLLIDER__

#include "Component.h"



class Collider : public Component
{
public:
	enum Shape
	{
		Esfera = 0,
		Cubo = 1
	};
public:
	Collider(Shape forma, float radio);
	~Collider();

	void update(GLuint deltaTime) override {};
	void render();

	inline void setVisible(bool visible) { m_visible = visible; }
private:
	friend class PhysicsSystem;

	/* Forma y radio */
	Shape shape;
	float radio;

	/* Componente renderer para depurar */
	Renderer* m_renderer;
	bool m_visible;
};

#endif
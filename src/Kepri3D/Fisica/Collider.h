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
	~Collider(){}

	void update(GLuint deltaTime) override {};

private:
	Shape shape;
	float radio;
};

#endif
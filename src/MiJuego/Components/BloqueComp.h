#ifndef __BLOQUE_COMP__
#define __BLOQUE_COMP__

#include "Component.h"

class BloqueComp : public Component
{
public:
	BloqueComp(){}
	~BloqueComp(){}

	/* Ejemplo de componente que empeque�ece las entidades que se cruzan con �l */
	void onTrigger(Collider* other) override;

	void update(float deltaTime) override {}
private:
	bool entered = false;
};

#endif
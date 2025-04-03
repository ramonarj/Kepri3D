#include "Ambulancia.h"

#include "Kepri3D.h"

Ambulancia::Ambulancia()
{
	estado = 0;
}

void Ambulancia::update(float deltaTime)
{
	entity->translate({ vel * deltaTime, 0, 0 });

	if(InputManager::Instance()->getKeyDown('r'))
	{
		if (estado == 0)
			vel = 100.0f;
		else if (estado == 1)
			vel = 0.0f;
		else
			entity->setPosition({ -100, 0, 0 });

		// Transicionar
		estado = (estado + 1) % 3;
	}
}

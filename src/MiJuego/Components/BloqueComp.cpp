#include "BloqueComp.h"
#include "Kepri3D.h"


void BloqueComp::onTrigger(Collider* other)
{
	other->getEntity()->scale({ 0.999, 0.999, 0.999 });
	Renderer* rend = other->getEntity()->getComponent<Renderer>();
	if (rend != nullptr)
		rend->setActive(true);
}

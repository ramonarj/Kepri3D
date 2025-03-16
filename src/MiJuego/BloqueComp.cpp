#include "BloqueComp.h"
#include "Kepri3D.h"


void BloqueComp::onTrigger(Collider* other)
{
	other->getEntity()->getComponent<Renderer>()->setActive(true);
	other->getEntity()->scale({ 0.999, 0.999, 0.999 });
}

#include "Muelle.h"

#include "Rigid.h"

Muelle::Muelle(Rigid* r1, Rigid* r2, double k)
{
	this->r1 = r1;
	this->r2 = r2;
	this->k = k;
	longitud = glm::length(r2->getEntity()->getPosition() - r1->getEntity()->getPosition());
}

Muelle::~Muelle()
{

}
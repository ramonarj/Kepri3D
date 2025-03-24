#include "Muelle.h"

#include "Rigid.h"

Muelle::Muelle(Rigid* r1, Rigid* r2, real k)
{
	this->r1 = r1;
	this->r2 = r2;
	this->k = k;
	longitud = glm::length(r2->getEntity()->getPosition() - r1->getEntity()->getPosition());
}

void Muelle::applyForce()
{
	real elongacion = glm::length(*r2->m_position - *r1->m_position) - longitud;
	vector3 R1toR2 = glm::normalize(*r2->m_position - *r1->m_position);
	if (glm::abs(elongacion) < 0.05) { return; } //pequeño umbral para que duerma
	// Ley de Hooke
	r1->addForce(k * (R1toR2 * elongacion));
	r2->addForce(k * (-R1toR2 * elongacion));
}

Muelle::~Muelle()
{

}
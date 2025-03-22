#include "Liquido.h"
#include "Rigid.h"
#include "PhysicsSystem.h"
#include "Collider.h"

Liquido::Liquido(real altura, real densidad)
{
	m_altura = altura;
	m_densidad = densidad;
}

void Liquido::applyBuoyancy(std::vector<Rigid*> rigids) const
{
	for(Rigid* r: rigids)
	{
		if (r->m_type == Static) { continue; }
		// Altura del rigid
		real yRigid = r->getEntity()->getPosition().y;
		real halfExtY = r->m_collider->getHalfExtents().y;

		// El objeto está sumergido total/parcialmente
		if(yRigid - halfExtY < m_altura)
		{
			// Volumen del rigid (para cubos solo)
			vector3 fullExtents = r->m_collider->getHalfExtents() * 2.0;
			real volumen = fullExtents.x * fullExtents.y * fullExtents.z;

			// Proporción del rigid que está bajo la superficie {0 - 1}
			real prop = (m_altura - (yRigid - halfExtY)) / fullExtents.y;
			prop = glm::min(1.0, prop);

			// Masa y peso (=EMPUJE) del líquido desplazado
			real masaDesp = m_densidad * (volumen * prop);
			vector3 pesoDesp = masaDesp * PhysicsSystem::s_gravity;
			r->addForce(-pesoDesp);

			//depuracionEmpuje(volumen, prop, -pesoDesp);
		}
		// Si no, está totalmente fuera del líquido
	}
}

void Liquido::depuracionEmpuje(real volumen, real prop, const vector3& empuje)
{
	std::cout << "Volumen total: " << volumen << "m2, Proporcion: " << prop * 100 << "%, Empuje = ";
	PrintVector(empuje);
	std::cout << "N" << std::endl;
}
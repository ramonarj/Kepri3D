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

		// La posici�n Y m�nima del collider
		Collider* col = r->m_collider;
		real rigidBase;
		if (r->m_collider->getShape() == Collider::Cubo)
			rigidBase = r->m_position->y - col->getHalfExtents().y;
		else
			rigidBase = r->m_position->y - col->getRadio();

		// El objeto est� sumergido total/parcialmente
		if(rigidBase < m_altura)
		{
			// a) Volumen total del Collider
			real volumen = col->volume();

			// b) Proporci�n del Collider que est� bajo la superficie {0 - 1}
			real prop;
			real h = m_altura - rigidBase;
			// Cubos
			if (col->getShape() == Collider::Cubo)
			{
				prop = h / (col->getHalfExtents().y * 2.0);
				prop = glm::min(1.0, prop); //capar
			}
			// Esferas
			else
			{
				// Sumergido totalmente
				if (h >= col->getRadio() * 2) // TODO limpiar
					prop = 1;
				// Calcular el volumen del casquete esf�rico sumergido -> Vc = PI*h2/3*(3*r-h)
				else
				{
					real vCasquete = (PI * h * h / 3) * (3 * col->getRadio() - h);
					prop = vCasquete / volumen;
				}
			}

			// c) Masa y peso (=EMPUJE) del l�quido desplazado
			real masaDesp = m_densidad * (volumen * prop);
			vector3 pesoDesp = masaDesp * PhysicsSystem::s_gravity;
			// d) Aplicar la fuerza
			r->addForce(-pesoDesp);

			//depuracionEmpuje(volumen, prop, -pesoDesp);
		}
		// Si no, est� totalmente fuera del l�quido
	}
}

void Liquido::depuracionEmpuje(real volumen, real prop, const vector3& empuje) const
{
	std::cout << "Volumen total: " << volumen << "m2, Proporcion: " << prop * 100 << "%, Empuje = ";
	PrintVector(empuje);
	std::cout << "N" << std::endl;
}
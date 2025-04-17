#include "Liquido.h"

#include "Rigid.h"
#include "PhysicsSystem.h"
#include "Collider.h"
#include "ParticleSystem.h"

Liquido::Liquido(real altura, real densidad)
{
	m_altura = altura;
	m_densidad = densidad;
}

void Liquido::applyForce(Rigid* r, real deltaTime)
{
	if (r->m_type == Static) { return; }

	// La posición Y mínima del collider
	Collider* col = r->m_collider;
	real rigidBase;
	if (r->m_collider->getShape() == Collider::Cubo)
		rigidBase = r->m_position->y - col->getHalfExtents().y;
	else
		rigidBase = r->m_position->y - col->getRadio();

	// Está totalmente fuera del líquido
	if (rigidBase >= m_altura) { return; }

	// a) Volumen total del Collider
	real volumen = col->volume();

	// b) Proporción del Collider que está bajo la superficie {0 - 1}
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
		// Calcular el volumen del casquete esférico sumergido -> Vc = PI*h2/3*(3*r-h)
		else
		{
			real vCasquete = (PI * h * h / 3) * (3 * col->getRadio() - h);
			prop = vCasquete / volumen;
		}
	}

	// c) Masa y peso (=EMPUJE) del líquido desplazado
	real masaDesp = m_densidad * (volumen * prop);
	vector3 pesoDesp = masaDesp * PhysicsSystem::s_gravity;
	// d) Aplicar la fuerza
	r->addForce(-pesoDesp);

	//depuracionEmpuje(volumen, prop, -pesoDesp);
}

void Liquido::depuracionEmpuje(real volumen, real prop, const vector3& empuje) const
{
	std::cout << "Volumen total: " << volumen << "m2, Proporcion: " << prop * 100 << "%, Empuje = ";
	PrintVector(empuje);
	std::cout << "N" << std::endl;
}

void Liquido::applyForce(Particle* p, real deltaTime)
{
	// La posición Y mínima del collider
	real rigidBase = p->position->y;

	// Está totalmente fuera del líquido
	if (rigidBase >= m_altura) { return; }

	// a) Volumen total de la partícula
	real volumen = 4.19E-9; // temporal. Asumimos r=0.01 (1 cm)

	// b) Proporción del Collider que está bajo la superficie {0 - 1}
	real prop;
	real h = m_altura - rigidBase;

	// Sumergido totalmente
	if (h >= 0.01 * 2)
		prop = 1;
	// Calcular el volumen del casquete esférico sumergido -> Vc = PI*h2/3*(3*r-h)
	else
	{
		real vCasquete = (PI * h * h / 3) * (3 * 0.01 - h);
		prop = vCasquete / volumen;
	}

	// c) Masa y peso (=EMPUJE) del líquido desplazado
	real masaDesp = m_densidad * (volumen * prop);
	vector3 pesoDesp = masaDesp * PhysicsSystem::s_gravity;
	// d) Darle velocidad hacia arriba, pero poquita
	p->velocity -= pesoDesp * 200.0; // suponemos masa de 0.005kg (5 gramos)
}
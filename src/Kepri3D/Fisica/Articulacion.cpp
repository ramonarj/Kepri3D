#include "Articulacion.h"
#include "Rigid.h"
// Circular
Articulacion::Articulacion(Rigid* r1, Rigid* r2, Type tipo, real distancia)
{
	m_tipo = tipo;
	m_r1 = r1;
	m_r2 = r2;
	m_distancia = distancia;
}
//Bisagra
Articulacion::Articulacion(Rigid* r1, Rigid* r2, Type tipo, real distancia, vector3 eje)
{
	m_tipo = tipo;
	m_r1 = r1;
	m_r2 = r2;
	m_distancia = distancia;
	m_ejeBisagra = eje;
}
// Fija
Articulacion::Articulacion(Rigid* r1, Rigid* r2, Type tipo, vector3 distancia)
{
	m_tipo = tipo;
	m_r1 = r1;
	m_r2 = r2;
	m_distanciaV = distancia;
}



Articulacion::Articulacion(Rigid* r1, Rigid* r2, Type tipo) : Articulacion(r1, r2, tipo, {0, 0, 0})
{
	// Valores por defecto
	if (m_tipo == Circular)
		m_distancia = glm::length(r1->getEntity()->getPosition() - r2->getEntity()->getPosition());
	else if (m_tipo == Fija)
		m_distanciaV = r2->getEntity()->getPosition() - r1->getEntity()->getPosition();
	else if (m_tipo == Bisagra)
	{
		m_distancia = glm::length(r1->getEntity()->getPosition() - r2->getEntity()->getPosition());
		m_ejeBisagra = { 0, 1, 0 };
	}	
}

void Articulacion::applyConstraints()
{
	vector3 posR1 = *m_r1->m_position;
	vector3 posR2 = *m_r2->m_position;
	switch(m_tipo)
	{
	// Articulación fija. Ej: frontal-parietal
	case Fija:
	{
		// Teletransportamos el Rigid 2 a su posición relativa al Rigid 1
		m_r2->getEntity()->setPosition(posR1 + m_distanciaV);
		m_r2->setVelocity({ 0, 0, 0 }); // para que no vibre
		break;
	}
	// Articulación de bisagra. Ej: rodilla, codo
	case Bisagra:
	{
		vector3 R2toR1 = posR1 - posR2;
		// Capar el movimiento del Rigid 2 a sus dos ejes libres
		vector3 desp = m_ejeBisagra * (posR1 - posR2);
		m_r2->getEntity()->translate(desp);
		// Tratarla como una circular una vez puesta en el eje
		real incr = m_distancia - glm::length(R2toR1);
		if (glm::abs(incr) > 0.05)
		{
			m_r2->getEntity()->translate(-incr * glm::normalize(R2toR1));
		}
		break;
	}

	// Articulación circular. Ej: hombro-húmero, cadera-fémur
	case Circular:
	{
		vector3 R1toR2 = posR2 - posR1;
		real incr = m_distancia - glm::length(R1toR2);
		if (glm::abs(incr) > 0.05) // pequeño umbral
		{
			//m_r1->getEntity()->translate(sobrante / 2 * glm::normalize(R1toR2));
			m_r2->getEntity()->translate(-incr * glm::normalize(-R1toR2));
		}
		break;
	}

	default:
		break;
	}
	///std::cout << "Applying" << std::endl;
}
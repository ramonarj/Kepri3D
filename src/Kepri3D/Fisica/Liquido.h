#ifndef __LIQUIDO__
#define __LIQUIDO__

#include <vector>
#include "CorePhysics.h"
#include "ForceGenerator.h"

class Rigid;
class Collider;

class Liquido : public ForceGenerator
{
public:
	Liquido(real altura = 0, real densidad = 1000.0); // valores por defecto -> agua
	~Liquido(){}

	/* Aplica la flotaci�n a los Rigids dados */
	void applyForce(Rigid* r, real deltaTime) override;
	// Versi�n part�culas
	void applyForce(Particle* p, real deltaTime) override;

	/* Cambia la altura de la superficie del fluido */
	inline void setAltura(real alt) { m_altura = alt; }

	// Getters
	inline real getAltura() const { return m_altura; }
private:
	/* Kg/m3 */
	real m_densidad;
	/* La altura (Y) de la superficie del l�quido */
	real m_altura;

	// M�todos auxiliares
	void depuracionEmpuje(real volumen, real prop, const vector3& pesoDesp) const;
};

#endif
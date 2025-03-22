#ifndef __LIQUIDO__
#define __LIQUIDO__

#include <vector>
#include "CorePhysics.h"

class Rigid;

class Liquido
{
public:
	Liquido(real altura = 0, real densidad = 1000.0); // valores por defecto -> agua
	~Liquido(){}

	/* Aplica la flotación a los Rigids dados */
	void applyBuoyancy(std::vector<Rigid*> rigids) const;

	/* Cambia la altura de la superficie del fluido */
	inline void setAltura(real alt) { m_altura = alt; }

	// Getters
	inline real getAltura() const { return m_altura; }
private:
	/* Kg/m3 */
	real m_densidad;
	/* La altura (Y) de la superficie del líquido */
	real m_altura;

	// Métodos auxiliares
	void depuracionEmpuje(real volumen, real prop, const vector3& pesoDesp);
};

#endif
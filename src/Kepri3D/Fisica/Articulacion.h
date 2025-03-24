#ifndef __ARTICULACION__
#define __ARTICULACION__

#include "CorePhysics.h"

class Rigid;

// Clase bastante similar a "Muelle"
class Articulacion
{
public:
	enum Type
	{
		Fija,
		Bisagra,
		Circular
	};

	/* Articulaciones circulares */
	Articulacion(Rigid* r1, Rigid* r2, Type tipo, real distancia);
	/* Articulaciones fijas */
	Articulacion(Rigid* r1, Rigid* r2, Type tipo, vector3 distancia);
	/* Articulaciones de bisagra */
	Articulacion(Rigid* r1, Rigid* r2, Type tipo, real distancia, vector3 eje);

	// Predet.
	Articulacion(Rigid* r1, Rigid* r2, Type tipo);
	~Articulacion(){}

	/* Se encarga de mantener la unión */
	void applyConstraints();
private:
	Rigid* m_r1;
	Rigid* m_r2;
	Type m_tipo;
	union
	{
		// Fijas
		vector3 m_distanciaV;
		// Circulares y bisagra
		real m_distancia;
	};
	// Bisagra
	vector3 m_ejeBisagra;
};
#endif
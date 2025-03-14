#ifndef __MUELLE__
#define __MUELLE__

class Rigid;

class Muelle
{
public:
	Muelle(Rigid* r1, Rigid* r2, double k = 10.0);
	~Muelle();
private:
	friend class PhysicsSystem;
	/* Rigidos conectados por el muelle */
	Rigid* r1;
	Rigid* r2;
	/* Longitud del muelle en reposo, cuando no actúan fuerzas sobre él */
	float longitud;
	/* Constante elástica (específica del muelle) [N/m] */
	double k;
};

#endif
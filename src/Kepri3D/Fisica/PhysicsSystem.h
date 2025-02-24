#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include <vector>

class Rigid;

/* Clase singleton para gestionar el sistema de f�sicas */
class PhysicsSystem
{
public:
	static PhysicsSystem* Instance()
	{
		if (s_instance == nullptr)
			s_instance = new PhysicsSystem();
		return s_instance;
	}

	/* Limpia la instancia; debe llamarse expl�citamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

	/* A�ade un s�lido r�gido a la simulaci�n */
	void addRigid(Rigid* r);

	void update();

private:
	static PhysicsSystem* s_instance;
	PhysicsSystem() {}

	/* Vector de s�lidos r�gidos*/
	std::vector<Rigid*> m_rigids;

	// M�todos privados
	bool checkCollision(Rigid* r1, Rigid* r2);
	void solveCollision(Rigid* r1, Rigid* r2);
};

#endif
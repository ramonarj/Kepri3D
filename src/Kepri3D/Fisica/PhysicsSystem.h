#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include <vector>

class Rigid;

/* Clase singleton para gestionar el sistema de físicas */
class PhysicsSystem
{
public:
	static PhysicsSystem* Instance()
	{
		if (s_instance == nullptr)
			s_instance = new PhysicsSystem();
		return s_instance;
	}

	/* Limpia la instancia; debe llamarse explícitamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

	/* Añade un sólido rígido a la simulación */
	void addRigid(Rigid* r);

	void update();

private:
	static PhysicsSystem* s_instance;
	PhysicsSystem() {}

	/* Vector de sólidos rígidos*/
	std::vector<Rigid*> m_rigids;

	// Métodos privados
	bool checkCollision(Rigid* r1, Rigid* r2);
	void solveCollision(Rigid* r1, Rigid* r2);
};

#endif
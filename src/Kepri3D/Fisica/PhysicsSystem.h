#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include <vector>
#include <glew.h>
#include "Game.h"

class Collider;
class Rigid;
class Muelle;

// Indica c�mo de el�stico es un choque. Por ahora, es compartido para todos los choques.
// 0 = totalmente inel�stico (se quedan pegados), 1 = totalmente el�stico (no se pierde Ec)
const float COEF_RESTITUCION = 0.6f;

// N�mero m�ximo de iteraciones para resolver una colisi�n
const unsigned int MAX_ITER = 10;

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

	/* A�ade un muelle a la simulaci�n */
	void addMuelle(Muelle* m);

	void update(GLuint deltaTime);

#ifdef __DEBUG_INFO__
	glm::vec3 momentoTotal = { 0, 0, 0 };
#endif

private:
	static PhysicsSystem* s_instance;
	PhysicsSystem() {}

	/* Vector de s�lidos r�gidos*/
	std::vector<Rigid*> m_rigids;

	/* Vector de muelles */
	std::vector<Muelle*> m_muelles;

	/* Tiempo entre el anterior frame y este */
	GLuint m_deltaTime;

	// M�todos privados
	bool checkCollision(Collider* r1, Collider* r2);
	void solveCollision(Rigid* r1, Rigid* r2);
	void notifyCollision(Collider* c1, Collider* c2);
};

#endif
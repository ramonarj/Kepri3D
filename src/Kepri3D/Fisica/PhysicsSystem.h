#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include <vector>
#include <glew.h>

class Collider;
class Rigid;

// Indica cómo de elástico es un choque. Por ahora, es compartido para todos los choques.
// 0 = totalmente inelástico (se quedan pegados), 1 = totalmente elástico (no se pierde Ec)
const float COEF_RESTITUCION = 0.6f;

// Número máximo de iteraciones para resolver una colisión
const unsigned int MAX_ITER = 10;

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

	void update(GLuint deltaTime);

private:
	static PhysicsSystem* s_instance;
	PhysicsSystem() {}

	/* Vector de sólidos rígidos*/
	std::vector<Rigid*> m_rigids;

	GLuint m_deltaTime;

	// Métodos privados
	bool checkCollision(Collider* r1, Collider* r2);
	void solveCollision(Rigid* r1, Rigid* r2);
};

#endif
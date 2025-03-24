#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include <vector>
#include <glew.h>
#include "Game.h"
#include "CorePhysics.h"

class Collider;
class Rigid;
class Muelle;
class Liquido;
class Articulacion;

// Indica cómo de elástico es un choque. Por ahora, es compartido para todos los choques.
// 0 = totalmente inelástico (se quedan pegados), 1 = totalmente elástico (no se pierde Ec)
const real COEF_RESTITUCION = 0.6f;

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
	void Clean();

	/* Gravedad global */
	static vector3 s_gravity;

	/* Añade un sólido rígido a la simulación */
	void addRigid(Rigid* r);

	/* Añade un muelle a la simulación */
	void addMuelle(Muelle* m);

	/* Añade un líquido a la simulación */
	void addLiquido(Liquido* l);

	/* Añade una articulación a la simulación */
	void addArticulacion(Articulacion* a);

	/* Lanza un rayo en la dirección dada y devuelve true si el rayo golpea un collider, false e.o.c. */
	bool raycast(const vector3& origen, const vector3& dir, real dist);

	/* Lanza un rayo desde la posición de la pantalla dada (la dirección dependerá del tipo de cámara) */
	bool raycastFromScreen(vector2 origen, real dist);

	void update(float deltaTime);

#ifdef __DEBUG_INFO__
	glm::vec3 momentoTotal = { 0, 0, 0 };
	unsigned int rigidsDespiertos = 0;
#endif

private:
	static PhysicsSystem* s_instance;
	PhysicsSystem() {}

	/* Vector de sólidos rígidos*/
	std::vector<Rigid*> m_rigids;

	/* Vector de muelles */
	std::vector<Muelle*> m_muelles;

	/* Vector del líquidos */
	std::vector<Liquido*> m_liquidos;

	/* Vector de articulaciones */
	std::vector<Articulacion*> m_articulaciones;

	/* Tiempo entre el anterior frame y este */
	real m_deltaTime;

	// Métodos privados
	bool checkOverlap(Collider* r1, Collider* r2);
	void solveCollision(Rigid* r1, Rigid* r2);
	// Envío de mensajes a los demás componentes
	void notifyCollision(Collider* c1, Collider* c2);
	void notifyTrigger(Collider* c1, Collider* c2);
	// Cálculo de las velocidades resultantes tras una colisión elástica
	std::pair<vector3, vector3> calculateElasticCollision(const vector3& v1, const vector3& v2,
		const vector3& x1, const vector3& x2, real m1, real m2);
	std::pair<vector3, vector3> calculateElasticCollision(const vector3& v1, const vector3& v2,
		const vector3& X1toX2, real m1, real m2);
};

#endif
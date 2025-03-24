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

// Indica c�mo de el�stico es un choque. Por ahora, es compartido para todos los choques.
// 0 = totalmente inel�stico (se quedan pegados), 1 = totalmente el�stico (no se pierde Ec)
const real COEF_RESTITUCION = 0.6f;

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
	void Clean();

	/* Gravedad global */
	static vector3 s_gravity;

	/* A�ade un s�lido r�gido a la simulaci�n */
	void addRigid(Rigid* r);

	/* A�ade un muelle a la simulaci�n */
	void addMuelle(Muelle* m);

	/* A�ade un l�quido a la simulaci�n */
	void addLiquido(Liquido* l);

	/* A�ade una articulaci�n a la simulaci�n */
	void addArticulacion(Articulacion* a);

	/* Lanza un rayo en la direcci�n dada y devuelve true si el rayo golpea un collider, false e.o.c. */
	bool raycast(const vector3& origen, const vector3& dir, real dist);

	/* Lanza un rayo desde la posici�n de la pantalla dada (la direcci�n depender� del tipo de c�mara) */
	bool raycastFromScreen(vector2 origen, real dist);

	void update(float deltaTime);

#ifdef __DEBUG_INFO__
	glm::vec3 momentoTotal = { 0, 0, 0 };
	unsigned int rigidsDespiertos = 0;
#endif

private:
	static PhysicsSystem* s_instance;
	PhysicsSystem() {}

	/* Vector de s�lidos r�gidos*/
	std::vector<Rigid*> m_rigids;

	/* Vector de muelles */
	std::vector<Muelle*> m_muelles;

	/* Vector del l�quidos */
	std::vector<Liquido*> m_liquidos;

	/* Vector de articulaciones */
	std::vector<Articulacion*> m_articulaciones;

	/* Tiempo entre el anterior frame y este */
	real m_deltaTime;

	// M�todos privados
	bool checkOverlap(Collider* r1, Collider* r2);
	void solveCollision(Rigid* r1, Rigid* r2);
	// Env�o de mensajes a los dem�s componentes
	void notifyCollision(Collider* c1, Collider* c2);
	void notifyTrigger(Collider* c1, Collider* c2);
	// C�lculo de las velocidades resultantes tras una colisi�n el�stica
	std::pair<vector3, vector3> calculateElasticCollision(const vector3& v1, const vector3& v2,
		const vector3& x1, const vector3& x2, real m1, real m2);
	std::pair<vector3, vector3> calculateElasticCollision(const vector3& v1, const vector3& v2,
		const vector3& X1toX2, real m1, real m2);
};

#endif
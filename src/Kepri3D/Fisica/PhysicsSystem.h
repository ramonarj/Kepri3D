#ifndef __PHYSICS_SYSTEM__
#define __PHYSICS_SYSTEM__

#include <vector>
#include <glew.h>
#include "Defines.h"
#include "CorePhysics.h"

class Collider;
class Rigid;
class Muelle;
class Articulacion;
class ForceGenerator;

// Indica c�mo de el�stico es un choque. Por ahora, es compartido para todos los choques.
// 0 = totalmente inel�stico (se quedan pegados), 1 = totalmente el�stico (no se pierde Ec)
const real COEF_RESTITUCION = 1.0f;

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

	void update(float deltaTime);

	/* Gravedad global */
	static vector3 s_gravity;

	// A�adir entidades f�sicas a la simulaci�n
	void addRigid(Rigid* r);
	void addForceGenerator(ForceGenerator* fg);
	void addMuelle(Muelle* m);
	void addArticulacion(Articulacion* a);

	/* Lanza un rayo en la direcci�n dada y devuelve true si el rayo golpea un collider, false e.o.c. */
	bool raycast(const vector3& origen, const vector3& dir, real dist);
	bool raycast(const vector3& origen, const vector3& dir, real dist, vector3& impactPoint);
	Collider* raycastPro(const vector3& origen, const vector3& dir, real dist);

	/* Lanza un rayo desde la posici�n de la pantalla dada (la direcci�n depender� del tipo de c�mara) */
	bool raycastFromScreen(vector2 origen, real dist);
	Collider* raycastFromScreenPro(vector2 origen, real dist);

	// Fixed Time
	inline real getFixedTime() const { return m_fixedTime; }
	void setFixedTime(real t);

#ifdef __DEBUG_INFO__
	glm::vec3 momentoTotal = { 0, 0, 0 };
	unsigned int rigidsDespiertos = 0;
#endif

private:
	struct Colision
	{
		Colision(Rigid* r1, Rigid* r2, vector3 n)
		{
			this->r1 = r1;
			this->r2 = r2;
			this->n = n;
		}
		// Cuerpos implicados
		Rigid* r1;
		Rigid* r2;
		// Normal de la colisi�n, vista de R1 a R2
		vector3 n;

		// M�todos auxiliares
		real separatingVelocity() const;
		void solveInterpenetration(real time);
		void solveVelocity();
		void solveVelocityAntiguo();
	};

	static PhysicsSystem* s_instance;
	PhysicsSystem() : accumTime(0) {}

	/* Vectores de las entidades f�sicas */
	std::vector<Rigid*> m_rigids;
	std::vector<Muelle*> m_muelles;
	std::vector<ForceGenerator*> m_generators;
	std::vector<Articulacion*> m_articulaciones;

	/* Lista de colisiones por resolver */
	std::vector<Colision> m_colisiones;

	/* Tiempo entre el anterior frame y este */
	real m_deltaTime;

	/* Tiempo fijo que transcurre entre cada paso de la simulaci�n */
	real accumTime;
	real m_fixedTime = 0.02; // 50 veces/segundo

	/* M�todos privados */ 
	// Colisiones
	void simulateStep(real delta);
	void createCollision(Rigid* r1, Rigid* r2);
	// Env�o de mensajes a los dem�s componentes
	void notifyCollision(Colision* c);
	void notifyTrigger(Collider* c1, Collider* c2);
	// C�lculo de las velocidades resultantes tras una colisi�n el�stica
	std::pair<vector3, vector3> calculateElasticCollision(const vector3& v1, const vector3& v2,
		const vector3& x1, const vector3& x2, real m1, real m2);
	std::pair<vector3, vector3> calculateElasticCollision(const vector3& v1, const vector3& v2,
		const vector3& X1toX2, real m1, real m2);
};

#endif
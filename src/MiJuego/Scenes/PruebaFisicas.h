#ifndef __FISICAS_SCENE__
#define __FISICAS_SCENE__

#include "../Scene.h"

#include "Rigid.h"

class PruebaFisicas : public Scene
{
public:
	PruebaFisicas() { m_name = "PruebaFisicas"; }
	~PruebaFisicas() {}

	void loadResources() override;
	void init() override;

private:
	Rigid* createRigidCubo(const std::string& name, vector3 pos, RigidType type = Dynamic, double masa = 1.0, 
		bool useGravity = true, double tamLado = 2.0, vector3 tamCollider = { 2, 2, 2 });
	Rigid* createRigidEsfera(const std::string& name, vector3 pos, RigidType type = Dynamic, double masa = 1.0, 
		bool useGravity = true, double radio = 1, real radioCollider = 1);
	void createDragon(Rigid* nodo, int numEslabones, int tipoUnion);
};

#endif
#ifndef __SANDBOX_MANAGER__
#define __SANDBOX_MANAGER__

#include "Component.h"

class Entity;

class SandboxManager : public Component
{
public:
	SandboxManager(Entity* suelo);
	~SandboxManager(){}

	void update(float deltaTime) override;
private:
	Entity* suelo;
	float tilingVel = 5.0f;

	void changeMaterial(const std::string& name);
};

#endif
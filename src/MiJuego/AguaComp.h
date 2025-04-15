#ifndef __AGUA_COMP__
#define __AGUA_COMP__

#include "Component.h"

class Material;
class Skybox;

// Componente muy sencillo que envía un valor al Material
class AguaComp : public Component
{
public:
	AguaComp(Material* mat);
	~AguaComp() {}

	void start() override;
	void update(float deltaTime) override;

	void setSpeeds(glm::vec2 texSpeed, glm::vec2 dispSpeed);

private:
	Material* mat;
	Skybox* cielo;
};

#endif

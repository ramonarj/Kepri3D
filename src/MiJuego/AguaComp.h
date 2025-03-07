#ifndef __AGUA_COMP__
#define __AGUA_COMP__

#include "Component.h"

class Material;

// Componente muy sencillo que envía un valor al Material
class AguaComp : public Component
{
public:
	AguaComp(Material* mat);
	~AguaComp() {}

	void update(GLuint deltaTime);

	void setSpeeds(glm::vec2 texSpeed, glm::vec2 dispSpeed);

private:
	Material* mat;
};

#endif

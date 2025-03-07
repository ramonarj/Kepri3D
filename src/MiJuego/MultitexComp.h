#ifndef __MULTITEXCOMP__
#define __MULTITEXCOMP__

#include "Component.h"

class Material;

// Componente muy sencillo que envía un valor al Material
class MultitexComp : public Component
{
public:
	MultitexComp(Material* mat) { this->mat = mat; }
	~MultitexComp() {}

	void update(GLuint deltaTime);

private:
	Material* mat;
};

#endif

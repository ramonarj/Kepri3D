#ifndef __PARALLAX_COMP__
#define __PARALLAX_COMP__

#include "Component.h"

class Material;

class ParallaxComp : public Component
{
public:
	ParallaxComp(Material* mat) { this->parallaxMat = mat; }
	~ParallaxComp() {}

	void update(float deltaTime) override;

private:
	Material* parallaxMat;
	float heightScale = 0.1f;
	int parallaxType = 1;
};

#endif
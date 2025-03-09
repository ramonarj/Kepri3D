#include "ParallaxComp.h"

#include "Kepri3D.h"

void ParallaxComp::update(GLuint deltaTime)
{
	// Aumentar/disminuir el efecto
	if (InputManager::Instance()->getKey('9'))
	{
		heightScale += ((float)deltaTime / 1000.0f) * 0.1f;
		parallaxMat->setFloat("height_scale", heightScale);
	}
	if (InputManager::Instance()->getKey('8'))
	{
		heightScale -= ((float)deltaTime / 1000.0f) * 0.1f;
		if (heightScale < 0) { heightScale = 0; }
		parallaxMat->setFloat("height_scale", heightScale);
	}
	
	// Cambiar el tipo de paralaje
	if (InputManager::Instance()->getKeyDown(9)) // Tabulador
	{
		parallaxType = (parallaxType + 1) % 2;
		parallaxMat->setInt("parallaxType", parallaxType);
	}
}

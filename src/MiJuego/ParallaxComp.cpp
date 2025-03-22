#include "ParallaxComp.h"

#include "Kepri3D.h"

void ParallaxComp::update(float deltaTime)
{
	// Aumentar/disminuir el efecto
	if (InputManager::Instance()->getKey('8'))
	{
		heightScale += deltaTime * 0.1f;
		parallaxMat->setFloat("height_scale", heightScale);
	}
	if (InputManager::Instance()->getKey('7'))
	{
		heightScale -= deltaTime * 0.1f;
		if (heightScale < 0) { heightScale = 0; }
		parallaxMat->setFloat("height_scale", heightScale);
	}
	
	// Cambiar el tipo de paralaje
	if (InputManager::Instance()->getKeyDown(9)) // Tabulador
	{
		parallaxType = (parallaxType + 1) % 3;
		parallaxMat->setInt("parallaxType", parallaxType);
	}
}

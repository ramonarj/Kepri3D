#include "RotationComp.h"

#include "Kepri3D.h"

void RotationComp::update(GLuint deltaTime)
{
	if (InputManager::Instance()->getKeyDown('x'))
	{
		rotAxis = { 1, 0, 0 };
	}
	if (InputManager::Instance()->getKeyDown('y'))
	{
		rotAxis = { 0, 1, 0 };
	}
	if (InputManager::Instance()->getKeyDown('z'))
	{
		rotAxis = { 0, 0, 1 };
	}
	entity->rotate(deltaTime / 1000.0 * rotSpeed, rotAxis, LOCAL);
}
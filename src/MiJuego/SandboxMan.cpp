#include "SandboxMan.h"

#include "Kepri3D.h"

SandboxManager::SandboxManager(Entity* suelo)
{
	this->suelo = suelo;
}

void SandboxManager::update(float deltaTime)
{
	// Cambiar el material del suelo
	if (InputManager::Instance()->getKeyDown('1'))
		changeMaterial("cesped");
	else if (InputManager::Instance()->getKeyDown('2'))
		changeMaterial("tierra");
	else if (InputManager::Instance()->getKeyDown('3'))
		changeMaterial("nieve");

	// Cambiar el tiling
	glm::vec2 tiling = suelo->getMaterial()->getVec2("tiling");
	// eje X
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
		tiling.x += deltaTime * tilingVel;
	else if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
		tiling.x -= deltaTime * tilingVel;
	// eje Y
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
		tiling.y -= deltaTime * tilingVel;
	else if (InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
		tiling.y += deltaTime * tilingVel;
	suelo->getMaterial()->setVec2("tiling", tiling);
}

void SandboxManager::changeMaterial(const std::string& name)
{
	// Cada entidad tiene una copia del material
	glm::vec2 tiling = suelo->getMaterial()->getVec2("tiling");
	suelo->setMaterial(name);
	suelo->getMaterial()->setVec2("tiling", tiling);
}
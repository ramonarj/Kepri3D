#include "AguaComp.h"

#include "Kepri3D.h"

AguaComp::AguaComp(Material* mat)
{
	this->mat = mat;

	// Valores por defecto
	setSpeeds({ 2, 0 }, { -1, -1 });
}

void AguaComp::update(GLuint deltaTime)
{
	// Pasarle el tiempo y velocidad de desplazamiento al material
	float t = glutGet(GLUT_ELAPSED_TIME);
	mat->setFloat("tiempo", t / 10000.0f);
}

void AguaComp::setSpeeds(glm::vec2 texSpeed, glm::vec2 dispSpeed)
{
	mat->setVec2("velTex", texSpeed);
	// Útil para simular la dirección y fuerza del viento en la superficie del agua
	mat->setVec2("velDisp", dispSpeed);
}

#include "MultitexComp.h"

#include "Kepri3D.h"

void MultitexComp::update(GLuint deltaTime)
{
	mat->setFloat("mix", (sin(glutGet(GLUT_ELAPSED_TIME) * 0.001) + 1) / 2.0f);
}

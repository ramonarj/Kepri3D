#include "Callbacks.h"

#include "Kepri3D.h"
#include "GameManager.h"


void cullingButtonPressed()
{
	// Activar / desactivar el culling de polígonos traseros
	Game::switchBoolParam(GL_CULL_FACE);
	GameManager::centerMouse();
}

void blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	Game::switchBoolParam(GL_BLEND);
	GameManager::centerMouse();
}

void lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	Game::switchBoolParam(GL_LIGHTING);
	GameManager::centerMouse();
}

void texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	Game::switchBoolParam(GL_TEXTURE_2D);
	GameManager::centerMouse();
}

void shadingButtonPressed()
{
	// Cambiar entre sombreado FLAT y SMOOTH
	GLint shadeType;
	glGetIntegerv(GL_SHADE_MODEL, &shadeType);
	if (shadeType == GL_FLAT)
	{
		glShadeModel(GL_SMOOTH);
		Material::setShadingType(GL_SMOOTH);
	}

	else
	{
		glShadeModel(GL_FLAT);
		Material::setShadingType(GL_FLAT);
	}

	GameManager::centerMouse();
}

void alphaButtonPressed()
{
	// Activar / desactivar el alpha test
	Game::switchBoolParam(GL_ALPHA_TEST);
	GameManager::centerMouse();
}

void multisamplingButtonPressed()
{
	// Activar / desactivar el multisampling
	Game::switchBoolParam(GL_MULTISAMPLE);
	GameManager::centerMouse();
}

void mipmapButtonPressed()
{
	// Activar / desactivar el uso de mipmaps
	Scene::mipmapsActive = !Scene::mipmapsActive;
	ResourceManager::Instance()->enableMipmaps(Scene::mipmapsActive);

	GameManager::centerMouse();
}

void normalsButtonPressed()
{
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (Scene::normalsShader == nullptr)
		Scene::normalsShader = (Shader*)&ResourceManager::Instance()->getShader("normals");
	else
		Scene::normalsShader = nullptr;

	GameManager::centerMouse();
}

void compositeButtonPressed()
{
	// Activar / desactivar los efectos composite
	Scene::compositesActive = !Scene::compositesActive;

	GameManager::centerMouse();
}


void scissorButtonPressed()
{
	int proporcionBarra = glutGet(GLUT_WINDOW_HEIGHT) / 6.0f;
	// Actualizar el área visible
	Game::updateScissorBox(0, proporcionBarra, glutGet(GLUT_WINDOW_WIDTH),
		glutGet(GLUT_WINDOW_HEIGHT) - proporcionBarra * 2);
	// Activar / desactivar el Scissor Test (hay que hacerlo en este orden)
	Game::switchBoolParam(GL_SCISSOR_TEST);

	GameManager::centerMouse();
}

void skyboxButtonPressed()
{
	Scene::skyboxActive = !Scene::skyboxActive;
	GameManager::centerMouse();
}

void gammaButtonPressed()
{
	Game::switchBoolParam(GL_FRAMEBUFFER_SRGB);
	GameManager::centerMouse();
}

void stencilButtonPressed()
{
	Game::switchBoolParam(GL_STENCIL_TEST);
	GameManager::centerMouse();
}

void logicOpButtonPressed()
{
	// Activar las operaciones lógicas implica desactivar totalmente el 'Blending'
	Game::switchBoolParam(GL_COLOR_LOGIC_OP);

	// Tipo de operación que se hace sobre cada píxel
	// Curiosas : GL_COPY (predet.), GL_COPY_INVERTED (negativo), GL_INVERT (b/n)
	glLogicOp(GL_COPY_INVERTED);

	GameManager::centerMouse();
}


void instancingButtonPressed()
{
	if (GameManager::particleSys != nullptr)
	{
		GameManager::particleSys->setActive(!GameManager::particleSys->isActive());
	}
	GameManager::centerMouse();
}

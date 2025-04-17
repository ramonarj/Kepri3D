#include "Callbacks.h"

#include "Kepri3D.h"
#include "GameManager.h"


void cullingButtonPressed()
{
	// Activar / desactivar el culling de polígonos traseros
	Game::switchBoolParam(GL_CULL_FACE);
}

void blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	Game::switchBoolParam(GL_BLEND);
}

void lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	Game::switchBoolParam(GL_LIGHTING);
}

void texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	// s/shaders
	Game::switchBoolParam(GL_TEXTURE_2D);
	// c/shaders
	Material::s_useTextures = !Material::s_useTextures;
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
}

void alphaButtonPressed()
{
	// Activar / desactivar el alpha test
	Game::switchBoolParam(GL_ALPHA_TEST);
}

void multisamplingButtonPressed()
{
	// Activar / desactivar el multisampling
	Game::switchBoolParam(GL_MULTISAMPLE);
	Scene::msaa_active = !Scene::msaa_active;
}

void mipmapButtonPressed()
{
	// Activar / desactivar el uso de mipmaps
	Scene::mipmapsActive = !Scene::mipmapsActive;
	ResourceManager::Instance()->enableMipmaps(Scene::mipmapsActive);
}

void normalsButtonPressed()
{
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (Scene::normalsShader == nullptr)
	{
		Scene::normalsShader = ResourceManager::Instance()->getShader("normals");
		Scene::normalsShader->useTextures(false);
	}
	else
		Scene::normalsShader = nullptr;
}

void compositeButtonPressed()
{
	// Activar / desactivar los efectos composite
	Scene::compositesActive = !Scene::compositesActive;
}


void scissorButtonPressed()
{
	int proporcionBarra = glutGet(GLUT_WINDOW_HEIGHT) / 6.0f;
	// Actualizar el área visible
	Game::updateScissorBox(0, proporcionBarra, glutGet(GLUT_WINDOW_WIDTH),
		glutGet(GLUT_WINDOW_HEIGHT) - proporcionBarra * 2);
	// Activar / desactivar el Scissor Test (hay que hacerlo en este orden)
	Game::switchBoolParam(GL_SCISSOR_TEST);
}

void skyboxButtonPressed()
{
	Scene::skyboxActive = !Scene::skyboxActive;
}

void gammaButtonPressed()
{
	Game::switchBoolParam(GL_FRAMEBUFFER_SRGB);
}

void fresnelButtonPressed()
{
	Material::fresnel = !Material::fresnel;
}


void stencilButtonPressed()
{
	Game::switchBoolParam(GL_STENCIL_TEST);
}

void shadowsButtonPressed()
{
	Game::Instance()->getScene()->toggleShadows();
}

void logicOpButtonPressed()
{
	// Activar las operaciones lógicas implica desactivar totalmente el 'Blending'
	Game::switchBoolParam(GL_COLOR_LOGIC_OP);

	// Tipo de operación que se hace sobre cada píxel
	// Curiosas : GL_COPY (predet.), GL_COPY_INVERTED (negativo), GL_INVERT (b/n)
	glLogicOp(GL_COPY_INVERTED);
}


void instancingButtonPressed()
{
	//if (GameManager::particleSys != nullptr)
	//{
	//	GameManager::particleSys->setActive(!GameManager::particleSys->isActive());
	//}
}

void volumesButtonPressed()
{
	Renderer::visibleBounds = !Renderer::visibleBounds;
}

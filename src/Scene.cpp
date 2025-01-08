#include "Scene.h"

#include "Mesh.h"
#include "Texture.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "UI/Canvas.h"


//#include <glew.h>
#include <freeglut.h>

bool Scene::skyboxActive = true;
bool Scene::mipmapsActive = false;
Shader* Scene::normalsShader = nullptr;
Shader* Scene::compositeShader = nullptr;

void Scene::AddEntity(Entity* e, bool isTranslucid)
{
	//if (!isTranslucid)
	//	m_opaqueEntities.push_back(e);
	//else
	//	m_translucentEntities.push_back(e);
	m_entities.push_back(e);
}

void Scene::init()
{

}

void Scene::render()
{
	// 0) Limpiar el color y depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1) Cargar las luces; IMPORTANTE hacerlo antes de pintar los objetos a los que puedan iluminar
	loadLights();

	// 2) Pintar el skybox, si lo hay
	const glm::dmat4 projViewMat = m_camera->getProjMat() * m_camera->getViewMat();
	renderSkybox(projViewMat);

	// 3) Pintar todas las entidades activas
	renderEntities(projViewMat);

	// 4) Pintar los vectores normales, si están activos
	renderNormals(projViewMat);

	// 5) Pintar el canvas
	renderCanvas();

	// 6) Post-procesar la imagen del color buffer
	renderEffects();

	//ViewportTest();

	// 7) Hacer swap de buffers
	// Hay 2 buffers; uno se está mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void Scene::loadLights()
{
	for (Light* l : m_lights)
	{
		if (l->isActive())
		{
			l->load(m_camera->getViewMat());
		}
	}
}

void Scene::renderSkybox(const glm::dmat4& projViewMat)
{
	// Comprobar que haya un skybox activo
	if (skyboxActive && m_skybox != nullptr)
	{
		// Debería haber una mejor forma de hacer esto
		m_skybox->setPosition(m_camera->getPosition());

		// Activar el shader y pasarle la MVP
		glUseProgram(m_skybox->getShader()->getId());
		int mvpMatLoc = glGetUniformLocation(m_skybox->getShader()->getId(), "mvpMat");
		glUniformMatrix4dv(mvpMatLoc, 1, GL_FALSE, glm::value_ptr(projViewMat * m_skybox->getModelMat()));

		// Pintar el skybox
		m_skybox->render();
	}
}

void Scene::renderEntities(const glm::dmat4& projViewMat)
{
	// Pintar todas las entidades activas
	const Shader* activeShader = nullptr;
	for (Entity* e : m_entities)
	{
		if (e->isActive())
		{
			// la entidad no usa shaders; desactivamos
			if (e->getShader() == nullptr)
				glUseProgram(0);
			// la entidad usa shaders
			else
			{
				int mvpMatLoc;
				// hay que cambiar el shader que usó la entidad anterior
				if (e->getShader() != activeShader)
				{
					activeShader = e->getShader();
					mvpMatLoc = glGetUniformLocation(activeShader->getId(), "mvpMat");
					glUseProgram(activeShader->getId());
				}
				// pasar la matriz MVP al vertex shader
				glUniformMatrix4dv(mvpMatLoc, 1, GL_FALSE, glm::value_ptr(projViewMat * e->getModelMat()));
			}
			// en cualquier caso, mandamos la info de los vértices
			e->render(m_camera->getViewMat());
		}
	}
}

void Scene::renderNormals(const glm::dmat4& projViewMat)
{
	if (normalsShader != nullptr)
	{
		glUseProgram(normalsShader->getId());
		int mvpMatLoc = glGetUniformLocation(normalsShader->getId(), "mvpMat");
		for (Entity* e : m_entities)
		{
			// Pasar la matriz MVP al vertex shader
			glUniformMatrix4dv(mvpMatLoc, 1, GL_FALSE, glm::value_ptr(projViewMat * e->getModelMat()));
			e->render(m_camera->getViewMat());
		}
	}
}

void Scene::renderCanvas()
{
	if (m_canvas == nullptr)
		return;

	// Desactivamos cualquier shader que hubiera
	glUseProgram(0);

	//glClear(GL_DEPTH_BUFFER_BIT); // esto es más costoso que cambiar la función del Z-test
	// Cambiar la función del Z-test para pasarlo siempre
	glDepthFunc(GL_ALWAYS);
	// Pintar el canvas
	m_canvas->render(m_camera->getViewMat());

	glDepthFunc(GL_LESS); // valor predet.
}

void Scene::renderEffects()
{
	if (compositeShader != nullptr)
	{
		// 'glClear' es una operación costosa, podría buscarse una alternativa
		//glClear(GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_ALWAYS);
		glUseProgram(compositeShader->getId());

		// Crear textura con los píxeles del color buffer (tiene que ser GL_BACK para que funcione)
		Texture t;
		t.loadRTT(m_camera->getVP()->getW(), m_camera->getVP()->getH(), GL_BACK);

		// "Re"dibujar la escena usando el shader
		m_effectsMesh->draw();

		glDepthFunc(GL_LESS);
	}
}

void Scene::update(GLuint deltaTime)
{
	// Actualizar el canvas. Importante hacerlo antes que el resto de entidades
	if(m_canvas != nullptr)
		m_canvas->update(deltaTime);

	// Actualizar las entidades
	for (Entity* e : m_entities)
	{
		if(e->isActive())
		{
			e->update(deltaTime);
		}
	}

	// Limpiar el input para el siguiente frame
	InputManager::Instance()->Update();
}

void Scene::takePhoto()
{
	// Leemos la información del front buffer (el que está en pantalla), porque 
	// el trasero podría estar dibujándose y tendríamos 'tearing'0
	Texture::loadColorBuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), GL_BACK);
	// Sin embargo, con GL_BACK afecta menos al rendimiento (framerate)

	Texture::save("foto.bmp");

	std::cout << "Se ha hecho una foto" << std::endl;
}

void Scene::switchBoolParam(GLenum param)
{
	GLboolean value;
	glGetBooleanv(param, &value);
	if (value)
		glDisable(param);
	else
		glEnable(param);
}

void Scene::cullingButtonPressed()
{
	// Activar / desactivar el culling de polígonos traseros
	switchBoolParam(GL_CULL_FACE);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	switchBoolParam(GL_BLEND);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	switchBoolParam(GL_LIGHTING);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	switchBoolParam(GL_TEXTURE_2D);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::shadingButtonPressed()
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

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::alphaButtonPressed()
{
	// Activar / desactivar el alpha test
	switchBoolParam(GL_ALPHA_TEST);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::multisamplingButtonPressed()
{
	// Activar / desactivar el multisampling
	switchBoolParam(GL_MULTISAMPLE);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::mipmapButtonPressed()
{
	// Activar / desactivar el uso de mipmaps
	mipmapsActive = !mipmapsActive;
	ResourceManager::Instance()->enableMipmaps(mipmapsActive);


	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::normalsButtonPressed()
{
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (normalsShader == nullptr)
		normalsShader = (Shader*)&ResourceManager::Instance()->getShader("normals");
	else
		normalsShader = nullptr;

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::compositeButtonPressed()
{
	// Activar / desactivar la visualización de vectores normales a cada vértice
	if (compositeShader == nullptr)
		compositeShader = (Shader*)&ResourceManager::Instance()->getComposite("waves");
	else
		compositeShader = nullptr;

	InputManager::Instance()->setMousePos(400, 300);
}


void Scene::scissorButtonPressed()
{
	// Bandas cinemáticas negras. Como tenemos 2 buffers, hay que ponerlos los 2 a negro antes de
	// activar el scissor test (para que los bordes no se queden con imágenes residuales y parpadeen)
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	// Segundo buffer
	if(glutGet(GLUT_WINDOW_DOUBLEBUFFER) == 1)
	{
		glutSwapBuffers();
		glClear(GL_COLOR_BUFFER_BIT);
	}
	
	// Establecer la zona visible (en píxeles) del scissor box
	float proporcionBarra = glutGet(GLUT_WINDOW_HEIGHT) / 6.0f;
	glScissor(0, proporcionBarra, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT) - proporcionBarra * 2);

	// Activar/desactivar el scissor test
	switchBoolParam(GL_SCISSOR_TEST);

	// Volver a dejar el color de fondo a blanco
	glClearColor(1, 1, 1, 0);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::skyboxButtonPressed()
{
	skyboxActive = !skyboxActive;

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::gammaButtonPressed()
{
	switchBoolParam(GL_FRAMEBUFFER_SRGB);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::stencilButtonPressed()
{
	switchBoolParam(GL_STENCIL_TEST);
	std::cout << "Stencil" << std::endl;
	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::logicOpButtonPressed()
{
	// Activar las operaciones lógicas implica desactivar totalmente el 'Blending'
	switchBoolParam(GL_COLOR_LOGIC_OP);

	// Tipo de operación que se hace sobre cada píxel
	// Curiosas : GL_COPY (predet.), GL_COPY_INVERTED (negativo), GL_INVERT (b/n)
	glLogicOp(GL_COPY_INVERTED);

	InputManager::Instance()->setMousePos(400, 300);
}

Scene::~Scene()
{
	// Borrar el canvas
	delete m_canvas;

	// Borrar las entidades
	CleanVector(m_entities);

	// Borrar las luces
	CleanVector(m_lights);

	// Borrar managers
	InputManager::Instance()->Clean();

	// Borrar las mallas, texturas, materiales y shaders cargados a la escena
	ResourceManager::Instance()->Clean();

	// Skybox
	delete m_skybox;

	// Malla para el postprocesado
	delete m_effectsMesh;
}
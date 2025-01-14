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

Scene::Scene() : m_camera(nullptr), m_canvas(nullptr), m_skybox(nullptr)
{
	// Crear la malla de rectángulo para el postprocesado
	// (2, 2) para que ocupe la pantalla entera
	m_effectsMesh = Mesh::generateRectangle(2, 2);
}

void Scene::AddEntity(Entity* e, bool isTranslucid)
{
	//if (!isTranslucid)
	//	m_opaqueEntities.push_back(e);
	//else
	//	m_translucentEntities.push_back(e);
	m_entities.push_back(e);
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
		m_skybox->getShader()->use();
		m_skybox->getShader()->setMat4d("mvpMat", projViewMat * m_skybox->getModelMat());

		// Pintar el skybox
		m_skybox->render();
	}
}

void Scene::renderEntities(const glm::dmat4& projViewMat)
{
	// Definir la forma de pintar la malla
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Pintar todas las entidades activas
	const Shader* activeShader = nullptr;
	for (Entity* e : m_entities)
	{
		if (e->isActive())
		{
			// la entidad no usa shaders; desactivamos
			if (e->getShader() == nullptr) // && activeShader != nullptr
			{
				Shader::turnOff();
				//activeShader = nullptr;
			}
				
			// la entidad usa shaders
			else
			{
				// hay que cambiar el shader que usó la entidad anterior
				if (e->getShader() != activeShader)
				{
					activeShader = e->getShader();
					activeShader->use();
				}
				// pasar las matrices necesarias al vertex shader
				activeShader->setMat4d("model", e->getModelMat());
				activeShader->setMat4d("view", m_camera->getViewMat());
				activeShader->setMat4d("projection", m_camera->getProjMat());

				// pasar las propiedades del material de la entidad al FS
				activeShader->setVec3("viewPos", m_camera->getPosition());
				activeShader->setVec3("material.specular", e->getMaterial()->getSpecular());
				activeShader->setFloat("material.brillo", e->getMaterial()->getBrillo());

				// por cada luz activa, pasamos sus propiedades al fragment shader
				for(int i = 0; i < m_lights.size(); i++)
				{
					Light* l = m_lights[i];
					std::string str = "luces[" + std::to_string(i) + "]";
					if (l->isActive())
					{
						//tipo de luz
						activeShader->setInt(str + ".type", l->getType()); 

						// pasar la información de las luces al fragment shader
						activeShader->setVec3(str + ".dir", l->getPosition());
						activeShader->setVec3(str + ".diffuse", l->getDiffuse());
						activeShader->setVec3(str + ".specular", l->getSpecular());

						// para luces NO direccionales exclusivamente (factores de atenuación)
						if (l->getType() != DIRECTIONAL_LIGHT)
						{
							activeShader->setFloat(str + ".constant", l->getAttenuation(0));
							activeShader->setFloat(str + ".linear", l->getAttenuation(1));
							activeShader->setFloat(str + ".quadratic", l->getAttenuation(2));
						}
					}
					else // 
					{
						activeShader->setVec3(str + ".diffuse", { 0, 0, 0 });
						activeShader->setVec3(str + ".specular", { 0, 0, 0 });
					}
				}
				// provisional
				activeShader->setMat4d("mvpMat", projViewMat * e->getModelMat());
			}
			// en cualquier caso, mandamos la info de los vértices
			e->render(m_camera->getViewMat());
		}
	}
	// Valor predet.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Scene::renderNormals(const glm::dmat4& projViewMat)
{
	if (normalsShader != nullptr)
	{
		normalsShader->use();
		for (Entity* e : m_entities)
		{
			// Pasar la matriz MVP al vertex shader y pintar
			normalsShader->setMat4d("mvpMat", projViewMat * e->getModelMat());
			e->render(m_camera->getViewMat());
		}
	}
}

void Scene::renderCanvas()
{
	if (m_canvas == nullptr)
		return;

	// Desactivamos cualquier shader que hubiera
	Shader::turnOff();

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
		compositeShader->use();

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
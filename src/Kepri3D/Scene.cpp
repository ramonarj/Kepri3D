#include "Scene.h"

#include "Game.h"
#include "Utils.h"
#include "Mesh.h"
#include "Texture.h"
#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "Shader.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "UI/Canvas.h"

#include <freeglut.h>

bool Scene::compositesActive = false;
bool Scene::skyboxActive = true;
bool Scene::mipmapsActive = false;
Shader* Scene::normalsShader = nullptr;


Scene::Scene() : m_canvas(nullptr), m_skybox(nullptr)
{
	m_camera = Game::Instance()->getCamera();

	// Crear la malla de rectángulo para el postprocesado. ({2, 2} para que ocupe la pantalla entera)
	m_effectsMesh = Mesh::generateRectangle(2, 2);

	// Crear los 2 FrameBuffers para  efectos
	frameBuf = new Framebuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), false);
	frameBuf2 = new Framebuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), false);

	// Crear el framebuffer para el multisampling
	msBuf = new Framebuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), true);

	// Composite por defecto
	//AddComposite((Shader*)&ResourceManager::Instance()->getComposite("defaultComposite"));

	// Crear los UBO para las matrices VP
	m_uboMatrices = new Uniformbuffer(0, sizeof(glm::dmat4) * 2);

	// Crear el UBO para las luces. Tamaño = 144 por temas de alineamiento
	m_uboLuces = new Uniformbuffer(1, 144);
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
	if(glGetError() != GL_NO_ERROR)
		std::cout << "ERROR OPENGL" << std::endl;

	// Activar este framebuffer; todo lo que se pinte se guardará en su textura
	if (compositesActive) { msBuf->bind(); }
	// 0) Limpiar el color y depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1) Cargar las luces; IMPORTANTE hacerlo antes de pintar los objetos a los que puedan iluminar
	loadLights();

	// 2) Pintar el skybox, si lo hay
	renderSkybox();

	// 3) Pintar todas las entidades activas
	renderEntities();

	// 4) Pintar los vectores normales, si están activos
	renderNormals();

	// 5) Pintar el canvas
	renderCanvas();

	// 6) Post-procesar la imagen del color buffer
	renderEffects();

	// 7) Hacer swap de buffers
	// Hay 2 buffers; uno se está mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void Scene::loadLights()
{
	for (Light* l : m_lights)
		if (l->isActive())
			l->load(m_camera->getViewMat());
}

void Scene::renderSkybox()
{
	const glm::dmat4 projViewMat = m_camera->getProjMat() * m_camera->getViewMat();

	// Comprobar que haya un skybox activo
	if (skyboxActive && m_skybox != nullptr)
	{
		// Activar el shader y pasarle la MVP
		m_skybox->getShader()->use();
		m_skybox->getShader()->setVec3("viewPos", m_camera->getPosition());
		m_skybox->getShader()->setMat4("projViewMat", projViewMat);

		// Pintar el skybox
		m_skybox->render();
	}
}

void Scene::renderEntities()
{
	// 1) Enviar uniforms comunes a todas las entidades
	sendUniformBlocks();

	// 2) Pintar todas las entidades activas
	for (Entity* e : m_entities)
	{
		if (e->isActive())
		{
			Shader* shader = (Shader*)e->getShader();
			// No usa shaders; los 'apagamos'
			if (shader == nullptr)
			{
				Shader::turnOff();
				// render s/shaders
				e->render(m_camera->getViewMat());
			}
			// Usa shaders; lo activamos y pasamos los valores uniform necesarios
			else
			{
				shader->use();
				sendUniforms(shader);
				// render c/shaders
				e->render();
			}
		}
	}
	// Valor predet.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Scene::renderNormals()
{
	if (normalsShader != nullptr)
	{
		normalsShader->use();
		for (Entity* e : m_entities)
		{
			if(e->getMesh() != nullptr)
			{
				// Pasar la matriz de modelado al VS y pintar
				normalsShader->setMat4d("model", e->getModelMat());
				e->render();
			}
		}
	}
}

void Scene::renderCanvas()
{
	if (m_canvas == nullptr)
		return;

	// Desactivamos cualquier shader que hubiera
	Shader::turnOff();
	bool lightOn = glIsEnabled(GL_LIGHTING);
	glDisable(GL_LIGHTING);

	//glClear(GL_DEPTH_BUFFER_BIT); // esto es más costoso que cambiar la función del Z-test
	// Cambiar la función del Z-test para pasarlo siempre
	glDepthFunc(GL_ALWAYS);
	// Pintar el canvas
	m_canvas->render(m_camera->getViewMat());

	// Dejarlo todo como estaba
	glDepthFunc(GL_LESS); // valor predet.
	if(lightOn)
		glEnable(GL_LIGHTING);
}

void Scene::renderEffects()
{
	// No hacemos nada
	if(!compositesActive){ return; }

	// Blit del Framebuffer con múltiples samples a uno normal
	msBuf->bind(GL_READ_FRAMEBUFFER);
	frameBuf->bind(GL_DRAW_FRAMEBUFFER);
	glBlitFramebuffer(0, 0, m_camera->getVP()->getW(), m_camera->getVP()->getH(),
		0, 0, m_camera->getVP()->getW(), m_camera->getVP()->getH(), GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glDepthFunc(GL_ALWAYS);

	Framebuffer* fbo1 = frameBuf;
	Framebuffer* fbo2 = frameBuf2;
	// Aplicamos solo los efectos que estén activos, haciendo "ping-pong" entre los 2 FBOs que tenemos
	// NOTA: siempre habrá al menos 1 composite (el default, que no hace nada). Si solo está este, no usamos el frameBuf2
	for(int i = 0; i < m_composites.size(); i++)
	{
		// Activar un buffer
		if (i < (m_composites.size() - 1))
			fbo2->bind();
		// (si es el último composite, pintamos al FrameBuffer predet. de OpenGL, no a los nuestros)
		else
			Framebuffer::unbind();

		// Usar el siguiente FX para pintar la textura (del otro buffer) sobre el rectángulo que ocupa la pantalla
		m_composites.at(i)->use();
		fbo1->bindTexture();
		m_effectsMesh->draw();

		// Intercambio de punteros
		std::swap(fbo1, fbo2);
	}
	glDepthFunc(GL_LESS);
}

void Scene::update(GLuint deltaTime)
{
	// Actualizar el canvas. Importante hacerlo antes que el resto de entidades
	if(m_canvas != nullptr)
		m_canvas->update(deltaTime);

	// Actualizar las entidades
	for (Entity* e : m_entities)
		if(e->isActive())
			e->update(deltaTime);

	// Limpiar el input para el siguiente frame
	InputManager::Instance()->Update();
}

void Scene::sendUniformBlocks()
{
	// - - - - - Matrices V y P - - - - - //
	glm::dmat4 proj = m_camera->getProjMat();
	glm::dmat4 view = m_camera->getViewMat();
	m_uboMatrices->bind();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::dmat4), glm::value_ptr(proj));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::dmat4), sizeof(glm::dmat4), glm::value_ptr(view));
	Uniformbuffer::unbind();

	// - - - - - - Luces - - - - - - - //
	Light* l = m_lights[0];
	int type = l->getType();
	glm::vec3 posDir = l->getPosition();
	glm::vec3 ambient = l->getAmbient();
	glm::vec3 diffuse = l->getDiffuse();
	glm::vec3 specular = l->getSpecular();


	m_uboLuces->bind();
	// Tipo de luz
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(int), &type);
	// Posición / dirección
	glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(glm::vec3), glm::value_ptr(posDir));
	// Componentes de la luz
	glBufferSubData(GL_UNIFORM_BUFFER, 32, sizeof(glm::vec3), glm::value_ptr(ambient));
	glBufferSubData(GL_UNIFORM_BUFFER, 48, sizeof(glm::vec3), glm::value_ptr(diffuse));
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(glm::vec3), glm::value_ptr(specular));
	// Factores de atenuación
	if (type != DIRECTIONAL_LIGHT)
	{
		float attConst = l->getAttenuation(0);
		float attLin = l->getAttenuation(1);
		float attQuad = l->getAttenuation(2);
		glBufferSubData(GL_UNIFORM_BUFFER, 76, sizeof(float), &attConst);
		glBufferSubData(GL_UNIFORM_BUFFER, 80, sizeof(float), &attLin);
		glBufferSubData(GL_UNIFORM_BUFFER, 84, sizeof(float), &attQuad);
		// Para focos necesitamos parámetros extra (dirección, apertura y exponente)
		if (type == SPOT_LIGHT)
		{
			glm::vec3 spotDir = l->getSpotDirection();
			float cutoff = l->getSpotCutoff();
			float spotExp = l->getSpotExponent();
			glBufferSubData(GL_UNIFORM_BUFFER, 96, sizeof(glm::vec3), glm::value_ptr(spotDir));
			glBufferSubData(GL_UNIFORM_BUFFER, 108, sizeof(float), &cutoff);
			glBufferSubData(GL_UNIFORM_BUFFER, 112, sizeof(float), &spotExp);
		}
	}
	// Indicar si la luz está encendida o no
	bool active = l->isActive();
	glBufferSubData(GL_UNIFORM_BUFFER, 116, sizeof(bool), &active);

	// Posición de la cámara (la metemos en el mismo UBO)
	glm::vec3 viewPos = m_camera->getPosition();
	glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(glm::vec3), &viewPos);
	
	// Reflejos blinn / blinn-phong
	glBufferSubData(GL_UNIFORM_BUFFER, 140, sizeof(bool), &blinn);

	Uniformbuffer::unbind();
}

void Scene::sendUniforms(Shader* sh)
{
	// posición de la cámara
	sh->setVec3("viewPos", m_camera->getPosition());

	//// todo esto ahora se pasa con UBOs
	//sh->setMat4d("view", m_camera->getViewMat());
	//sh->setMat4d("projection", m_camera->getProjMat());

	//// tipo de reflejos especulares
	//sh->setInt("blinn", blinn);

	//// por cada luz activa, pasamos sus propiedades al fragment shader
	//for (int i = 0; i < m_lights.size(); i++)
	//{
	//	Light* l = m_lights[i];
	//	std::string str = "luces[" + std::to_string(i) + "]";
	//	if (l->isActive())
	//	{
	//		//tipo de luz
	//		sh->setInt(str + ".type", l->getType());

	//		// pasar la información de las luces al fragment shader
	//		sh->setVec3(str + ".dir", l->getPosition());
	//		sh->setVec3(str + ".ambient", l->getAmbient());
	//		sh->setVec3(str + ".diffuse", l->getDiffuse());
	//		sh->setVec3(str + ".specular", l->getSpecular());

	//		// para luces NO direccionales exclusivamente (factores de atenuación)
	//		if (l->getType() != DIRECTIONAL_LIGHT)
	//		{
	//			sh->setFloat(str + ".constant", l->getAttenuation(0));
	//			sh->setFloat(str + ".linear", l->getAttenuation(1));
	//			sh->setFloat(str + ".quadratic", l->getAttenuation(2));
	//			// Para focos necesitamos parámetros extra
	//			if(l->getType() == SPOT_LIGHT)
	//			{
	//				sh->setVec3(str + ".spotDir", l->getSpotDirection());
	//				sh->setFloat(str + ".spotCutoff", l->getSpotCutoff());
	//				sh->setFloat(str + ".spotExp", l->getSpotExponent());
	//			}
	//		}
	//	}
	//	else // 
	//	{
	//		sh->setVec3(str + ".ambient", { 0, 0, 0 });
	//		sh->setVec3(str + ".diffuse", { 0, 0, 0 });
	//		sh->setVec3(str + ".specular", { 0, 0, 0 });
	//	}
	//}
}

void Scene::AddComposite(Shader* sh, bool active)
{
	m_composites.push_back(sh);
}

void Scene::resize(int width, int height)
{
	delete frameBuf; delete frameBuf2; delete msBuf;
	frameBuf = new Framebuffer(width, height, false);
	frameBuf2 = new Framebuffer(width, height, false);
	msBuf = new Framebuffer(width, height, true);
#ifdef __DEBUG_INFO__
	fbSize = { width, height };
#endif
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

	// FB para el postprocesado
	delete frameBuf;
	delete frameBuf2;

	//FB para multisampling
	delete msBuf;

	// UBO para matrices y luces
	delete m_uboMatrices;
	delete m_uboLuces;
}
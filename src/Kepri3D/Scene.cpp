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
#include "PhysicsSystem.h"
#include "UI/Canvas.h"
#include "Renderer.h"

#include <freeglut.h>

// Miembros estáticos
Camera* Scene::m_camera = nullptr;
Mesh* Scene::m_effectsMesh = nullptr;
Framebuffer* Scene::frameBuf = nullptr;
Framebuffer* Scene::frameBuf2 = nullptr;
Framebuffer* Scene::msBuf = nullptr;
Uniformbuffer* Scene::m_uboMatrices = nullptr;
Uniformbuffer* Scene::m_uboLuces = nullptr;
Shader* Scene::m_shadowComp = nullptr;
#ifdef __DEBUG_INFO__
glm::ivec2 Scene::fbSize;
#endif
// - - //
bool Scene::compositesActive = false;
bool Scene::skyboxActive = true;
bool Scene::mipmapsActive = false;
Shader* Scene::normalsShader = nullptr;

Scene::Scene() : m_canvas(nullptr), m_skybox(nullptr)
{

}

void Scene::setupStatics(Camera* cam)
{
	m_camera = cam;

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
	// 16 = viewPos + blinn | 120 = lo que ocupa una luz | 8 = relleno para que la siguiente luz empiece en múltiplo de 16
	m_uboLuces = new Uniformbuffer(1, 16 + LIGHT_STRUCT_SIZE * MAX_LUCES);

	// Debug
	ResourceManager::Instance()->loadComposite("shadowDebug.frag", "shadowComp");
	m_shadowComp = ((Shader*)&ResourceManager::Instance()->getComposite("shadowComp"));
	m_shadowComp->use();
	m_shadowComp->setInt("depthMap", 0);
}

void Scene::AddEntity(Entity* e)
{
	// Vector general
	m_entities.push_back(e);

	// Una entidad puede ser tranlúcida por: a) Material con diffuse.a < 1 | b) Textura difusa con transparencias
	bool trans = e->getMaterial()->getDiffuse().a < 1.0 ||
		(e->getTexture() != nullptr && e->getTexture()->hasAlpha());
	
	// Diferenciar entre entidades opacas y translúcidas
	if (trans)
		m_transEntities.push_back(e);
	else
		m_opaqueEntities.push_back(e);

	// Ver si tiene un componente Renderer
	Renderer* r = e->getComponent<Renderer>();
	if (r != nullptr)
		m_renderers.push_back(r);

	// Ver si tiene un componente Light
	Light* l = e->getComponent<Light>();
	if (l != nullptr)
		m_lights.push_back(l);
}

void Scene::render()
{
	//if(glGetError() != GL_NO_ERROR)
	//	std::cout << "ERROR OPENGL" << std::endl;

	// Activar este framebuffer; todo lo que se pinte se guardará en su textura
	if (compositesActive) { msBuf->bind(); }
	// 0) Limpiar el color y depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1) Cargar las luces (Fixed Pip.), fabricar los mapas de profundidad y rellenar el UBOs
	loadLights();
	//debugShadowMap();

	// 2) Rellenar UBO de las matrices
	loadMatrices();

	// 3) Pintar todas las entidades opacas
	renderEntities(m_opaqueEntities);

	// 4) Pintar los vectores normales, si están activos
	renderNormals();

	// 5) Pintar el skybox, si lo hay
	renderSkybox();

	// 6) Pintar todas las entidades transparentes
	glDepthMask(GL_FALSE);
	renderEntities(m_transEntities);
	glDepthMask(GL_TRUE);

	// 7) Pintar el canvas lo último
	renderCanvas();

	// 8) Post-procesar la imagen del color buffer
	renderEffects();

	// 9) Hacer swap de buffers
	// Hay 2 buffers; uno se está mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void Scene::loadLights()
{
	// 1) Fixed pipeline
	for (Light* l : m_lights)
		if (l->isActive())
			l->load(m_camera->getViewMat());

	// 2) Actualizar los mapas de sombras (la luces que lo requieran)
	bakeShadows();

	// 3) Rellenar el UBO con la información de todas las luces
	m_uboLuces->bind();

	// a) Común a todas las luces; posición de la cámara y tipo de reflejos
	glm::vec3 viewPos = m_camera->getPosition();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &viewPos);
	glBufferSubData(GL_UNIFORM_BUFFER, 12, sizeof(bool), &blinn);

	// b) Array de luces; cargar cada una de ellas
	GLintptr offset = 16;
	for(Light* l : m_lights)
	{
		l->loadToUBO(offset);
		// Hay que desperdiciar 8 bytes al final (del 120 - 128)
		offset += LIGHT_STRUCT_SIZE;
	}

	Uniformbuffer::unbind();
}

void Scene::debugShadowMap()
{
	Shadowmap* map = m_lights[0]->getShadowMap();
	m_shadowComp->use();
	m_shadowComp->setFloat("near_plane", map->nearPlane);
	m_shadowComp->setFloat("far_plane", map->farPlane);
	map->depthBuf->bindTexture();
	m_effectsMesh->draw();
}

void Scene::bakeShadows()
{
	for(Light* l : m_lights) // Light* l : m_lights
	{
		Shadowmap* map = l->getShadowMap();
		if (!l->isActive() || map == nullptr) { continue; }
		// Mandar los uniforms de las matrices
		l->sendShadowUniforms(m_uboMatrices);

		// Cambiar a la resolución del depth map
		glViewport(0, 0, map->width, map->height);
		map->depthBuf->bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		// Culling desactivado
		GLboolean cull = glIsEnabled(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		// Pintar todas las entidades activas con la resolución del depth map
		map->shader->use();
		for (Renderer* r : m_renderers)
		{
			Entity* e = r->getEntity();
			// Comprobamos que la entidad emita sombras
			if (e->isActive() && r != nullptr && r->castShadows())
			{
				// Pintar con el shader del Shadow Pass
				e->render(map->shader);
			}
		}
		// Valores prederminados
		if (cull) { glEnable(GL_CULL_FACE); }
		//glCullFace(GL_BACK);

		// Volver al frameBuffer anterior
		if (compositesActive) { msBuf->bind(); }
		else { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		glViewport(0, 0, m_camera->getVP()->getW(), m_camera->getVP()->getH());
	}
}

void Scene::renderSkybox()
{
	// Comprobar que haya un skybox activo
	if (skyboxActive && m_skybox != nullptr)
	{
		glDepthFunc(GL_LEQUAL);

		// Activar el shader y pasarle la posición de la cámara
		m_skybox->getShader()->use();
		m_skybox->getShader()->setVec3("viewPos", m_camera->getPosition());

		// Pintar el skybox
		m_skybox->render();

		glDepthFunc(GL_LESS);
	}
}

void Scene::renderEntities(const std::vector<Entity*>& entityList)
{
	// Pintar todas las entidades activas
	for (Entity* e : entityList)
	{
		//Entity* e = r->getEntity();
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
	if (normalsShader == nullptr)
		return;

	// Pintar todas las entidades con el shader de las normales
	normalsShader->use();
	for (Renderer* r : m_renderers)
	{
		//normalsShader->setMat4d("model", r->getEntity()->getModelMat());
		r->getEntity()->render();
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
	m_canvas->render();

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
	// Detectar y resolver colisiones
	PhysicsSystem::Instance()->update(deltaTime);
}

void Scene::loadMatrices()
{
	// - - - - - Matrices V y P - - - - - //
	glm::dmat4 proj = m_camera->getProjMat();
	glm::dmat4 view = m_camera->getViewMat();
	m_uboMatrices->bind();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::dmat4), glm::value_ptr(proj));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::dmat4), sizeof(glm::dmat4), glm::value_ptr(view));
	Uniformbuffer::unbind();
}


void Scene::sendUniforms(Shader* sh)
{
	// posición de la cámara; sigue siendo necesario para el terreno, que lo usa en el TCS
	sh->setVec3("viewPos", m_camera->getPosition());

	if (shadowsState == 0)
		return;

	// shadowmaps
	int ini_index = 8;
	for(int i = 0; i < m_lights.size(); i++)
	{
		Shadowmap* map = m_lights[i]->getShadowMap();
		if (map == nullptr) { continue; }

		std::string str = "shadowMaps[" + std::to_string(i) + "]";
		glActiveTexture(GL_TEXTURE0 + ini_index + i);
		// direccional
		if(m_lights[i]->getType() == DIRECTIONAL_LIGHT)
		{
			map->depthBuf->bindTexture();
			sh->setInt(str + ".directionalMap", ini_index + i);
			glm::dmat4 lightSpaceMat = map->lightProj * map->lightView;
			sh->setMat4d("lightSpaceMatrix", lightSpaceMat);
		}
		// puntual
		else
		{
			map->depthBuf->bindTexture(GL_TEXTURE_CUBE_MAP);
			sh->setInt(str + ".pointMap", ini_index + i);
			sh->setFloat(str + ".far_plane", map->farPlane);
		}
		sh->setInt(str + ".soft_shadows", map->softShadows);
	}

	// Las luces ahora se pasan con UBOs
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

void Scene::toggleShadows()
{
	shadowsState = (shadowsState + 1) % 3;
	if(shadowsState == 0)
	{
		for (Renderer* r : m_renderers)
			r->castShadows(false);
	}
	else if(shadowsState == 1)
	{
		for (Renderer* r : m_renderers)
			r->castShadows(true);
		for (int i = 0; i < m_lights.size(); i++)
		{
			Shadowmap* shMap = m_lights[i]->getShadowMap();
			if (shMap != nullptr)
				shMap->softShadows = false;
		}
	}
	else if(shadowsState == 2)
	{
		for (int i = 0; i < m_lights.size(); i++)
		{
			Shadowmap* shMap = m_lights[i]->getShadowMap();
			if(shMap != nullptr)
				shMap->softShadows = true;
		}
	}
}

void Scene::clean()
{
	// Borrar managers
	InputManager::Instance()->Clean();

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

Scene::~Scene()
{
	/* Borrar solo las cosas no estáticas */ 
	// Borrar el canvas
	delete m_canvas;

	// Apagar todas las luces; para que al cambiar la escena, no se quede el valor residual 'on' de las
	// luces y el shader lo siga usando
	m_uboLuces->bind();
	bool on = false;
	for (int i = 0; i < MAX_LUCES; i++)
		glBufferSubData(GL_UNIFORM_BUFFER, 16 + 116 + i * LIGHT_STRUCT_SIZE, sizeof(bool), &on);
	m_uboLuces->unbind();

	// Borrar las entidades y sus componentes
	CleanVector(m_entities);

	// Borrar managers
	PhysicsSystem::Instance()->Clean();

	// Borrar las mallas, texturas, materiales y shaders cargados a la escena
	ResourceManager::Instance()->Clean();

	// Skybox
	delete m_skybox;
}
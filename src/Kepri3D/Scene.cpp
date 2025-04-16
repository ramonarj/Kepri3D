#include "Scene.h"

#include "Kepri3D.h"

// Miembros estáticos
Camera* Scene::m_camera = nullptr;
Mesh* Scene::m_effectsMesh = nullptr;
Framebuffer* Scene::frameBuf = nullptr;
Framebuffer* Scene::frameBuf2 = nullptr;
Framebuffer* Scene::msBuf = nullptr;
Framebuffer* Scene::mrtBuf = nullptr;
Framebuffer* Scene::activeFB = nullptr;
Uniformbuffer* Scene::m_uboMatrices = nullptr;
Uniformbuffer* Scene::m_uboLuces = nullptr;
Shader* Scene::m_shadowComp = nullptr;
#ifdef __DEBUG_INFO__
glm::ivec2 Scene::fbSize;
#endif
// - - //
bool Scene::msaa_active = false;
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

	// - - Framebuffers - - //
	// efectos
	frameBuf = new Framebuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), false);
	frameBuf2 = new Framebuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), false);
	// multisampling
	msBuf = new Framebuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), true);
	// MRT
	mrtBuf = Framebuffer::createMRTBuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH());

	// - - UBOs - - //
	// matrices VP
	m_uboMatrices = new Uniformbuffer(0, sizeof(glm::dmat4) * 2);
	// luces. Tamaño = 144 por temas de alineamiento
	// 16 = viewPos + blinn | 120 = lo que ocupa una luz | 8 = relleno para que la siguiente luz empiece en múltiplo de 16
	m_uboLuces = new Uniformbuffer(1, 16 + LIGHT_STRUCT_SIZE * MAX_LUCES);

	// Debug
	ResourceManager::Instance()->loadComposite("shadowDebug.frag", "shadowComp");
	m_shadowComp = (ResourceManager::Instance()->getComposite("shadowComp"));
	m_shadowComp->use();
	m_shadowComp->setInt("depthMap", 0);
}

void Scene::startComponents()
{
	for (Entity* e : m_entities)
		for (Component* c : e->getComponents())
			c->start();
}

void Scene::AddEntity(Entity* e)
{
	// Vector general
	m_entities.push_back(e);

	// Ver si tiene un componente Renderer
	Renderer* r = e->getComponent<Renderer>();
	if (r != nullptr)
	{
		m_renderers.push_back(r);

		// Una entidad puede ser tranlúcida por: a) Material con diffuse.a < 1 | b) Textura difusa con transparencias
		Material* m = e->getMaterial();
		bool trans = m->getDiffuse().a < 1.0 ||
			(m->getTexture(0) != nullptr && m->getTexture(0)->hasAlpha());

		// Diferenciar entre entidades opacas y translúcidas
		if (trans)
			m_transEntities.push_back(e);
		else
			m_opaqueEntities.push_back(e);
	}

	// Ver si tiene un componente Light
	Light* l = e->getComponent<Light>();
	if (l != nullptr)
		m_lights.push_back(l);

	// Ver si tiene un componente Rigid (añadir a la simulación)
	Rigid* rigid = e->getComponent<Rigid>();
	if (rigid != nullptr) { PhysicsSystem::Instance()->addRigid(rigid); }
}

void Scene::render()
{
	//if(glGetError() != GL_NO_ERROR)
	//	std::cout << "ERROR OPENGL" << std::endl;

	// Activar este framebuffer; todo lo que se pinte se guardará en su textura
	if (msaa_active) { activeFB = msBuf; }
	else { activeFB = mrtBuf; }
	activeFB->bind();
	// 0) Limpiar el color y depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1) Cargar las luces (Fixed Pip.), fabricar los mapas de profundidad y rellenar el UBOs
	loadLights();

	// 2) Rellenar UBO de las matrices
	loadMatrices();

	// 3) Pintar todas las entidades opacas
	renderOpaques();

	// 4) Pintar los vectores normales, si están activos
	renderNormals();

	// 5) Pintar el skybox, si lo hay
	renderSkybox();

	// 6) Pintar todas las entidades transparentes
	renderTranslucids();

	// 7) Pintar el canvas lo último
	renderCanvas();

	//debugDepthMap();

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

void Scene::debugDepthMap()
{
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_ALWAYS);

	bool sombras = false;
	// Sombras
	if(sombras)
	{
		Shadowmap* map = m_lights[0]->getShadowMap();
		m_shadowComp->use();
		m_shadowComp->setFloat("near_plane", map->nearPlane);
		m_shadowComp->setFloat("far_plane", map->farPlane);
		m_shadowComp->setInt("perspective", false);
		map->depthBuf->bindTexture();
		m_effectsMesh->draw();
	}
	// MRT
	else
	{
		m_shadowComp->use();
		m_shadowComp->setFloat("near_plane", m_camera->getNearPlane());
		m_shadowComp->setFloat("far_plane", m_camera->getFarPlane());
		m_shadowComp->setInt("perspective", !m_camera->isOrto());
		activeFB->bindDepth();
		m_effectsMesh->draw();
	}
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}

void Scene::bakeShadows()
{
	if (shadowsState == 0) { return; }
	for(Light* l : m_lights) // Light* l : m_lights
	{
		Shadowmap* map = l->getShadowMap();
		if (!l->isActive() || map == nullptr) { continue; }
		// Mandar los uniforms de las matrices
		l->sendShadowUniforms(m_uboMatrices, m_camera->getPosition());

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
		activeFB->bind();
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

void Scene::renderOpaques()
{
	renderEntities(m_opaqueEntities);
}

void Scene::renderTranslucids()
{
	// [Blending desactivado]
	if (!glIsEnabled(GL_BLEND)) {
		renderEntities(m_transEntities);
		return;
	}

	// Ordenar las entidades translúcidas de más lejanas a más cercanas (algoritmo del pintor)
	glm::dvec3 camPos = m_camera->getPosition();
	// Selection Sort chapurrero
	for (int i = 0; i < m_transEntities.size(); i++)
	{
		float maxDist = glm::length(camPos - m_transEntities[i]->getPosition());
		for (int j = i + 1; j < m_transEntities.size(); j++)
		{
			float dist = glm::length(camPos - m_transEntities[j]->getPosition());
			if (dist > maxDist)
			{
				// update max dist
				maxDist = dist;
				// swap
				std::swap(m_transEntities[i], m_transEntities[j]);
				//std::cout << "Swap" << std::endl;
				//Entity* aux = m_transEntities[i];
				//m_transEntities[i] = m_transEntities[j];
				//m_transEntities[j] = aux;
			}
		}
	}
	
	// Comprobación del algoritmo
	//for(int i = 1; i < m_transEntities.size(); i++)
	//{
	//	float dist = glm::length(camPos - m_transEntities[i]->getPosition());
	//	for(int j = i + 1; j < m_transEntities.size(); j++)
	//	{
	//		if (glm::length(camPos - m_transEntities[j]->getPosition()) > dist)
	//		{
	//			std::cout << "Fallo" << std::endl;
	//			std::cout << glm::length(camPos - m_transEntities[i]->getPosition());
	//			std::cout << " vs " << dist << std::endl;
	//		}
	//	}
	//}

	// Dibujarlas con el Z-write desactivado
	glDepthMask(GL_FALSE);
	renderEntities(m_transEntities);
	glDepthMask(GL_TRUE);
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
		r->getEntity()->render(normalsShader);
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

void Scene::Blit(Framebuffer* readFB, Framebuffer* writeFB)
{
	readFB->bind(GL_READ_FRAMEBUFFER);
	if (writeFB == nullptr)
		Framebuffer::unbind(GL_DRAW_FRAMEBUFFER);
	else
		writeFB->bind(GL_DRAW_FRAMEBUFFER);

	GLint w = m_camera->getVP()->getW();
	GLint h = m_camera->getVP()->getH();
	glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Scene::renderEffects()
{
	// No hay efectos activos -> solo volcamos el FBO en el FB de OpenGL
	if(!compositesActive || m_composites.size() == 0)
	{
		Blit(activeFB, nullptr);
		return;
	}
	// Blit del Framebuffer con múltiples samples a uno normal
	else if(true) //msaa_active
	{
		Blit(activeFB, frameBuf);
	}

	glDepthFunc(GL_ALWAYS);

	Framebuffer* fbo1 = frameBuf;
	Framebuffer* fbo2 = frameBuf2;
	Framebuffer* depthFB = activeFB;
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
		// RTTO
		glActiveTexture(GL_TEXTURE0 + 9);
		fbo1->bindTexture();
		m_composites.at(i)->setInt("RTT0", 9);
		// depth
		glActiveTexture(GL_TEXTURE0 + 8);
		depthFB->bindDepth();
		m_composites.at(i)->setInt("depthBuf", 8);
		// dibujar
		m_effectsMesh->draw();

		// Intercambio de punteros
		std::swap(fbo1, fbo2);
	}
	glDepthFunc(GL_LESS);
}

void Scene::update(float deltaTime)
{
	// Actualizar el canvas. Importante hacerlo antes que el resto de entidades
	if(m_canvas != nullptr)
		m_canvas->update(deltaTime);

	// Actualizar las entidades
	for (Entity* e : m_entities)
		if(e->isActive())
			e->update(deltaTime);

	// Actualizar sistema de audio
	AudioManager::Instance()->Update(deltaTime);

	// Limpiar el input para el siguiente frame
	InputManager::Instance()->Update();

	// Informar al motor físico de cuánto tiempo ha pasado (él decidirá si le toca actualizarse)
	PhysicsSystem::Instance()->update(deltaTime);
}
void Scene::callFixedUpdates(float fixedTime)
{
	for (Entity* e : m_entities)
		if (e->isActive())
			e->fixedUpdate(fixedTime);
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

	// depth buffer (podría hacer falta para usar la ley de Beer-Lambert en objetos traslúcidos)
	/*
	glActiveTexture(GL_TEXTURE0 + 8);
	activeFB->bindDepth();
	sh->setInt("depthBuf", 8);
	*/

	if (shadowsState == 0)
		return;

	// shadowmaps
	int ini_index = 20;
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
			map->depthBuf->bindTexture();
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
	delete frameBuf; delete frameBuf2; delete msBuf; delete mrtBuf;
	frameBuf = new Framebuffer(width, height, false);
	frameBuf2 = new Framebuffer(width, height, false);
	msBuf = new Framebuffer(width, height, true);
	mrtBuf = Framebuffer::createMRTBuffer(width, height);
#ifdef __DEBUG_INFO__
	fbSize = { width, height };
#endif
}

void Scene::toggleShadows()
{
	shadowsState = (shadowsState + 1) % 3;
	if(shadowsState == 1)
	{
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

void Scene::init()
{
	// 1) Cargar recursos necesarios
	loadResources();
	// 2) Crear las entidades y componentes
	setup();
	// 3) Gestionar VRAM
	bindUBOs();
	// 4) Llamar al start() de todos los componentes
	startComponents();
}

void Scene::bindUBOs()
{
	// Establecer el punto de enlace de los shaders que usen el UBO
	for (Entity* e : m_entities)
	{
		if (e->getRenderer() != nullptr)
		{
			Shader* sh = (Shader*)e->getShader();
			if (sh != nullptr)
			{
				sh->bindUniformBlock("Matrices", 0);
				sh->bindUniformBlock("Lights", 1);
			}
		}
	}
}

void Scene::clean()
{
	// Borrar managers
	InputManager::Instance()->Clean();
	AudioManager::Instance()->Clean();

	// Malla para el postprocesado
	delete m_effectsMesh;

	// FB para el postprocesado
	delete frameBuf;
	delete frameBuf2;

	//FB para multisampling
	delete msBuf;

	// FB con MRT
	delete mrtBuf;

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
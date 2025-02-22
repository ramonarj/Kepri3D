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
#include "Renderer.h"

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
	// 16 = viewPos + blinn | 120 = lo que ocupa una luz | 8 = relleno para que la siguiente luz empiece en múltiplo de 16
	m_uboLuces = new Uniformbuffer(1, 16 + LIGHT_STRUCT_SIZE * MAX_LUCES);

	// Shadow maps
	const unsigned int SHADOW_SIZE = 1024;
	// a) Luz direccional
	Framebuffer* shadowFB = Framebuffer::createShadowMap(SHADOW_SIZE, SHADOW_SIZE);
	Shader* shadowSh = (Shader*)&ResourceManager::Instance()->getShader("shadows");
	shadowMaps[0] = Shadowmap(shadowFB, shadowSh, SHADOW_SIZE, SHADOW_SIZE, 1.0f, 100.0f);
	float ortoSize = 50.0f;
	lightProj = glm::ortho(-ortoSize, ortoSize, -ortoSize, ortoSize, shadowMaps[0].nearPlane, shadowMaps[0].farPlane);

	// b) Luz puntual
	Framebuffer* pointShadowFB = Framebuffer::createShadowMap(SHADOW_SIZE, SHADOW_SIZE, true);
	Shader* pointShadowSh = (Shader*)&ResourceManager::Instance()->getShader("shadows_point");
	shadowMaps[1] = Shadowmap(pointShadowFB, pointShadowSh, SHADOW_SIZE, SHADOW_SIZE, 1.0f, 50.0f);

	// Debug
	ResourceManager::Instance()->loadComposite("shadowDebug.frag", "shadowComp");
	m_shadowComp = ((Shader*)&ResourceManager::Instance()->getComposite("shadowComp"));
	m_shadowComp->use();
	m_shadowComp->setInt("depthMap", 0);
}

void Scene::AddEntity(Entity* e, bool isTranslucid)
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

	// 1) Cargar las luces; IMPORTANTE hacerlo antes de pintar los objetos a los que puedan iluminar
	loadLights();

	// 2) Fabricar los mapas de profundidad de las luces
	renderShadows();
	//debugShadowMap();

	// 3) Enviar uniforms comunes a todas las entidades (matrices y luces)
	sendUniformBlocks();

	// 4) Pintar todas las entidades opacas
	renderEntities(m_opaqueEntities);

	// 5) Pintar los vectores normales, si están activos
	renderNormals();

	// 6) Pintar el canvas
	renderCanvas();

	// 7) Pintar el skybox, si lo hay
	renderSkybox();

	// 7.5) Pintar todas las entidades transparentes
	glDepthMask(GL_FALSE);
	renderEntities(m_transEntities);
	glDepthMask(GL_TRUE);

	// 8) Post-procesar la imagen del color buffer
	renderEffects();

	// 8) Hacer swap de buffers
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

void Scene::debugShadowMap()
{
	m_shadowComp->use();
	m_shadowComp->setFloat("near_plane", shadowMaps[0].nearPlane);
	m_shadowComp->setFloat("far_plane", shadowMaps[0].farPlane);
	shadowMaps[0].depthBuf->bindTexture();
	m_effectsMesh->draw();
}

void Scene::renderShadows()
{
	for(int i = 0; i < 2; i++) // Light* l : m_lights
	{
		// Mandar los uniforms de las matrices
		sendShadowUniforms(shadowMaps[i], m_lights[i], m_lights[i]->getType());

		// Cambiar a la resolución del depth map
		glViewport(0, 0, shadowMaps[i].width, shadowMaps[i].height);
		shadowMaps[i].depthBuf->bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		// Culling desactivado
		GLboolean cull = glIsEnabled(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		// Pintar todas las entidades activas con la resolución del depth map
		shadowMaps[i].shader->use();
		for (Renderer* r : m_renderers)
		{
			Entity* e = r->getEntity();
			// Comprobamos que la entidad emita sombras
			if (e->isActive() && r != nullptr && r->castShadows())
			{
				// Guardamos su shader y le ponemos el barato
				Shader* shader = (Shader*)e->getShader();
				e->setShader(shadowMaps[i].shader);
				// Pintar
				e->render();
				// Devolverle el suyo
				e->setShader(shader);
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
		// Pasar la matriz de modelado al VS y pintar
		normalsShader->setMat4d("model", r->getEntity()->getModelMat());
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

	// a) Común a todas las luces
	m_uboLuces->bind();
	// Posición de la cámara
	glm::vec3 viewPos = m_camera->getPosition();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec3), &viewPos);

	// Reflejos blinn / blinn-phong
	glBufferSubData(GL_UNIFORM_BUFFER, 12, sizeof(bool), &blinn);

	// b) Array de luces
	GLintptr offset = 16;
	for(int i = 0; i < m_lights.size(); i++)
	{
		Light* l = m_lights[i];
		int type = l->getType();
		glm::vec3 posDir = (type == DIRECTIONAL_LIGHT) ? l->getDirection() : (glm::vec3)l->getEntity()->getPosition();
		glm::vec3 ambient = l->getAmbient();
		glm::vec3 diffuse = l->getDiffuse();
		glm::vec3 specular = l->getSpecular();

		// Tipo de luz
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(int), &type);
		// Posición / dirección
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 16, sizeof(glm::vec3), glm::value_ptr(posDir));
		// Componentes de la luz
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 32, sizeof(glm::vec3), glm::value_ptr(ambient));
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 48, sizeof(glm::vec3), glm::value_ptr(diffuse));
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 64, sizeof(glm::vec3), glm::value_ptr(specular));
		// Factores de atenuación
		if (type != DIRECTIONAL_LIGHT)
		{
			float attConst = l->getAttenuation(0);
			float attLin = l->getAttenuation(1);
			float attQuad = l->getAttenuation(2);
			glBufferSubData(GL_UNIFORM_BUFFER, offset + 76, sizeof(float), &attConst);
			glBufferSubData(GL_UNIFORM_BUFFER, offset + 80, sizeof(float), &attLin);
			glBufferSubData(GL_UNIFORM_BUFFER, offset + 84, sizeof(float), &attQuad);
			// Para focos necesitamos parámetros extra (dirección, apertura y exponente)
			if (type == SPOT_LIGHT)
			{
				glm::vec3 spotDir = l->getSpotDirection();
				float cutoff = l->getSpotCutoff();
				float spotExp = l->getSpotExponent();
				glBufferSubData(GL_UNIFORM_BUFFER, offset + 96, sizeof(glm::vec3), glm::value_ptr(spotDir));
				glBufferSubData(GL_UNIFORM_BUFFER, offset + 108, sizeof(float), &cutoff);
				glBufferSubData(GL_UNIFORM_BUFFER, offset + 112, sizeof(float), &spotExp);
			}
		}
		// Indicar si la luz está encendida o no
		bool active = l->isActive();
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 116, sizeof(bool), &active);

		// Hay que desperdiciar 8 bytes al final (del 120 - 128)
		offset += LIGHT_STRUCT_SIZE;
	}

	Uniformbuffer::unbind();
}

void Scene::sendShadowUniforms(Shadowmap map, Light* l, bool point)
{
	// Luces direccionales
	if(!point)
	{
		glm::vec3 lightDir = l->getDirection();
		// Actualizar la matriz de vista y ponerla donde la luz
		glm::vec3 origen = { 0, 0, 0 };
		lightView = glm::lookAt(origen + lightDir * distOrigen, origen, glm::vec3(0.0, 1.0, 0.0));

		// Mandar el uniform
		m_uboMatrices->bind();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::dmat4), glm::value_ptr(lightProj));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::dmat4), sizeof(glm::dmat4), glm::value_ptr(lightView));
		Uniformbuffer::unbind();
	}
	// Luces puntuales
	else
	{
		glm::vec3 lightPos = l->getEntity()->getPosition();
		// Actualizar la matriz de vista y ponerla donde la luz
		float aspect = (float)map.width / (float)map.height;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect,
			map.nearPlane, map.farPlane);

		// Matrices de vista (1 por cada cara del cubemap). Der, Izq, Arr, Aba, Fre, Atr
		std::vector<glm::mat4> shadowTransforms;
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
		shadowTransforms.push_back(shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

		// Mandar los uniforms
		map.shader->use();
		for (int i = 0; i < 6; i++) {
			map.shader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
		map.shader->setVec3("lightPos", lightPos);
		map.shader->setFloat("far_plane", map.farPlane);
	}
}

void Scene::sendUniforms(Shader* sh)
{
	// posición de la cámara; sigue siendo necesario para el terreno, que lo usa en el TCS
	sh->setVec3("viewPos", m_camera->getPosition());

	// matriz luz
	glm::dmat4 lightSpaceMat = lightProj * lightView;
	sh->setMat4d("lightSpaceMatrix", lightSpaceMat);

	// pasarle el shadow map
	// Direccional
	glActiveTexture(GL_TEXTURE0 + 8);
	shadowMaps[0].depthBuf->bindTexture();
	sh->setInt("shadowMap.directionalMap", 8);

	// Puntual
	glActiveTexture(GL_TEXTURE0 + 9);
	shadowMaps[1].depthBuf->bindTexture(GL_TEXTURE_CUBE_MAP);
	sh->setInt("shadowMap.pointMap", 9);
	sh->setFloat("shadowMap.far_plane", shadowMaps[1].farPlane);


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

void Scene::toggleShadows()
{
	shadowsEnabled = !shadowsEnabled;
	for (Renderer* r : m_renderers)
		r->castShadows(shadowsEnabled);
}

Scene::~Scene()
{
	// Borrar el canvas
	delete m_canvas;

	// Borrar las entidades y sus componentes
	CleanVector(m_entities);

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

	// Shadow maps
	for (int i = 0; i < 2; i++)
		shadowMaps[i].clean();
}
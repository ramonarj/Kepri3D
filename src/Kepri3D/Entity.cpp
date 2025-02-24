#include "Entity.h"

#include <iostream>

#include <gtc/type_ptr.hpp>

#include "Utils.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshLoader.h"
#include "Shader.h"
#include "Component.h"
#include "Camera.h"
#include "Renderer.h"
#include "Collider.h"

#include <freeglut.h>

using namespace glm;

Entity::Entity(const std::string& name)
{
	// Valores por defecto de la entidad
	defaultValues();
	m_name = name;
}

Entity::Entity(std::vector<Component*> comps, const std::string& name) : Entity(name)
{
	for (Component* c : comps)
		addComponent(c);
}


Entity::~Entity()
{
	// Destruir los hijos
	for (Entity* e : m_children)
		delete e;

	// Destruir los componentes
	for (Component* c : m_componentes)
		delete c;
}

void Entity::defaultValues()
{
	m_active = true;
	m_parent = nullptr;
	m_renderer = nullptr;

	// Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	modelMat = (1.0);
	m_shader = nullptr;
	m_collider = nullptr;
}

void Entity::addComponent(Component* c)
{
	// Comprobar que no lo tenga ya
	for (Component* it : m_componentes)
		if (it == c)
			return;
	// Añadirlo a la lista y darle nuestra referencia
	m_componentes.push_back(c);
	c->setEntity(this);

	// Comprobar si es un Collider
	Collider* col = dynamic_cast<Collider*>(c);
	if (col != nullptr) { m_collider = col; }

	// Comprobar si es un Renderer
	Renderer* rend = dynamic_cast<Renderer*>(c);
	if (rend != nullptr) { m_renderer = rend; }
}

void Entity::render(glm::dmat4 const& viewMat)
{
	// 1) Renderizar la propia entidad
	// Activar la textura si la tiene
	//if (m_textures[0] != nullptr) //TODO
	//	m_textures[0]->bind();

	// 1) Cargar la matriz V*M
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz VM
	glm::dmat4 modelViewMat = viewMat * modelMat;
	glLoadMatrixd(value_ptr(modelViewMat));

	// 2) Dibujar la/s malla/s
	m_material.load();
	if (m_renderer != nullptr)
		m_renderer->draw();

	// Desactivar la textura si la tiene
	//if (m_textures[0] != nullptr)
	//	m_textures[0]->unbind();


	// 2) Renderizar sus hijos
	for(Entity* e : m_children)
		if(e->isActive())
			e->render(modelViewMat);
}

void Entity::render()
{
	// 1) Renderizar la propia entidad
	// Cargar el material
	m_material.loadToShader(m_shader);

	// Dibujar la/s malla/s
	sendUniforms();
	if (m_renderer != nullptr)
		m_renderer->draw();
	// Debug del collider
	if (m_collider != nullptr) { m_collider->render(); }

	// Desactivar texturas
	m_material.unload();

	// 2) Renderizar sus hijos con el mismo shader dado
	for (Entity* e : m_children)
		if (e->isActive())
			e->render();
}

void Entity::sendUniforms()
{
	if (m_shader == nullptr)
		return;

	// Deshacer la posición de los hijos relativa al padre
	glm::dmat4 model = modelMat;
	Entity* parent = m_parent;
	while (parent != nullptr)
	{
		model = parent->modelMat * model;
		parent = parent->m_parent;
	}

	m_shader->setMat4d("model", model);
	// Sombras
	m_shader->setInt("receive_shadows", m_renderer->receiveShadows());
}

void Entity::update(GLuint deltaTime)
{
	// Actualiza cada uno de sus componentes
	for(Component* c : m_componentes)
	{
		if (c->isActive())
			c->update(deltaTime);
	}

	// Actualiza los hijos
	for(Entity* e : m_children)
		if(e->isActive())
			e->update(deltaTime);
}

void Entity::setPosition(const glm::dvec3& pos)
{
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = pos.x;
	modelMat[3][1] = pos.y;
	modelMat[3][2] = pos.z;
}

void Entity::translate(const glm::dvec3& transVector, ReferenceSystem refSys)
{
	dmat4 transMat = glm::translate({ 1.0 }, transVector);

	// Traslación local: postmultiplicamos la matriz de traslación
	if(refSys == LOCAL)
	{
		modelMat = modelMat * transMat;
	}
	// Traslación global: premultiplicamos " "
	else if(refSys == GLOBAL)
	{
		modelMat = transMat * modelMat;
	}
}

void Entity::rotate(GLdouble alpha, const glm::dvec3& axis, ReferenceSystem refSys)
{
	// Rotación local: postmultiplicamos la matriz de rotación
	if(refSys == LOCAL)
	{
		// por ahora todas las rotaciones son locales
		modelMat = glm::rotate(modelMat, alpha, axis);
	}
	// Rotación global: premultiplicamos la matriz de rotación
	else if (refSys == GLOBAL)
	{
		// Guardarnos la posición previa
		dvec3 pos = { modelMat[3][0], modelMat[3][1], modelMat[3][2] };

		// Rotar respecto al eje global
		dmat4 rotMatrix = glm::rotate(dmat4(1.0), alpha, axis);
		modelMat = rotMatrix * modelMat;

		// Devolver la entidad a su sitio
		setPosition(pos);
	}
}

void Entity::scale(const glm::dvec3& scale)
{
	modelMat = glm::scale(modelMat, scale);
}

void Entity::setParent(Entity* e)
{
	// Actualizar referencia al padre
	m_parent = e;
	// Añadirnos a sus hijos
	e->m_children.push_back(this);
	// Solución temporal
	m_shader = m_parent->m_shader;
}

void Entity::setMesh(const std::string& meshID)
{
	if (m_renderer != nullptr)
		delete m_renderer;
	m_renderer = new Renderer((Mesh*) & ResourceManager::Instance()->getMesh(meshID));
	addComponent(m_renderer);
}

void Entity::setMaterial(const std::string& materialID)
{
	m_material = ResourceManager::Instance()->getMaterial(materialID);
}

void Entity::setShader(const std::string& shaderID)
{
	m_shader = (Shader*)&ResourceManager::Instance()->getShader(shaderID);
}

void Entity::setTexture(const std::string& textureID)
{
	m_material.setTexture(0, (Texture*)&ResourceManager::Instance()->getTexture(textureID));
}

void Entity::setSecondTex(const std::string& textureID)
{
	m_material.setTexture(1, (Texture*)&ResourceManager::Instance()->getTexture(textureID));
}

void Entity::setSpecularMap(const std::string& textureID)
{
	m_material.setTexture(2, (Texture*)&ResourceManager::Instance()->getTexture(textureID));
}

void Entity::setNormalMap(const std::string& textureID)
{
	m_material.setTexture(3, (Texture*)&ResourceManager::Instance()->getTexture(textureID));
	static_cast<IndexMesh*>(m_renderer->getMesh())->setTangents();
}

void Entity::setDisplacementMap(const std::string& textureID)
{
	m_material.setTexture(4, (Texture*)&ResourceManager::Instance()->getTexture(textureID));
}

void Entity::enableReflections(const std::string& reflectionMapID, const std::string& cubemapID)
{
	m_material.setTexture(5, (Texture*)&ResourceManager::Instance()->getTexture(reflectionMapID));
	m_material.setTexture(6, (Texture*)&ResourceManager::Instance()->getTexture(cubemapID));
}


// - - - - - - - - - - - - - - - - - 

EjesRGB::EjesRGB(GLdouble l)
{
	Renderer* rend = new Renderer(Mesh::generateAxesRGB(l));
	addComponent(rend);
	m_name = "EjesRGB";
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, GLdouble size, bool relleno)
{
	Renderer* rend;
	if(relleno)
		rend = new Renderer(Mesh::generateFilledPolygon(sides, size));
	else
		rend = new Renderer(Mesh::generatePolygon(sides, size));
	addComponent(rend);
	m_name = "Poligono";
}


// - - - - - - - - - - - - - - - - - 

Cubo::Cubo(GLdouble size, bool equalFaces)
{
	Renderer* rend = new Renderer(IndexMesh::generateCube(size, equalFaces));
	addComponent(rend);
	m_name = "Cubo";
}


// - - - - - - - - - - - - - - - - - 

Esfera::Esfera(GLdouble radio, GLuint paralelos, GLuint meridianos)
{
	Renderer* r = new Renderer(IndexMesh::generateSphere(radio, paralelos, meridianos));
	addComponent(r);
	m_name = "Esfera";
}

// - - - - - - - - - - - - - - - - - 

Cilindro::Cilindro(GLdouble radio, GLdouble altura, GLuint lados)
{
	Renderer* rend = new Renderer(IndexMesh::generateCilindro(radio, altura, lados));
	addComponent(rend);
	m_name = "Cilindro";
}


// - - - - - - - - - - - - - - - - - 

Toro::Toro(GLdouble radExt, GLdouble radInt, GLuint anillos, GLuint lineas)
{
	Renderer* rend = new Renderer(IndexMesh::generateToro(radExt, radInt, anillos, lineas));
	addComponent(rend);
	m_name = "Toro";
}

// - - - - - - - - - - - - - - - - - 

Grid::Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	Renderer* rend = new Renderer(IndexMesh::generateGrid(filas, columnas, tamFila, tamColumna));
	addComponent(rend);
	m_name = "Grid";
}

// - - - - - - - - - - - - - - - - 

Terrain::Terrain()
{
	m_name = "Terrain";
}

void Terrain::loadRAW(const std::string& rawFile, GLdouble scale)
{
	Renderer* rend = new Renderer(IndexMesh::generateTerrain(rawFile, scale, true));
	addComponent(rend);
}

void Terrain::loadHeightMap(const std::string& heightMap, GLdouble scale)
{
	Renderer* rend = new Renderer(IndexMesh::generateTerrain(heightMap, scale, false));
	addComponent(rend);
}

// - - - - - - - - - - - - - - - - - 

Skybox::Skybox(const std::string& cubemapTextureID)
{
	// Generar la malla y cargar la textura y el shader
	Renderer* rend = new Renderer(IndexMesh::generateCubemap(4.0));
	addComponent(rend);
	m_name = "Skybox_" + cubemapTextureID;

	// Las caras están puestas para que miren hacia dentro del cubo
	setTexture(cubemapTextureID);
	setShader("skybox");
}

// - - - - - - - - - - - - - - - - -

Billboard::Billboard(const std::string& textureID, GLfloat width, GLfloat height)
{
	Renderer* rend = new Renderer(IndexMesh::generateRectangle(width, height));
	addComponent(rend);
	m_name = "Billboard";
	setTexture(textureID);

	setShader("billboard");
	// Para que el billboard esté anclado por abajo (valor por defecto)
	ancla = glm::vec2(0, - height / 2.0f);
}

void Billboard::render()
{
	// Uniforms necesarios
	m_shader->setVec2("ancla", ancla);
	Entity::render();
}

// - - - - - - - - - - - - - - - - - 

MovingGrid::MovingGrid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna) :
	Grid(filas, columnas, tamFila, tamColumna)
{
	m_name = "MovingGrid";

	setShader("movimiento");

	velDisp = { -1, -1 };
	velTex = { 2, 0 };
}

void MovingGrid::render()
{
	// Pasarle el tiempo y velocidad de desplazamiento al fragment shader
	float t = glutGet(GLUT_ELAPSED_TIME);
	m_shader->setFloat("tiempo", t / 10000.0f);
	m_shader->setVec2("velTex", velTex);
	m_shader->setVec2("velDisp", velDisp);

	Entity::render();
}

// - - - - - - - - - - - - - - - - - 

CuboMultitex::CuboMultitex(GLdouble size)
{
	Renderer* rend = new Renderer(IndexMesh::generateCube(size, true));
	addComponent(rend);
	m_name = "CuboMT";

	setShader("multitexture");
}


void CuboMultitex::render()
{
	m_shader->setFloat("mix", (sin(glutGet(GLUT_ELAPSED_TIME) * 0.001) + 1) / 2.0f);
	Entity::render();
}

// - - - - - - - - - - - - - - - - - 

Hierba::Hierba(GLdouble width, GLdouble height, const std::string& textureID)
{
	Renderer* rend = new Renderer(IndexMesh::generateRectangle(width, height));
	addComponent(rend);
	m_name = "Hierba";

	texture = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Hierba::render(glm::dmat4 const& viewMat)
{
	// Quitar el culling
	GLboolean cullActivado;
	glGetBooleanv(GL_CULL_FACE, &cullActivado);
	if(cullActivado)
		glDisable(GL_CULL_FACE);

	// Activar la textura si la tiene
	texture->bind();

	// Pintar los haces
	if (m_renderer != nullptr)
	{
		// En una dirección
		glMatrixMode(GL_MODELVIEW);
		glm::dmat4 modelViewMat = viewMat * modelMat;
		glLoadMatrixd(value_ptr(modelViewMat));

		m_material.load();
		m_renderer->draw();

		// En la otra
		glMatrixMode(GL_MODELVIEW);
		modelViewMat = glm::rotate(modelViewMat, PI/2, { 0, 1, 0 });
		glLoadMatrixd(value_ptr(modelViewMat));

		m_material.load();
		m_renderer->draw();
	}

	// Desactivar la textura si la tiene
	texture->unbind();

	if(cullActivado)
		glEnable(GL_CULL_FACE);
}

// - - - - - - - - - - - - - - - - - 


ClippableEntity::ClippableEntity()
{
	Renderer* rend = new Renderer(IndexMesh::generateToro(2.5, 1.25, 20, 6));
	addComponent(rend);
	setShader("clippable");

	// 3 planos de corte de prueba
	planos.push_back({ 0, -1, 0, 1 }); // -y + 1 = 0
	planos.push_back({ 1, 0, 0, 0 }); // x = 0
	planos.push_back({ 0.71, 0.71, 0, -0.71 }); // x + y - 1 = 0 (es equivalente)
}

void ClippableEntity::render()
{
	// Activar y mandar los planos de corte al shader
	for (int i = 0; i < planos.size(); i++)
	{
		glEnable(GL_CLIP_DISTANCE0 + i);
		m_shader->setVec4("planoCorte[" + std::to_string(i) + "]", planos[i]);
	}

	Entity::render();

	// Dejarlo todo como estaba
	// Si no los desactivamos, hay errores con el resto de shaders
	for (int i = 0; i < planos.size(); i++)
		glDisable(GL_CLIP_DISTANCE0 + i);
}

// - - - - - - - - - - - - - - - - - 

TessTerrain::TessTerrain(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	m_name = "TessTerrain";
	Renderer* rend = new Renderer(IndexMesh::generateTessGrid(filas, columnas, tamFila, tamColumna));
	addComponent(rend);

	setShader("terreno");
	useEyedir = false;
	patchSize = tamFila * 2;
	this->elevacion = 1.0f;
}

void TessTerrain::setHeightMap(const std::string& texID, float elevacion)
{
	setDisplacementMap(texID);
	this->elevacion = elevacion;
}

void TessTerrain::render()
{
	m_shader->setInt("use_eyeDir", useEyedir);
	m_shader->setInt("patch_size", patchSize);
	m_shader->setVec3("camFW", cam->forward());
	m_shader->setFloat("elevacion", elevacion);
	Entity::render();
}

// - - - - - - - - - - - - - - - - - 

#include "Entity.h"

#include <iostream>

#include <gtc/type_ptr.hpp>

#include "Utils.h"
#include "ResourceManager.h"
#include "Mesh.h"
#include "Texture.h"
#include "MeshLoader.h"
#include "Shader.h"
#include "Pixmap32RGBA.h"
#include "Component.h"

#include <freeglut.h>

using namespace glm;

Entity::Entity()
{
	// Valores por defecto de la entidad
	defaultValues();

	m_name = "- Entidad s/n - ";
	
	//PrintMatrix<double, 4>(&modelMat);
	//NOMBRE(modelMat);
}

Entity::Entity(const std::string& name)
{
	defaultValues();
	m_name = name;
}

Entity::~Entity()
{
	delete m_mesh;

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
	m_mesh = nullptr;
	for (int i = 0; i < NUM_TEXTURES; i++)
		m_textures[i] = nullptr;
	// Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	modelMat = (1.0);
	m_shader = nullptr;

	m_polyModeFront = GL_FILL;
	m_polyModeBack = GL_LINE;
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
}

void Entity::render(glm::dmat4 const& viewMat)
{
	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	// 1) Renderizar la propia entidad
	// Activar la textura si la tiene
	if (m_textures[0] != nullptr)
		m_textures[0]->bind();

	// 1) Cargar la matriz V*M
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz VM
	glm::dmat4 modelViewMat = viewMat * modelMat;
	glLoadMatrixd(value_ptr(modelViewMat));

	// 2) Dibujar la/s malla/s
	m_material.load();
	if (m_mesh != nullptr)
		m_mesh->draw();

	// Desactivar la textura si la tiene
	if (m_textures[0] != nullptr)
		m_textures[0]->unbind();


	// 2) Renderizar sus hijos
	for(Entity* e : m_children)
		if(e->isActive())
			e->render(modelViewMat);
}

void Entity::render()
{
	glPolygonMode(GL_FRONT, m_polyModeFront);
	glPolygonMode(GL_BACK, m_polyModeBack);

	// 1) Renderizar la propia entidad
	// Enviar las texturas necesarias al shader
	bindTextures();

	// 2) Dibujar la/s malla/s
	if (m_mesh != nullptr)
		m_mesh->draw();

	// Desactivar texturas
	unbindTextures();

	// 2) Renderizar sus hijos con el mismo shader dado
	for (Entity* e : m_children)
		if (e->isActive())
			e->render();
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
}

void Entity::setMesh(const std::string& meshID)
{
	m_mesh = (Mesh*) & ResourceManager::Instance()->getMesh(meshID);
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
	m_textures[0] = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setSecondTex(const std::string& textureID)
{
	m_textures[1] = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setSpecularMap(const std::string& textureID)
{
	m_textures[2] = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setNormalMap(const std::string& textureID)
{
	m_textures[3] = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setDisplacementMap(const std::string& textureID)
{
	m_textures[4] = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setPolygonMode(GLenum front, GLenum back)
{
	m_polyModeFront = front;
	m_polyModeBack = back;
}

void Entity::bindTextures()
{
	if (m_shader == nullptr)
		return;
	// Nombre que deben tener las respectivas variables 'sampler2D' del shader
	std::string texNames[NUM_TEXTURES] = { "textura", "textura2", "material.specular_map", "normalMap", "dispMap"};

	// Activar cada textura existente
	for(int i = 0; i < NUM_TEXTURES; i++)
	{
		if(m_textures[i] != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			m_textures[i]->bind();
			m_shader->setInt(texNames[i], i);
		}
	}

	// Booleanos extra
	if(m_textures[2] != nullptr)
		m_shader->setInt("use_spec_map", true);
	else 
		m_shader->setInt("use_spec_map", false);

	if (m_textures[3] != nullptr)
		m_shader->setInt("use_normal_map", true);
	else
		m_shader->setInt("use_normal_map", false);
}

void Entity::unbindTextures()
{
	// El método Texture::unbind() debería ser estático, solo hay que llamarlo 1 vez
	if (m_textures[0] != nullptr)
	{
		m_textures[0]->unbind();
		glActiveTexture(GL_TEXTURE0);
	}
}


// - - - - - - - - - - - - - - - - - 

EjesRGB::EjesRGB(GLdouble l)
{
	m_mesh = Mesh::generateAxesRGB(l);
	m_name = "EjesRGB";
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, GLdouble size, bool relleno)
{
	if(relleno)
		m_mesh = Mesh::generateFilledPolygon(sides, size);
	else
		m_mesh = Mesh::generatePolygon(sides, size);
	m_name = "Poligono";
}


// - - - - - - - - - - - - - - - - - 

Cubo::Cubo(GLdouble size, bool equalFaces)
{
	m_mesh = IndexMesh::generateCube(size, equalFaces);
	m_name = "Cubo";
}


// - - - - - - - - - - - - - - - - - 

Esfera::Esfera(GLdouble radio, GLuint paralelos, GLuint meridianos)
{
	m_mesh = IndexMesh::generateSphere(radio, paralelos, meridianos);
	m_name = "Esfera";
}

// - - - - - - - - - - - - - - - - - 

Cilindro::Cilindro(GLdouble radio, GLdouble altura, GLuint lados)
{
	m_mesh = IndexMesh::generateCilindro(radio, altura, lados);
	m_name = "Cilindro";
}


// - - - - - - - - - - - - - - - - - 

Toro::Toro(GLdouble radExt, GLdouble radInt, GLuint anillos, GLuint lineas)
{
	m_mesh = IndexMesh::generateToro(radExt, radInt, anillos, lineas);
	m_name = "Toro";
}

// - - - - - - - - - - - - - - - - - 

Grid::Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	m_mesh = IndexMesh::generateGrid(filas, columnas, tamFila, tamColumna);
	m_name = "Grid";
}

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

Terrain::Terrain()
{
	m_name = "Terrain";
}

void Terrain::loadRAW(const std::string& rawFile, GLdouble scale)
{
	m_mesh = IndexMesh::generateTerrain(rawFile, scale, true);
}

void Terrain::loadHeightMap(const std::string& heightMap, GLdouble scale)
{
	m_mesh = IndexMesh::generateTerrain(heightMap, scale, false);
}

// - - - - - - - - - - - - - - - - - 

Skybox::Skybox(const std::string& cubemapTextureID)
{
	// Generar la malla y cargar la textura y el shader
	m_mesh = IndexMesh::generateCubemap(4.0);
	m_name = "Skybox_" + cubemapTextureID;

	// Las caras están puestas para que miren hacia dentro del cubo
	setTexture(cubemapTextureID);
	setShader("skybox");
}

void Skybox::render()
{
	// No escribimos en el Z-buffer para no interferir con otras entidades
	glDepthMask(GL_FALSE);
	Entity::render();
	glDepthMask(GL_TRUE);
}

// - - - - - - - - - - - - - - - - -

Billboard::Billboard(const std::string& textureID, GLfloat width, GLfloat height)
{
	m_mesh = IndexMesh::generateRectangle(width, height);
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

CuboMultitex::CuboMultitex(GLdouble size)
{
	m_mesh = IndexMesh::generateCube(size, true);
	m_name = "CuboMT";

	setShader("multitexture");
}


void CuboMultitex::render()
{
	m_shader->setFloat("mix", (sin(glutGet(GLUT_ELAPSED_TIME) * 0.001) + 1) / 2.0f);
	Entity::render();
}

// - - - - - - - - - - - - - - - - - 

CuboSpecmap::CuboSpecmap(GLdouble size) 
{
	m_mesh = IndexMesh::generateCube(size, true);
	m_name = "CuboSpecmap";

	setShader("lights");
}

// - - - - - - - - - - - - - - - - - 

Hierba::Hierba(GLdouble width, GLdouble height)
{
	m_mesh = IndexMesh::generateRectangle(width, height);
	m_name = "Hierba";
}

void Hierba::render(glm::dmat4 const& viewMat)
{
	// Quitar el culling
	GLboolean cullActivado;
	glGetBooleanv(GL_CULL_FACE, &cullActivado);
	if(cullActivado)
		glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Activar la textura si la tiene
	if (m_textures[0] != nullptr)
		m_textures[0]->bind();

	// Pintar los haces
	if (m_mesh != nullptr)
	{
		// En una dirección
		glMatrixMode(GL_MODELVIEW);
		glm::dmat4 modelViewMat = viewMat * modelMat;
		glLoadMatrixd(value_ptr(modelViewMat));

		m_material.load();
		m_mesh->draw();

		// En la otra
		glMatrixMode(GL_MODELVIEW);
		modelViewMat = glm::rotate(modelViewMat, PI/2, { 0, 1, 0 });
		glLoadMatrixd(value_ptr(modelViewMat));

		m_material.load();
		m_mesh->draw();
	}

	// Desactivar la textura si la tiene
	if (m_textures[0] != nullptr)
		m_textures[0]->unbind();

	if(cullActivado)
		glEnable(GL_CULL_FACE);
}

// - - - - - - - - - - - - - - - - - 

NormalMapWall::NormalMapWall(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	IndexMesh* m = IndexMesh::generateGrid(filas, columnas, tamFila, tamColumna);
	m->setTangents();
	m_mesh = m;
	m_name = "Wall";

	setShader("lights");
}


// - - - - - - - - - - - - - - - - - 

ClippableEntity::ClippableEntity()
{
	m_mesh = IndexMesh::generateToro(2.5, 1.25, 20, 6);
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
	m_mesh = IndexMesh::generateTessGrid(filas, columnas, tamFila, tamColumna);
	// Para depurarlo
	setPolygonMode(GL_FILL, GL_LINE);

	setShader("terreno");
}

void TessTerrain::render()
{
	m_shader->setInt("subdivisions", subdivisiones);
	Entity::render();
}

void TessTerrain::setSubdivisions(int sub)
{
	subdivisiones = sub;
}

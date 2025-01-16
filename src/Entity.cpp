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
}

void Entity::defaultValues()
{
	m_active = true;
	m_parent = nullptr;
	m_mesh = nullptr;
	m_texture = nullptr;
	// Pone la matriz de modelado a la matriz identidad de grado 4 (1 0 0 0 / 0 1 0 0 ...)
	modelMat = (1.0);
	m_shader = nullptr;
	m_specMap = nullptr;
}

void Entity::render(glm::dmat4 const& viewMat)
{
	// 1) Renderizar la propia entidad
	
	// Para materiales translúcidos
	if (m_material.isTranslucid())
	{
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glDepthMask(GL_FALSE);
	}

	// Activar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->bind();

	// 1) Cargar la matriz V*M (esto NO hace falta cuando se usan shaders)
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz VM (vista * modelado)
	glm::dmat4 modelViewMat = viewMat * modelMat;
	// Cargarla 
	glLoadMatrixd(value_ptr(modelViewMat));
	//glLoadMatrixd((GLdouble*)&modelViewMat); //equivalente a hacer esto


	// 2) Dibujar la/s malla/s
	m_material.load();
	if (m_mesh != nullptr)
		m_mesh->draw();

	// Desactivar la textura si la tiene
	if (m_texture != nullptr)
		m_texture->unbind();

	//
	if (m_material.isTranslucid())
	{
		//glDepthMask(GL_TRUE);
	}

	// 2) Renderizar sus hijos
	for(Entity* e : m_children)
	{
		if(e->isActive())
		{
			e->render(modelViewMat);
		}
	}
}

void Entity::update(GLuint deltaTime)
{
	// Actualizar los hijos
	for(Entity* e : m_children)
	{
		if(e->isActive())
		{
			e->update(deltaTime);
		}
	}
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

void Entity::setTexture(const std::string& textureID)
{
	m_texture = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

void Entity::setMaterial(const std::string& materialID)
{
	m_material = ResourceManager::Instance()->getMaterial(materialID);
}

void Entity::setShader(const std::string& shaderID)
{
	m_shader = (Shader*)&ResourceManager::Instance()->getShader(shaderID);
}

void Entity::setSpecularMap(const std::string& textureID)
{
	m_specMap = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
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

Cubo::Cubo(GLdouble size, bool textured, bool equalFaces)
{
	m_mesh = IndexMesh::generateCube(size, textured, equalFaces);
	m_name = "Cubo";
}


void Cubo::update(GLuint deltaTime)
{
	//rotate(deltaTime * 0.001, { 0,1,0 }, LOCAL);
}


// - - - - - - - - - - - - - - - - - 

Esfera::Esfera(GLdouble size, GLuint subdivisions, bool textured)
{
	m_mesh = IndexMesh::generateSphere(size, subdivisions, textured);
	m_name = "Esfera";
}

void Esfera::update(GLuint timeElapsed)
{
	rotate(timeElapsed * 0.0005, { 0,1,0 }, LOCAL);
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

void Grid::update(GLuint timeElapsed)
{

}

void Grid::render(glm::dmat4 const& viewMat)
{
	// Pasarle el tiempo al fragment shader
	if (m_shader != nullptr)
	{
		float t = glutGet(GLUT_ELAPSED_TIME);
		m_shader->setFloat("tiempo", t / 10000.0f);
	}

	Entity::render(viewMat);
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

	setTexture(cubemapTextureID);
	setShader("skybox");
}

void Skybox::render()
{
	// No escribimos nada en el Z-buffer para que se pinte todo al fondo
	glDepthMask(GL_FALSE);

	// Definir la forma de pintar la malla
	glPolygonMode(GL_BACK, GL_FILL);

	// Activar la textura de tipo CUBE_MAP
	m_texture->bind();

	// 2) Dibujar la/s malla/s
	m_material.load();
	if (m_mesh != nullptr)
		m_mesh->draw();

	// Desactivar la textura
	m_texture->unbind();

	// Dejarlo todo como estaba
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_TRUE);
}

// - - - - - - - - - - - - - - - - - 

CuboMultitex::CuboMultitex(GLdouble size) : secondTex(nullptr)
{
	m_mesh = IndexMesh::generateCube(size, true, true);
	m_name = "CuboMT";

	setShader("multitexture");
}


void CuboMultitex::render(glm::dmat4 const& viewMat)
{
	glActiveTexture(GL_TEXTURE0);
	m_texture->bind();
	glActiveTexture(GL_TEXTURE1);
	secondTex->bind();

	// Pasar las texturas al shader
	m_shader->setInt("texture1", 0); //este incluso sobraría, porque se manda automático
	m_shader->setInt("texture2", 1);
	m_shader->setFloat("mix", (sin(glutGet(GLUT_ELAPSED_TIME) * 0.001) + 1) / 2.0f);

	// Dibujar la entidad
	m_material.load();
	if (m_mesh != nullptr)
		m_mesh->draw();

	secondTex->unbind();
	glActiveTexture(GL_TEXTURE0);

	m_texture->unbind();
}

void CuboMultitex::setSecondTex(const std::string& textureID)
{
	this->secondTex = (Texture*)&ResourceManager::Instance()->getTexture(textureID);
}

// - - - - - - - - - - - - - - - - - 

CuboSpecmap::CuboSpecmap(GLdouble size) 
{
	m_mesh = IndexMesh::generateCube(size, true, true);
	m_name = "CuboSpecmap";

	setShader("specMap");
}


void CuboSpecmap::render(glm::dmat4 const& viewMat)
{
	glActiveTexture(GL_TEXTURE0);
	m_texture->bind();
	glActiveTexture(GL_TEXTURE1);
	m_specMap->bind();

	// Pasar las texturas al shader
	m_shader->setInt("textura", 0); //este incluso sobraría, porque se manda automático
	m_shader->setInt("specMap", 1);

	// Dibujar la entidad
	if (m_mesh != nullptr)
		m_mesh->draw();

	m_specMap->unbind();
	glActiveTexture(GL_TEXTURE0);

	m_texture->unbind();
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
	if (m_texture != nullptr)
		m_texture->bind();

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
	if (m_texture != nullptr)
		m_texture->unbind();

	// Dejarlo como estaba
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	if(cullActivado)
		glEnable(GL_CULL_FACE);
}

void Hierba::update(GLuint timeElapsed)
{
	rotate(timeElapsed / 1000.0f * 0.5, { 0, 1, 0 });
}


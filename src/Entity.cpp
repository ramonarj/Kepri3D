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

Entity::~Entity()
{
	delete m_mesh;

	// Destruir los hijos
	for (Entity* e : m_children)
		delete e;
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

	// Definir la forma de pintar la malla
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

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


	// Dejarlo como estaba
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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

void Entity::setPosition(glm::dvec3 pos)
{
	// Cambiamos la última columna, que contiene la posición
	modelMat[3][0] = pos.x;
	modelMat[3][1] = pos.y;
	modelMat[3][2] = pos.z;
}

void Entity::translate(glm::dvec3 transVector, ReferenceSystem refSys)
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

void Entity::rotate(GLdouble alpha, glm::dvec3 axis, ReferenceSystem refSys)
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

void Entity::scale(glm::dvec3 scale)
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
}

// - - - - - - - - - - - - - - - - - 

Poligono::Poligono(GLint sides, GLdouble size, bool relleno)
{
	if(relleno)
		m_mesh = Mesh::generateFilledPolygon(sides, size);
	else
		m_mesh = Mesh::generatePolygon(sides, size);
}


// - - - - - - - - - - - - - - - - - 

Cubo::Cubo(GLdouble size, bool textured, bool equalFaces)
{
	m_mesh = IndexMesh::generateCube(size, textured, equalFaces);
}


void Cubo::update(GLuint deltaTime)
{
	//rotate(deltaTime * 0.001, { 0,1,0 }, LOCAL);
}


// - - - - - - - - - - - - - - - - - 

Esfera::Esfera(GLdouble size, GLuint subdivisions, bool textured)
{
	m_mesh = IndexMesh::generateSphere(size, subdivisions, textured);
}

void Esfera::update(GLuint timeElapsed)
{
	rotate(timeElapsed * 0.0005, { 0,1,0 }, LOCAL);
}

// - - - - - - - - - - - - - - - - - 

Grid::Grid(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	m_mesh = IndexMesh::generateGrid(filas, columnas, tamFila, tamColumna);
}

void Grid::update(GLuint timeElapsed)
{
	// Pasarle el tiempo al fragment shader
	if(m_shader != nullptr)
		timeLoc = glGetUniformLocation(m_shader->getId(), "tiempo");
	//glUniform1f(timeLoc, 1.0f);
}

void Grid::render(glm::dmat4 const& viewMat)
{
	// Pasarle el tiempo al fragment shader
	//timeLoc = glGetUniformLocation(m_shader->getId(), "tiempo");
	float t = glutGet(GLUT_ELAPSED_TIME);
	glUniform1f(timeLoc, t / 10000.0f);

	Entity::render(viewMat);
}

// - - - - - - - - - - - - - - - - - 

Terrain::Terrain(std::string filename, GLdouble scale)
{
	m_mesh = IndexMesh::generateTerrain(filename, scale);
}

// - - - - - - - - - - - - - - - - - 

Skybox::Skybox(std::vector<std::string> faces)
{
	// Generar la malla
	m_mesh = IndexMesh::generateCubemap(4.0);

	// Crear la textura de tipo CubeMap
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	// Cargar las 6 texturas de los lados
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		// Cargamos la información del BMP
		PixMap32RGBA pixMap;
		pixMap.load_bmp24BGR(TEXTURES_PATH + "skyboxes/" + faces[i]);
		if (pixMap.is_null())
			std::cout << "No se encontró la textura " << faces[i] << std::endl;

		// Dimensiones
		GLuint w = pixMap.width();
		GLuint h = pixMap.height();

		// Crear la textura (tiene que ser RGBA por el formato del PixMap)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, 
			w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixMap.data());
		
		// Parámetros de escalado y repetición
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// IMPORTANTE: la tercera dimensión
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	
}

void Skybox::render()
{
	// No escribimos nada en el Z-buffer para que se pinte todo al fondo
	glDepthMask(GL_FALSE);

	// Definir la forma de pintar la malla
	glPolygonMode(GL_BACK, GL_FILL);

	// Activar la textura de tipo CUBE_MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); //REPLACE sería lo correcto

	// 2) Dibujar la/s malla/s
	m_material.load();
	if (m_mesh != nullptr)
		m_mesh->draw();

	// Desactivar la textura
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	// Dejarlo todo como estaba
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthMask(GL_TRUE);
}

// - - - - - - - - - - - - - - - - - 

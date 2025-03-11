#include "Entity.h"

#include <gtc/type_ptr.hpp>

#include "Kepri3D.h"

#include <iostream>

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

	// Comprobar si es un Rigidbody (añadir a la simulación)
	Rigid* rigid = dynamic_cast<Rigid*>(c);
	if (rigid != nullptr) { PhysicsSystem::Instance()->addRigid(rigid); }
}

void Entity::render(glm::dmat4 const& viewMat)
{
	// 1) Cargar la matriz V*M
	// Decirle a OpenGL que la siguiente matriz que cargaremos es de modelado/vista (no de proyección)
	glMatrixMode(GL_MODELVIEW);
	// Calcular la matriz VM
	glm::dmat4 modelViewMat = viewMat * modelMat;
	glLoadMatrixd(value_ptr(modelViewMat));

	// 2) Cargar el material y dibujar la malla
	m_material.load();
	if (m_renderer != nullptr)
		m_renderer->draw();

	// 2) Renderizar sus hijos
	for(Entity* e : m_children)
		if(e->isActive())
			e->render(modelViewMat);
}

void Entity::render()
{
	render(m_material.getShader());
}

void Entity::render(Shader* sh)
{
	// 1) Renderizar la propia entidad
	if (m_renderer != nullptr)
	{
		// Cargar el material
		m_material.loadToShader(sh);

		// Dibujar la/s malla/s
		sendUniforms(sh);
		m_renderer->draw();
	}

	// Debug del collider
	if (m_collider != nullptr) { m_collider->render(); }

	// 2) Renderizar sus hijos con el mismo shader dado
	for (Entity* e : m_children)
		if (e->isActive())
			e->render();
}

void Entity::sendUniforms(Shader* sh)
{
	if (sh == nullptr)
		return;

	// Deshacer la posición de los hijos relativa al padre
	glm::dmat4 model = modelMat;
	Entity* parent = m_parent;
	while (parent != nullptr)
	{
		model = parent->modelMat * model;
		parent = parent->m_parent;
	}

	sh->setMat4d("model", model);
	// Sombras
	sh->setInt("receive_shadows", m_renderer->receiveShadows());
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
	if (alpha == 0) { return; }
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
	m_material.setShader((Shader*)m_parent->getShader());
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
	m_material.setShader((Shader*)&ResourceManager::Instance()->getShader(shaderID));
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
	// Temporal
	static_cast<IndexMesh*>(m_renderer->getMesh())->setTangents();
}

void Entity::enableReflections(const std::string& reflectionMapID, const std::string& cubemapID)
{
	m_material.setTexture(5, (Texture*)&ResourceManager::Instance()->getTexture(reflectionMapID));
	m_material.setTexture(6, (Texture*)&ResourceManager::Instance()->getTexture(cubemapID));
}

void Entity::setEmissionMap(const std::string& textureID)
{
	m_material.setTexture(7, (Texture*)&ResourceManager::Instance()->getTexture(textureID));
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

TessTerrain::TessTerrain(GLuint filas, GLuint columnas, GLdouble tamFila, GLdouble tamColumna)
{
	m_name = "TessTerrain";
	Renderer* rend = new Renderer(IndexMesh::generateTessGrid(filas, columnas, tamFila, tamColumna));
	addComponent(rend);

	setShader("terreno");
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
	m_material.getShader()->setVec2("ancla", ancla);
	Entity::render();
}

// - - - - - - - - - - - - - - - - - 

Hierba::Hierba(GLdouble width, GLdouble height)
{
	Renderer* rend = new Renderer(IndexMesh::generateRectangle(width, height));
	addComponent(rend);
	m_name = "Hierba";
}

void Hierba::render(glm::dmat4 const& viewMat)
{
	// Quitar el culling
	GLboolean cullActivado;
	glGetBooleanv(GL_CULL_FACE, &cullActivado);
	if(cullActivado)
		glDisable(GL_CULL_FACE);

	// Pintar los haces
	if (m_renderer != nullptr)
	{
		glMatrixMode(GL_MODELVIEW);

		// En una dirección
		glm::dmat4 modelViewMat = viewMat * modelMat;
		glLoadMatrixd(value_ptr(modelViewMat));

		m_material.load();
		m_renderer->draw();

		// En la otra
		modelViewMat = glm::rotate(modelViewMat, PI/2, { 0, 1, 0 });
		glLoadMatrixd(value_ptr(modelViewMat));

		m_material.load();
		m_renderer->draw();
	}

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
		m_material.getShader()->setVec4("planoCorte[" + std::to_string(i) + "]", planos[i]);
	}

	Entity::render();

	// Dejarlo todo como estaba
	// Si no los desactivamos, hay errores con el resto de shaders
	for (int i = 0; i < planos.size(); i++)
		glDisable(GL_CLIP_DISTANCE0 + i);
}

// - - - - - - - - - - - - - - - - - 

VBOEntity::VBOEntity()
{
	Renderer* rend = new Renderer(Mesh::generateFilledPolygon(6, 2.0));
	addComponent(rend);
	m_name = "VBOEntity";

	type = rend->getMesh()->getType();
	numVerts = rend->getMesh()->getVerticesNum();
	vbo = new Vertexbuffer((void*)rend->getMesh()->getVertices(), numVerts);
}

void VBOEntity::render()
{
	// Cargar el material
	m_material.loadToShader(m_material.getShader());
	sendUniforms(m_material.getShader());
	
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Activar VBO
	vbo->bind();

	// Prueba: actualizar el buffer con la nueva posición de los vértices
	if(glutGet(GLUT_ELAPSED_TIME) % 1000 < 10)
	{
		int posZ = std::rand() % 10;
		((double*)m_renderer->getMesh()->getVertices())[2] = posZ;
		vbo->updateData((void*)m_renderer->getMesh()->getVertices());
	}

	// Esto de momento hay que llamarlo, porque en el resto de entidades, se cambia el estado del atributo 0
	// (con el último argumento a 'vertices') en vez de activando el VBO correspondiente
	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	// Dibujar
	glDrawArrays(type, 0, numVerts);

	// Desactivar VBO
	vbo->unbind();
}

// - - - - - - - - - - - - - - - - - 

EBOEntity::EBOEntity()
{
	Renderer* rend = new Renderer(IndexMesh::generateToro(1.5, 0.75, 15, 8));
	addComponent(rend);
	m_name = "EBOEntity";

	type = rend->getMesh()->getType();
	if (type != GL_TRIANGLES) { std::cout << "Quieto parao" << std::endl; }
	numVerts = rend->getMesh()->getVerticesNum();
	numIndices = static_cast<IndexMesh*>(rend->getMesh())->getIndicesNum();

	vbo = new Vertexbuffer((void*)rend->getMesh()->getVertices(), numVerts);
	ebo = new Elementbuffer((void*)static_cast<IndexMesh*>(rend->getMesh())->getIndices(), numIndices);
}

void EBOEntity::render()
{
	// Cargar el material
	m_material.loadToShader(m_material.getShader());
	sendUniforms(m_material.getShader());

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Activar VBO y EBO
	vbo->bind();
	ebo->bind();

	glVertexAttribPointer(0, 3, GL_DOUBLE, GL_FALSE, 0, 0);

	// Dibujar triángulos indexados
	glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);

	// Desactivar VBO
	vbo->unbind();
	ebo->unbind();
}

// - - - - - - - - - - - - - - - - - 

VAOEntity::VAOEntity()
{
	Renderer* rend = new Renderer(IndexMesh::generateSphere(2, 20, 20));
	addComponent(rend);
	m_name = "VAOEntity";

	type = rend->getMesh()->getType();
	if (type != GL_TRIANGLES) { std::cout << "Quieto parao" << std::endl; }
	numVerts = rend->getMesh()->getVerticesNum();
	numIndices = static_cast<IndexMesh*>(rend->getMesh())->getIndicesNum();

	// Crear el VAO y, mientras está enlazado, crear los VBO (y el EBO) que incluirá
	vao = new VertexArray();
	vao->bind();
	Vertexbuffer vboVertices((void*)rend->getMesh()->getVertices(), numVerts);
	Vertexbuffer vboNormales((void*)rend->getMesh()->getNormals(), numVerts, 3);
	Elementbuffer ebo((void*)static_cast<IndexMesh*>(rend->getMesh())->getIndices(), numIndices);
	vao->unbind();
}

void VAOEntity::render()
{
	// Cargar el material
	m_material.loadToShader(m_material.getShader());
	sendUniforms(m_material.getShader());

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	// Activar VAO solamente (no es necesaria la llamada a 'glVertexAttribPointer' porque el VAO guarda el estado)
	vao->bind();

	// Dibujar triángulos indexados
	glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);

	// Desactivar VAO
	vao->unbind();
}
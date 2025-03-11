#include "Light.h"

#include "BufferObjects.h"
#include "ResourceManager.h"

#include <gtc/type_ptr.hpp>
#include "Shader.h"

GLuint Light::cont = 0;

Shadowmap::Shadowmap(Shader* shader, unsigned int width, unsigned int height,
	float nearPlane, float farPlane, bool omnidirectional)
{
	this->shader = shader;
	this->width = width;
	this->height = height;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;

	this->depthBuf = Framebuffer::createShadowMap(width, height, omnidirectional);
}

Shadowmap::~Shadowmap()
{
	if (depthBuf != nullptr)
		delete depthBuf;
}

// - - - - - - - - - - - - - - - - - - 

Light::Light(LightType type, glm::fvec4 diffuse) : id(GL_MAX_LIGHTS), m_active(true), direction({0,0,0})
{
	this->type = type;

	// Valores por defecto para la atenuación de la luz
	this->constantAtt = 1;
	this->linearAtt = 0;
	this->quadraticAtt = 0;

	// Valor por defecto para focos (hacia abajo)
	this->spotDir = { 0,-1,0 };
	this->spotCutoff = 180.0f;
	this->spotExp = 0.0f;

	// Asegurarnos de que cabe otra luz más
	if(cont < GL_MAX_LIGHTS)
	{
		// El id tiene que empezar a partir de LIGHT0 (0x4000)
		id = GL_LIGHT0 + cont;
		glEnable(id); // es como hacer glEnable(GL_LIGHTid)
		cont++;

		this->ambient = { 0,0,0,1 };
		this->diffuse = diffuse;
		this->specular = { 1,1,1,1 };
	}

	// Crear el mapa de sombras
	Framebuffer* shadowFB; Shader* shadowSh;
	if (type == DIRECTIONAL_LIGHT)
	{
		shadowSh = (Shader*)&ResourceManager::Instance()->getShader("shadows");
		shadowSh->useTextures(false); // para no pasar ni una textura de más (que no se usarían)
		m_shadowMap = new Shadowmap(shadowSh, SHADOW_SIZE, SHADOW_SIZE, 1.0f, 150.0f, false);

		float ortoSize = 40.0f;
		m_shadowMap->lightProj = glm::ortho(-ortoSize, ortoSize, -ortoSize, ortoSize,
			m_shadowMap->nearPlane, m_shadowMap->farPlane);
	}
	else if (type == POINT_LIGHT)
	{
		shadowSh = (Shader*)&ResourceManager::Instance()->getShader("shadows_point");
		shadowSh->useTextures(false);
		m_shadowMap = new Shadowmap(shadowSh, SHADOW_SIZE, SHADOW_SIZE, 1.0f, 50.0f, true);
	}
	else
		m_shadowMap = nullptr;
}

Light::~Light()
{
	setActive(false);
	if(m_shadowMap != nullptr)
		delete m_shadowMap;
	cont--;
}

void Light::update(GLuint deltaTime)
{
}

void Light::load(glm::dmat4 viewMat)
{
	// La luz tiene que moverse a la inversa de la cámara igual que las entidades
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(value_ptr(viewMat));

	// Posición /dirección de la luz
	if(type != DIRECTIONAL_LIGHT)
		glLightfv(id, GL_POSITION, value_ptr(glm::vec4(entity->getPosition(), 1.0)));
	else
		glLightfv(id, GL_POSITION, value_ptr(glm::vec4(direction.x, direction.y, direction.z, 0.0)));
	
	//fv = float vector (puntero a)
	// Valores de la luz
	glLightfv(id, GL_AMBIENT, value_ptr(ambient));
	glLightfv(id, GL_DIFFUSE, value_ptr(diffuse));
	glLightfv(id, GL_SPECULAR, value_ptr(specular));

	// Radio y ángulo (SOLO para focos)
	if (type == SPOT_LIGHT)
	{
		// Valores por defecto: 180º, 0 y (0,0,-1)
		glLightf(id, GL_SPOT_CUTOFF, spotCutoff);
		glLightf(id, GL_SPOT_EXPONENT, spotExp); // entre 0 - 128 (0 = duro)
		glLightfv(id, GL_SPOT_DIRECTION, value_ptr(spotDir));
	}

	// Atenuación de la luz con la distancia (para luces no direccionales)
	if(type != DIRECTIONAL_LIGHT)
	{
		glLightf(id, GL_CONSTANT_ATTENUATION, constantAtt);
		glLightf(id, GL_LINEAR_ATTENUATION, linearAtt);
		glLightf(id, GL_QUADRATIC_ATTENUATION, quadraticAtt);
	}
}

float Light::getAttenuation(GLuint i) const
{
	if (i == 0)
		return constantAtt;
	else if (i == 1)
		return linearAtt;
	else if (i == 2)
		return quadraticAtt;
}

void Light::setActive(bool active)
{
	m_active = active;
	if(id < GL_LIGHT0 + GL_MAX_LIGHTS)
	{
		if(active)
			glEnable(id);
		else
			glDisable(id);
	}
}

void Light::setAttenuationFactors(float constant, float linear, float quadratic)
{
	this->constantAtt = constant;
	this->linearAtt = linear;
	this->quadraticAtt = quadratic;
}

void Light::setSpotlightDir(glm::fvec3 dir)
{
	spotDir = dir;
}

void Light::setSpotlightCutoff(GLfloat angle)
{
	spotCutoff = angle / 2.0f;
}

void Light::setSpotlightHardness(GLfloat hardness)
{
	spotExp = 128.0 - hardness;
}

void Light::emitShadows(bool b)
{
	if (!b && m_shadowMap != nullptr)
	{
		delete m_shadowMap;
		m_shadowMap = nullptr;
	}
		
}

void Light::sendShadowUniforms(Uniformbuffer* uboMatrices, const glm::vec3& camPos)
{
	// Luces direccionales
	if (type != POINT_LIGHT)
	{
		// Actualizar la matriz de vista y ponerla donde la luz
		glm::vec3 origen = camPos;
		m_shadowMap->lightView = glm::lookAt(origen + direction * m_shadowMap->distOrigen, origen, glm::vec3(0.0, 1.0, 0.0));

		// Mandar el uniform
		uboMatrices->bind();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::dmat4), glm::value_ptr(m_shadowMap->lightProj));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::dmat4), sizeof(glm::dmat4), glm::value_ptr(m_shadowMap->lightView));
		Uniformbuffer::unbind();
	}
	// Luces puntuales
	else
	{
		// TODO: no mandar nada si la luz no ha cambiado en el último frame
		glm::vec3 lightPos = entity->getPosition();
		// Actualizar la matriz de vista y ponerla donde la luz
		float aspect = (float)m_shadowMap->width / (float)m_shadowMap->height;
		glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect,
			m_shadowMap->nearPlane, m_shadowMap->farPlane);

		// Matrices de vista (1 por cada cara del cubemap). Der, Izq, Arr, Aba, Fre, Atr
		std::vector<glm::mat4> shadowTransforms(6);
		shadowTransforms[0] = shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		shadowTransforms[1] = shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
		shadowTransforms[2] = shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
		shadowTransforms[3] = shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
		shadowTransforms[4] = shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
		shadowTransforms[5] = shadowProj *
			glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));

		// Mandar los uniforms
		m_shadowMap->shader->use();
		for (int i = 0; i < 6; i++) {
			m_shadowMap->shader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
		}
		m_shadowMap->shader->setVec3("lightPos", lightPos);
		m_shadowMap->shader->setFloat("far_plane", m_shadowMap->farPlane);
	}
}

void Light::loadToUBO(unsigned int offset)
{
	glm::vec3 posDir = (type == DIRECTIONAL_LIGHT) ? direction : (glm::vec3)entity->getPosition();

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
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 76, sizeof(float), &constantAtt);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 80, sizeof(float), &linearAtt);
		glBufferSubData(GL_UNIFORM_BUFFER, offset + 84, sizeof(float), &quadraticAtt);
		// Para focos necesitamos parámetros extra (dirección, apertura y exponente)
		if (type == SPOT_LIGHT)
		{
			glBufferSubData(GL_UNIFORM_BUFFER, offset + 96, sizeof(glm::vec3), glm::value_ptr(spotDir));
			glBufferSubData(GL_UNIFORM_BUFFER, offset + 108, sizeof(float), &spotCutoff);
			glBufferSubData(GL_UNIFORM_BUFFER, offset + 112, sizeof(float), &spotExp);
		}
	}
	// Indicar si la luz está encendida o no
	glBufferSubData(GL_UNIFORM_BUFFER, offset + 116, sizeof(bool), &m_active);
}
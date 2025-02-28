#include "Light.h"

#include "BufferObjects.h"
#include "ResourceManager.h"

#include <gtc/type_ptr.hpp>

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

Shadowmap::~Shadowmap() { clean(); }

// - - - - - - - - - - - - - - - - - - 

Light::Light(LightType type, glm::fvec4 diffuse) : id(GL_MAX_LIGHTS), m_active(true), direction({0,0,0,0})
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
		m_shadowMap = new Shadowmap(shadowSh, SHADOW_SIZE, SHADOW_SIZE, 1.0f, 80.0f, false);

		float ortoSize = 40.0f;
		m_shadowMap->lightProj = glm::ortho(-ortoSize, ortoSize, -ortoSize, ortoSize,
			m_shadowMap->nearPlane, m_shadowMap->farPlane);
	}
	else if (type == POINT_LIGHT)
	{
		shadowSh = (Shader*)&ResourceManager::Instance()->getShader("shadows_point");
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
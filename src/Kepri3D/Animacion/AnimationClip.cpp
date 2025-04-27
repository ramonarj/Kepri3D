#include "AnimationClip.h"

AnimationClip::AnimationClip(const std::string& name, float duracion, bool loop) :
	name(name), duracion(duracion), loop(loop)
{
	this->paused = false;
	this->tiempo = 0;
}

void AnimationClip::update(float deltaTime)
{
	if (paused) { return; }

	// Actualizar las AnimatedProperties de tipo float
	for (int i = 0; i < m_floats.size(); i++)
		m_floats[i].animationStep(deltaTime);
	for (int i = 0; i < m_doubles.size(); i++)
		m_doubles[i].animationStep(deltaTime);
	for (int i = 0; i < m_dvector3s.size(); i++)
		m_dvector3s[i].animationStep(deltaTime);
	for (int i = 0; i < m_vector4s.size(); i++)
		m_vector4s[i].animationStep(deltaTime);

	tiempo += deltaTime;

	// Terminar la animación
	if (tiempo > duracion)
	{
		endAnimation();
	}
}

void AnimationClip::endAnimation()
{
	// Volver a empezar
	if (loop)
	{
		for (int i = 0; i < m_floats.size(); i++)
			*(m_floats[i].parametro) = m_floats[i].keyframes[0].valor;
		for (int i = 0; i < m_dvector3s.size(); i++)
			*(m_dvector3s[i].parametro) = m_dvector3s[i].keyframes[0].valor;
		for (int i = 0; i < m_vector4s.size(); i++)
			*(m_vector4s[i].parametro) = m_vector4s[i].keyframes[0].valor;
	}
	// Acabar del todo
	else
	{
		for (int i = 0; i < m_floats.size(); i++)
			*(m_floats[i].parametro) = m_floats[i].keyframes[m_floats[i].frameActual + 1].valor;
		for (int i = 0; i < m_dvector3s.size(); i++)
			*(m_dvector3s[i].parametro) = m_dvector3s[i].keyframes[m_dvector3s[i].frameActual + 1].valor;
		for (int i = 0; i < m_vector4s.size(); i++)
			*(m_vector4s[i].parametro) = m_vector4s[i].keyframes[m_vector4s[i].frameActual + 1].valor;
		//setActive(false); TODO
	}
	tiempo = 0;
	for (int i = 0; i < m_floats.size(); i++)
	{
		m_floats[i].tiempoKey = 0;
		m_floats[i].frameActual = 0;
	}
	for (int i = 0; i < m_doubles.size(); i++)
	{
		m_doubles[i].tiempoKey = 0;
		m_doubles[i].frameActual = 0;
	}
	for (int i = 0; i < m_dvector3s.size(); i++)
	{
		m_dvector3s[i].tiempoKey = 0;
		m_dvector3s[i].frameActual = 0;
	}
	for (int i = 0; i < m_vector4s.size(); i++)
	{
		m_vector4s[i].tiempoKey = 0;
		m_vector4s[i].frameActual = 0;
	}
}

void AnimationClip::play()
{
	paused = false;
}


void AnimationClip::pause()
{
	paused = true;
}


void AnimationClip::restart()
{
	paused = false;
	endAnimation();
}

void AnimationClip::addProperty(const AnimatedProperty<float>& floatProp)
{
	m_floats.push_back(floatProp);
}

void AnimationClip::addProperty(const AnimatedProperty<double>& floatProp)
{
	m_doubles.push_back(floatProp);
}

void AnimationClip::addProperty(const AnimatedProperty<glm::dvec3>& floatProp)
{
	m_dvector3s.push_back(floatProp);
}

void AnimationClip::addProperty(const AnimatedProperty<glm::vec4>& floatProp)
{
	m_vector4s.push_back(floatProp);
}
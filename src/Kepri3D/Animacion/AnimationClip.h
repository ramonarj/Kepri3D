#ifndef __ANIMATION_CLIP__
#define __ANIMATION_CLIP__

#include <string>
#include <vector>
#include "Utils.h"

/* Instantánea en el tiempo a partir de las cuales se interpolan los demás frames*/
template<class T>
struct Keyframe
{
	Keyframe(T valor, float momento)
	{
		this->valor = valor;
		this->momento = momento;
	}

	T valor;
	float momento;
};

/* Cada una de las propiedades que se quiera animar (posición, rotación, escala, color, volumen, etc.) */
template<class T>
struct AnimatedProperty
{
	AnimatedProperty(T* parametro);
	AnimatedProperty(T* parametro, const std::vector<Keyframe<T>>& keyframes);
	AnimatedProperty(T* parametro, T valorInicial, T valorFinal, float duracion);

	// Añadir keyframes
	void addKeyframe(T valor, float momento);

	// 
	void animationStep(float deltaTime);

	// Lista de keyframes
	std::vector<Keyframe<T>> keyframes;
	// Puntero al parámetro
	T* parametro;
	// Índice del keyframe actual
	unsigned int frameActual;
	// Tiempo que lleva ejecutándose el keyframe actual
	float tiempoKey;
};

/* Conjunto de propiedades animadas, engblobadas con un nombre */
class AnimationClip
{
public:
	AnimationClip(const std::string& name, float duracion, bool loop = false);

	// Añadir propiedades animadas
	void addProperty(const AnimatedProperty<float>& floatProp);
	void addProperty(const AnimatedProperty<double>& doubleProp);
	void addProperty(const AnimatedProperty<glm::dvec3>& dvec3Prop);
	void addProperty(const AnimatedProperty<glm::vec4>& float4Prop);

	// Heredadas
	void update(float deltaTime);

	// Controles de reproducción
	void play();
	void pause();
	void restart();

	// - -  Variables públicas - - //
	// Nombre
	std::string name;
	// Duracion de la animacion
	float duracion;
	// Si se ejecuta en bucle o no
	bool loop;
	// ¿Está pausada? 
	bool paused;

private:
	// Listas de propiedades animadas
	std::vector<AnimatedProperty<float>> m_floats;
	std::vector<AnimatedProperty<double>> m_doubles;
	std::vector<AnimatedProperty<glm::dvec3>> m_dvector3s;
	std::vector<AnimatedProperty<glm::vec4>> m_vector4s;
	//std::vector<AnimatedProperty<glm::dvec3>> m_vector3s;
	//std::vector<AnimatedProperty<float>> m_vector4s;
	// Tiempo desde el inicio de la animación
	float tiempo;

	// Métodos privados
	void endAnimation();
};
template<typename T>
AnimatedProperty<T>::AnimatedProperty(T* parametro)
	: parametro(parametro)
{
	this->tiempoKey = 0;
	this->frameActual = 0;
}


template<typename T>
AnimatedProperty<T>::AnimatedProperty(T* parametro, T ini, T fin, float dur)
	: parametro(parametro)
{
	this->tiempoKey = 0;
	this->frameActual = 0;
	this->keyframes.push_back({ ini, 0 });
	this->keyframes.push_back({ fin, dur });
}

template<typename T>
AnimatedProperty<T>::AnimatedProperty(T* parametro, const std::vector<Keyframe<T>>& keyframes)
	: parametro(parametro), keyframes(keyframes)
{
	this->tiempoKey = 0;
	this->frameActual = 0;
}

template<typename T>
void AnimatedProperty<T>::addKeyframe(T valor, float momento)
{
	// Hay que insertarlo en orden
	int i = 0;
	while (i < keyframes.size() && keyframes[i].momento < momento)
		i++;
	keyframes.insert(keyframes.begin() + i, { valor, momento });

	// Ver si se alarga la animación
	//if (momento > duracion) { duracion = momento; }
}

template<typename T>
void AnimatedProperty<T>::animationStep(float deltaTime)
{
	if (frameActual >= keyframes.size() - 1) { return; } // TODO: quitar esto de alguna forma
	//(*parametro) = Kepri::lerp(keyframes[frameActual].valor, keyframes[frameActual + 1].valor,
	//	(tiempoKey / duracion));

	// Interpolar
	*(parametro) = Kepri::lerp(keyframes[frameActual].valor, keyframes[frameActual + 1].valor,
		(tiempoKey / (keyframes[frameActual + 1].momento - keyframes[frameActual].momento)));

	tiempoKey += deltaTime;

	// ¿Siguiente keyframe?
	if (keyframes[frameActual].momento + tiempoKey > keyframes[frameActual + 1].momento)
	{
		frameActual++;
		tiempoKey = 0;
	}
}

#endif

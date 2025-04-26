#ifndef __ANIMACION__
#define __ANIMACION__

#include "Component.h"

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

template<class T>
class Animacion : public Component
{
public:
	Animacion(T* parametro, const std::vector<Keyframe<T>>& keyframes, bool loop = false);
	Animacion(T* parametro, T valorInicial, T valorFinal, float duracion, bool loop = false);

	// Heredadas
	void update(float deltaTime) override;

	// Añadir keyframes
	void addKeyframe(T valor, float momento);

	// Puntero al parámetro
	T* parametro;
	// Duracion de la animacion
	float duracion;
	// Si se ejecuta en bucle o no
	bool loop;

private:
	// Lista de keyframes
	std::vector<Keyframe<T>> keyframes;
	// Índice del keyframe actual
	unsigned int frameActual;
	float tiempo;
	// Tiempo que lleva ejecutándose el keyframe actuals
	float tiempoKey;

	// Métodos privados
	void endAnimation();
};

template<typename T>
Animacion<T>::Animacion(T* parametro, T ini, T fin, float dur, bool loop) 
	: parametro(parametro), duracion(dur), loop(loop)
{
	this->tiempo = 0;
	this->tiempoKey = 0;
	this->frameActual = 0;
	this->keyframes.push_back({ ini, 0 });
	this->keyframes.push_back({ fin, duracion });
}

template<typename T>
Animacion<T>::Animacion(T* parametro, const std::vector<Keyframe<T>>& keyframes, bool loop) 
	: parametro(parametro), keyframes(keyframes), loop(loop)
{

}

template<typename T>
void Animacion<T>::addKeyframe(T valor, float momento)
{
	// Hay que insertarlo en orden
	int i = 0; 
	while (i < keyframes.size() && keyframes[i].momento < momento)
		i++;
	keyframes.insert(keyframes.begin() + i, { valor, momento });

	// Ver si se alarga la animación
	if (momento > duracion) { duracion = momento; }
}

template<typename T>
void Animacion<T>::update(float deltaTime)
{
	//(*parametro) = Kepri::lerp(keyframes[frameActual].valor, keyframes[frameActual + 1].valor,
	//	(tiempoKey / duracion));
	(*parametro) = Kepri::lerp(keyframes[frameActual].valor, keyframes[frameActual + 1].valor, 
		(tiempoKey / (keyframes[frameActual + 1].momento - keyframes[frameActual].momento)));
	tiempo += deltaTime;
	tiempoKey += deltaTime;

	// Terminar la animación
	if (tiempo > duracion)
	{
		endAnimation();
	}

	// Siguiente keyframe
	else if (tiempo > keyframes[frameActual + 1].momento)
	{
		frameActual++;
		tiempoKey = 0;
	}
}

template<typename T>
void Animacion<T>::endAnimation()
{
	// Volver a empezar
	if (loop)
	{
		(*parametro) = keyframes[0].valor;
	}
	// Acabar del todo
	else
	{
		(*parametro) = keyframes[frameActual + 1].valor;
		setActive(false);
	}
	tiempo = 0;
	tiempoKey = 0;
	frameActual = 0;
}

#endif
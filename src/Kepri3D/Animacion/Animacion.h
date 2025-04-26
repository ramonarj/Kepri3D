#ifndef __ANIMACION__
#define __ANIMACION__

#include "Component.h"

template<class T>
class Animacion : public Component
{
public:
	Animacion(T* parametro, T valorInicial, T valorFinal, float duracion, bool loop = false);

	// Heredadas
	void update(float deltaTime) override;

	// Puntero al parámetro
	T* parametro;
	// Valor inicial
	T ini;
	// Valor final
	T fin;
	// Duracion de la animacion
	float duracion;
	// Si se ejecuta en bucle o no
	bool loop;

private:
	float tiempo;
};

template<typename T>
Animacion<T>::Animacion(T* parametro, T ini, T fin, float dur, bool loop) :
	parametro(parametro), ini(ini), fin(fin), duracion(dur), loop(loop)
{
	this->tiempo = 0;
}

template<typename T>
void Animacion<T>::update(float deltaTime)
{
	(*parametro) = Kepri::lerp(ini, fin, (tiempo / duracion));
	tiempo += deltaTime;

	// Terminar la animación
	if (tiempo > duracion)
	{
		// Volver a empezar
		if(loop)
		{
			(*parametro) = ini;
		}
		// Acabar del todo
		else
		{
			(*parametro) = fin;
			setActive(false);
		}
		tiempo = 0;
	}
}

#endif
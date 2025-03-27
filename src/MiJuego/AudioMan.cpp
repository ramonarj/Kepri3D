#include "AudioMan.h"

#include "Kepri3D.h"

AudioMan::AudioMan(AudioSource* src, Audio* altAudio)
{
	this->source = src;
	this->audio1 = source->getAudio();
	this->audio2 = altAudio;
}

void AudioMan::update(float deltaTime)
{
	// Reproducir el sonido (espacio)
	if (InputManager::Instance()->getKeyDown(32))
	{
		source->play();
	}

	// Cambiar el sonido que usa la fuente (tabulador)
	if (InputManager::Instance()->getKeyDown(9))
	{
		if (source->getAudio() == audio1)
			source->setAudio(audio2);
		else
			source->setAudio(audio1);
	}
}

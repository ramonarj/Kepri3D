#include "AudioMan.h"

#include "Kepri3D.h"

AudioMan::AudioMan(AudioSource* src, Audio* altAudio) : pausedMusic(false)
{
	this->source = src;
	this->audio1 = source->getAudio();
	this->audio2 = altAudio;
}

void AudioMan::start()
{
	musicSrc = entity->getComponent<AudioSource>();
}

void AudioMan::update(float deltaTime)
{
	// Reproducir el sonido (espacio)
	if (InputManager::Instance()->getKeyDown(13))
		source->play();

	// Cambiar el sonido que usa la fuente (tabulador)
	if (InputManager::Instance()->getKeyDown(9))
	{
		if (source->getAudio() == audio1)
			source->setAudio(audio2);
		else
			source->setAudio(audio1);
	}

	// Looping
	if (InputManager::Instance()->getKeyDown('l'))
		source->setLoop(true);

	// Subir/bajar el volumen global
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))  {
		AudioManager::Instance()->setGlobalVolume(AudioManager::Instance()->getGlobalVolume() + deltaTime);
	}
	else if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN)) {
		AudioManager::Instance()->setGlobalVolume(AudioManager::Instance()->getGlobalVolume() - deltaTime);
	}

	controlMusica(deltaTime);
}

void AudioMan::controlMusica(float deltaTime)
{	
	// Subir/bajar el volumen
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
	{
		musicSrc->setVolume(musicSrc->getVolume() + deltaTime);
	}
	else if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
	{
		musicSrc->setVolume(musicSrc->getVolume() - deltaTime);
	}

	// Pausar la música
	if (InputManager::Instance()->getKeyDown('m'))
	{
		if (pausedMusic)
			musicSrc->play();
		else
			musicSrc->pause();
		pausedMusic = !pausedMusic;
	}

	// Parar la música
	if (InputManager::Instance()->getKeyDown('n'))
	{
		musicSrc->stop();
		pausedMusic = true;
	}

	// Subir/bajar el pitch
	if (InputManager::Instance()->getKey('9'))
	{
		musicSrc->setPitch(musicSrc->getPitch() - deltaTime);
	}
	else if (InputManager::Instance()->getKey('0'))
	{
		musicSrc->setPitch(musicSrc->getPitch() + deltaTime);
	}
}

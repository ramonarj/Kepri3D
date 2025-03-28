#include "AudioSource.h"

#include "al.h"
#include "alut.h"

#include "Audio.h"
#include "ResourceManager.h"
#include "Camera.h"

AudioSource::AudioSource(Audio* audio) : m_loop(false)
{
	setup(audio);
}

AudioSource::AudioSource(const std::string& audioID) : m_loop(false)
{
	Audio* audio = (Audio*)&ResourceManager::Instance()->getAudio(audioID);
	setup(audio);
}

void AudioSource::setup(Audio* audio)
{
	// Generar la fuente y decirle qué audio usaremos
	alGenSources(1, &sourceId);
	alSourcei(sourceId, AL_BUFFER, audio->bufferId);
	alSourcei(sourceId, AL_LOOPING, m_loop);

	m_audio = audio;
}

void AudioSource::update(float deltaTime)
{
	// Actualizar la posición de la fuente
	//glm::dvec3 pos = entity->getPosition() - Game::Instance()->getCamera()->getPosition();
	glm::dvec3 pos = entity->getPosition();
	alSource3f(sourceId, AL_POSITION, pos.x, pos.y, pos.z);
}

void AudioSource::play()
{
	alSourcePlay(sourceId);
}

void AudioSource::setAudio(Audio* audio)
{
	// Hay que borrar el source ya creado para cambiar el buffer
	alDeleteSources(1, &sourceId);
	setup(audio);
}

void AudioSource::setLoop(bool loop)
{
	m_loop = loop;
	alSourcei(sourceId, AL_LOOPING, m_loop);
}
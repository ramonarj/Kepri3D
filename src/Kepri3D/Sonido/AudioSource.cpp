#include "AudioSource.h"

#include "al.h"
#include "alut.h"

#include "Audio.h"
#include "ResourceManager.h"
#include "Camera.h"

#ifdef __DEBUG_INFO__
unsigned int AudioSource::numSources = 0;
#endif

AudioSource::AudioSource(Audio* audio) : m_loop(false), m_volume(1)
{
	setup(audio);
#ifdef __DEBUG_INFO__
	numSources++;
#endif
}

AudioSource::AudioSource(const std::string& audioID) : m_loop(false), m_volume(1)
{
	Audio* audio = (Audio*)&ResourceManager::Instance()->getAudio(audioID);
	setup(audio);
#ifdef __DEBUG_INFO__
	numSources++;
#endif
}

AudioSource::~AudioSource()
{
	alSourceStop(sourceId);
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

void AudioSource::pause()
{
	alSourcePause(sourceId);
}

void AudioSource::stop()
{
	alSourceStop(sourceId);
}

void AudioSource::setAudio(Audio* audio)
{
	// Hay que borrar el source ya creado para cambiar el buffer
	alDeleteSources(1, &sourceId);
	setup(audio);
}

void AudioSource::setVolume(float vol)
{
	if (vol > 1 || vol < 0) { return; }

	m_volume = vol;
	alSourcef(sourceId, AL_GAIN, m_volume);
}

void AudioSource::setLoop(bool loop)
{
	m_loop = loop;
	alSourcei(sourceId, AL_LOOPING, m_loop);
}
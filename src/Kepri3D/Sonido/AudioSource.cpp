#include "AudioSource.h"

#include "al.h"
#include "alut.h"

#include "Audio.h"
#include "ResourceManager.h"
#include "Camera.h"

#ifdef __DEBUG_INFO__
unsigned int AudioSource::numSources = 0;
#endif

AudioSource::AudioSource(Audio* audio) : m_loop(false), m_volume(1), m_pitch(1)
{
	setup(audio);
#ifdef __DEBUG_INFO__
	numSources++;
#endif
}

AudioSource::AudioSource(const std::string& audioID) : m_loop(false), m_volume(1), m_pitch(1)
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
	// Otros parámetros
	//alSourcef(sourceId, AL_MAX_DISTANCE, 100.0f); // a partir de qué distancia deja de atenuarse
	//alSourcef(sourceId, AL_REFERENCE_DISTANCE, 1.0f); // radio dentro del cual la ganancia no aumenta más
	//alSourcef(sourceId, AL_ROLLOFF_FACTOR, 3.0f); // penddiente de la recta/curva de atenuación

	m_audio = audio;
}

void AudioSource::update(float deltaTime)
{
	alGetSource3f(sourceId, AL_POSITION, &m_vel.x, &m_vel.y, &m_vel.z);
	//glm::dvec3 pos = entity->getPosition() - Game::Instance()->getCamera()->getPosition(); // relativo

	// Actualizar la posición
	glm::dvec3 pos = entity->getPosition();
	alSource3f(sourceId, AL_POSITION, pos.x, pos.y, pos.z);

	// Calcular la velocidad de la fuente
	m_vel = ((glm::vec3)pos - m_vel) / deltaTime;

	// Aplicarla si es necesario
	if (!isnan(m_vel.x)) // && ha variado significativamente?
	{
		alSource3f(sourceId, AL_VELOCITY, m_vel.x, m_vel.y, m_vel.z);
	}
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

void AudioSource::setPitch(float pitch)
{
	if (pitch < 0) { return; } // ¿pitch maximo?

	m_pitch = pitch;
	alSourcef(sourceId, AL_PITCH, m_pitch);
}
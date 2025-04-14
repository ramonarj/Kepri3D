#include "AudioSource.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "alut.h"

#include "Audio.h"
#include "AudioManager.cpp" // TEMPORAL
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
	// Generar la fuente y decirle qu� audio usaremos
	alGenSources(1, &sourceId);
	alSourcei(sourceId, AL_BUFFER, audio->bufferId);
	alSourcei(sourceId, AL_LOOPING, m_loop);
	// Otros par�metros
	//alSourcef(sourceId, AL_MAX_DISTANCE, 100.0f); // a partir de qu� distancia deja de atenuarse
	//alSourcef(sourceId, AL_REFERENCE_DISTANCE, 1.0f); // radio dentro del cual la ganancia no aumenta m�s
	//alSourcef(sourceId, AL_ROLLOFF_FACTOR, 3.0f); // penddiente de la recta/curva de atenuaci�n
	// Efectos
	//configureSends();
	//attachFilter();
	m_audio = audio;
}

void AudioSource::update(float deltaTime)
{
	alGetSource3f(sourceId, AL_POSITION, &m_vel.x, &m_vel.y, &m_vel.z);
	//glm::dvec3 pos = entity->getPosition() - Game::Instance()->getCamera()->getPosition(); // relativo

	// Actualizar la posici�n
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
	if (vol > 10 || vol < 0) { return; }

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
	if (pitch < 0) { return; } // NOTA: el pitch m�ximo que permite OpenAL es de 4.0f

	m_pitch = pitch;
	alSourcef(sourceId, AL_PITCH, m_pitch);
}


void AudioSource::configureSends()
{
	/* Configure Source Auxiliary Effect Slot Sends */

	/* Set Source Send 0 to feed effectSlots[0] without filtering */
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, effectSlots[0], 0, NULL);
	if (alGetError() != AL_NO_ERROR)
		printf("Failed to configure Source Send 0\n");

	/* Set Source Send 1 to feed effectSlots[1] with filter "filters[0]" */
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, effectSlots[1], 1, filters[0]);
	if (alGetError() != AL_NO_ERROR)
		printf("Failed to configure Source Send 1\n");

	/* Disable Send 0 */
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, NULL);
	if (alGetError() != AL_NO_ERROR)
		printf("Failed to disable Source Send 0\n");

	/* Disable Send 1 */
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 1, NULL);
	if (alGetError() != AL_NO_ERROR)
		printf("Failed to disable Source Send 1\n");
}

void AudioSource::attachFilter()
{
	/* Filter 'sourceId', a generated Source */
	alSourcei(sourceId, AL_DIRECT_FILTER, filters[0]);
	if (alGetError() == AL_NO_ERROR)
	{
		/* Remove filter from 'uiSource' */
		alSourcei(sourceId, AL_DIRECT_FILTER, AL_FILTER_NULL);
		if (alGetError() != AL_NO_ERROR)
			printf("Error: could not remove direct filter\n");
	} else printf("Error: could not apply a direct path filter\n");

	/* Filter the Source send 0 from 'sourceId' to */
	/* Auxiliary Effect Slot "effectSlots[0]" using Filter filters[0] */
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, effectSlots[0], 0, filters[0]);
	if (alGetError() == AL_NO_ERROR)
	{
		/* Remove Filter from Source Auxiliary Send */
		alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, effectSlots[0], 0, AL_FILTER_NULL);
		if (alGetError() != AL_NO_ERROR)
			printf("Error: could not remove filter\n");
	} else printf("Error: could not apply aux send filter\n");
}
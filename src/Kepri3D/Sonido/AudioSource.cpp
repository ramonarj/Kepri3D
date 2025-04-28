#include "AudioSource.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "alut.h"

#include "Filter.h"
#include "Audio.h"
#include "AudioManager.h"
#include "ResourceManager.h"
#include "Camera.h"

#ifdef __DEBUG_INFO__
unsigned int AudioSource::numSources = 0;
#endif

AudioSource::AudioSource(Audio* audio) : m_loop(false), m_volume(1), m_pitch(1), directFilter(nullptr)
{
	setup(audio);
#ifdef __DEBUG_INFO__
	numSources++;
#endif
}

AudioSource::AudioSource(const std::string& audioID) : m_loop(false), m_volume(1), m_pitch(1), directFilter(nullptr)
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
	// Filtros
	if (directFilter != nullptr)
		addFilter(directFilter);
	// Efectos

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
	if (pitch < 0) { return; } // NOTA: el pitch máximo que permite OpenAL es de 4.0f

	m_pitch = pitch;
	alSourcef(sourceId, AL_PITCH, m_pitch);
}

void AudioSource::addFilter(Filter* f)
{
	// Añadir un filtro directo a esta fuente de audio
	alSourcei(sourceId, AL_DIRECT_FILTER, f->filterId);
	if (alGetError() != AL_NO_ERROR)
		printf("Error: no se pudo aplicar el filtro directo\n");
	directFilter = f;
}

void AudioSource::removeFilter()
{
	// Quitar el filtro directo que tuviera
	alSourcei(sourceId, AL_DIRECT_FILTER, AL_FILTER_NULL);
	if (alGetError() != AL_NO_ERROR)
		printf("Error: no se pudo quitar el filtro directo\n");
	directFilter = nullptr;
}

void AudioSource::addEffect(Effect* e, unsigned int auxSend)
{
	// Conectar una de las salidas auxiliares de la fuente sin filtrar al efecto ("wet")
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, e->slotId, auxSend, NULL);
	if (alGetError() != AL_NO_ERROR)
		printf("Error: no se pudo conectar la fuente al efecto\n");

	auxSends.push_back(e);
}

void AudioSource::removeEffect(unsigned int auxSend)
{
	// Desactivar la salida auxiliar dada
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, auxSend, NULL);
	if (alGetError() != AL_NO_ERROR)
		printf("Error: no se pudo quitar el efecto\n");

	auxSends.erase(auxSends.begin() + auxSend);
}

Effect* AudioSource::getAuxSend(int i)
{
	if (i >= auxSends.size())
		return nullptr;
	return auxSends[i];
}

void AudioSource::addFilteredEffect(Filter* f, Effect* e, unsigned int auxSend)
{
	// Enviar la señal al efecto dado, filtrándola por el camino
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, e->slotId, auxSend, f->filterId);
	if (alGetError() != AL_NO_ERROR)
		printf("Failed to configure Source Send 1\n");

	auxSends.push_back(e);
}


void AudioSource::attachFilter()
{
	// Quitarle el filtro a un efecto TODO
	/*
	// Filter the Source send 0 from 'sourceId' to 
	// Auxiliary Effect Slot "effectSlots[0]" using Filter filters[0] 
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AudioManager::Instance()->getSlot(0), 0, 
		AudioManager::Instance()->getFilter()->filterId);
	if (alGetError() == AL_NO_ERROR)
	{
		// Remove Filter from Source Auxiliary Send
		alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AudioManager::Instance()->getSlot(0), 0, AL_FILTER_NULL);
		if (alGetError() != AL_NO_ERROR)
			printf("Error: could not remove filter\n");
	}
	else printf("Error: could not apply aux send filter\n");
	*/
}
#include "AudioSource.h"

#include "CoreAudio.h"
#include "alext.h"

//#include "alc.h"
//#include "alut.h"

#include "Filter.h"
#include "Audio.h"
#include "AudioManager.h"
#include "ResourceManager.h"
#include "Camera.h"

#ifdef __DEBUG_INFO__
unsigned int AudioSource::numSources = 0;
#endif

unsigned int AudioSource::MAX_EFFECTS = 4;

AudioSource::AudioSource(Audio* audio) : m_loop(false), m_volume(1), m_pitch(1), directFilter(nullptr)
{
	setup(audio);
#ifdef __DEBUG_INFO__
	numSources++;
#endif
}

AudioSource::AudioSource(const std::string& audioID) : m_loop(false), m_volume(1), m_pitch(1), directFilter(nullptr)
{
	Audio* audio = ResourceManager::Instance()->getAudio(audioID);
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
	
	// Reconectar filtros
	if (directFilter != nullptr)
		addFilter(directFilter);
	// Reconectar efectos
	for (int i = 0; i < auxSends.size(); i++)
	{
		alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, auxSends[i]->slotId, i, NULL);
		checkALError("No se pudo conectar la fuente al efecto");
	}

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

void AudioSource::addFilter(Filter* f)
{
	// A�adir un filtro directo a esta fuente de audio
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

bool AudioSource::addEffect(Effect* e)
{
	if (e == nullptr || auxSends.size() >= MAX_EFFECTS) { return false; }

	// Conectar una de las salidas auxiliares de la fuente sin filtrar al efecto ("wet")
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, e->slotId, auxSends.size(), NULL);
	checkALError("No se pudo conectar la fuente al efecto");

	auxSends.push_back(e);
	return true;
}

void AudioSource::removeEffect(unsigned int auxSend)
{
	// Desactivar la salida auxiliar dada
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL,auxSend, NULL);
	checkALError("No se pudo quitar el efecto");

	auxSends.erase(auxSends.begin() + auxSend);
}

void AudioSource::removeEffect(Effect* effect)
{
	if (effect == nullptr || !hasEffect(effect)) { return; }

	int i = 0; bool found = false;
	while (i < auxSends.size())
	{
		if (auxSends[i] == effect && !found)
			found = true;
		i++;
	}

	// Desactivar la salida auxiliar dada
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, i - 1, NULL);
	checkALError("No se pudo quitar el efecto");

	auxSends[i - 1] = nullptr;
	auxSends.erase(auxSends.begin() + (i - 1));
}

Effect* AudioSource::getEffect(int i)
{
	if (i >= auxSends.size())
		return nullptr;
	return auxSends[i];
}

bool AudioSource::hasEffect(Effect* effect)
{
	int i = 0; bool found = false;
	while (i < auxSends.size() && !found)
	{
		if (auxSends[i] == effect)
			found = true;
		i++;
	}

	return found;
}

void AudioSource::addFilteredEffect(Filter* f, Effect* e, unsigned int auxSend)
{
	// Enviar la se�al al efecto dado, filtr�ndola por el camino
	alSource3i(sourceId, AL_AUXILIARY_SEND_FILTER, e->slotId, auxSend, f->filterId);
	checkALError("Failed to configure Source Send" + auxSend);

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
#include "AudioSource.h"

#include "al.h"
#include "alut.h"

#include "Audio.h"
#include "ResourceManager.h"

AudioSource::AudioSource(Audio* audio)
{
	// Generar la fuente y decirle qué audio usaremos
	alGenSources(1, &sourceId);

	alSourcei(sourceId, AL_BUFFER, audio->bufferId);
	m_audio = audio;
}

AudioSource::AudioSource(const std::string& audioID)
{
	Audio* audio = (Audio*)&ResourceManager::Instance()->getAudio(audioID);

	// Generar la fuente y decirle qué audio usaremos
	alGenSources(1, &sourceId);

	alSourcei(sourceId, AL_BUFFER, audio->bufferId);
	m_audio = audio;
}

void AudioSource::play()
{
	alSourcePlay(sourceId);
}

void AudioSource::setAudio(Audio* audio)
{
	// Hay que borrar el source ya creado para cambiar el buffer
	alDeleteSources(1, &sourceId);

	// Volver a crear el source
	alGenSources(1, &sourceId);
	alSourcei(sourceId, AL_BUFFER, audio->bufferId);
	m_audio = audio;
}
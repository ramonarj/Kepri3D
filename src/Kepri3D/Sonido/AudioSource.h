#ifndef __AUDIO_SOURCE__
#define __AUDIO_SOURCE__

#include "Component.h"
#include "Defines.h"

struct Audio;

class AudioSource : public Component
{
public:
	AudioSource(Audio* audio);
	AudioSource(const std::string& audioID);
	~AudioSource();

	void update(float deltaTime) override;

	/* Reproduce el sonido asociado a esta fuente */
	void play();
	void pause();
	void stop();

	// Setters
	void setAudio(Audio* audio);
	void setVolume(float vol);
	void setLoop(bool loop);
	void setPitch(float pitch);

	// Gettters
	inline Audio* getAudio() const { return m_audio; }
	inline float getVolume() const { return m_volume; }
	inline float getPitch() const { return m_pitch; }

	// Efectos
	void configureSends();
	void attachFilter();

#ifdef __DEBUG_INFO__
	static unsigned int numSources;
#endif

private:
	unsigned int sourceId;
	Audio* m_audio;

	bool m_loop;
	float m_volume;
	float m_pitch;

	// Velocidad a la que se mueve la fuente de audio
	glm::vec3 m_vel;

	// Funciones aauxiliares
	void setup(Audio* audio);
};

#endif
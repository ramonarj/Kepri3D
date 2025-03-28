#ifndef __AUDIO_SOURCE__
#define __AUDIO_SOURCE__

#include "Component.h"

struct Audio;

class AudioSource : public Component
{
public:
	AudioSource(Audio* audio);
	AudioSource(const std::string& audioID);
	~AudioSource(){}

	void update(float deltaTime) override;

	/* Reproduce el sonido asociado a esta fuente */
	void play();

	// Setters
	void setAudio(Audio* audio);
	void setLoop(bool loop);

	// Gettters
	inline Audio* getAudio() const { return m_audio; }

private:
	unsigned int sourceId;
	Audio* m_audio;

	bool m_loop;

	// Funciones aauxiliares
	void setup(Audio* audio);
};

#endif
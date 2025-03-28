#ifndef __AUDIO_MAN__
#define __AUDIO_MAN__

#include "Component.h"

class AudioSource;
class Audio;
class Entity;

class AudioMan : public Component
{
public:
	AudioMan(AudioSource* src, Audio* altAudio);
	~AudioMan() {}

	void update(float deltaTime) override;
private:
	AudioSource* source;
	Audio* audio1;
	Audio* audio2;

	bool pausedMusic;
};

#endif

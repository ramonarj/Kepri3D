#ifndef __PIANO_COMP__
#define __PIANO_COMP__

#include "Component.h"

class AudioSource;
class Audio;
class Entity;

class Piano : public Component
{
public:
	Piano(AudioSource* src);
	~Piano() {}

	void update(float deltaTime) override;
private:
	AudioSource* source;
};

#endif

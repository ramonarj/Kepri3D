#ifndef __PIANO_COMP__
#define __PIANO_COMP__

#include "Component.h"

class AudioSource;
class Audio;
class Entity;

class Piano : public Component
{
public:
	Piano();
	~Piano();

	void start() override;
	void update(float deltaTime) override;
private:
	bool pianoActive = false;
	bool playing = false;
	int escala = 0;
	AudioSource* source;
	Audio* onda;

	// Toca una de las 12 notas disponibles, de semitono en semitono (0 = 440Hz, 11 = 880Hz)
	void playNote(int nota);
	void cambioSinte();
};

#endif

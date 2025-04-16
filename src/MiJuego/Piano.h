#ifndef __PIANO_COMP__
#define __PIANO_COMP__

#include "Component.h"

class AudioSource;
class Audio;
class Entity;
class UIElement;

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

	unsigned int m_instrument;

	// Vibrato
	float vibratoInit = 0;
	float vibratoNote;
	// Tremolo
	float tremoloInit = 0;
	float noteVolume;
	// Portamento
	bool portamento = false;
	bool porting = false;
	float targetPitch;
	// Visualizador de la onda
	UIElement* waveVisualizer;
	std::vector<UIElement*> effectVisualizers;
	std::string waveName;

	// Toca una de las 12 notas disponibles, de semitono en semitono (0 = 440Hz, 11 = 880Hz)
	void playNote(int nota);
	void controlEscalas();
	void controlNotas();
	void cambioSinte();
	void controlVibrato(float deltaTime);
	void controlTremolo(float deltaTime);
	void controlPortamento(float deltaTime);

	// Graficos
	void renderWave();
	void renderEffect(unsigned int effect, bool render);
};

#endif

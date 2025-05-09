#ifndef __PIANO_COMP__
#define __PIANO_COMP__

#include "Component.h"

class AudioSource;
class Audio;
class Entity;
class UIElement;
struct Filter;
struct Effect;
class Button;

class Piano : public Component
{
public:
	const int MAX_NOTAS = 4;
	Piano(const std::vector<std::string>& samples = {});
	~Piano();

	void start() override;
	void update(float deltaTime) override;
private:
	bool pianoActive = false;
	int escala = 0;
	bool unaNota;
	// Fuente sonora para cada tecla
	std::vector<AudioSource*> sources;
	std::vector<char> teclasPulsadas;

	// Audio para los instrumentos
	std::vector<Audio*> m_samples;
	Audio* m_recording = nullptr;
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

	// Filtros
	Filter* filtros [3];
	Filter* activeFilter;

	// Efectos
	static const unsigned int NUM_EFFECTS = 5;
	Effect* effects[NUM_EFFECTS];

	// Controles del canvas
	std::vector<Button*> botones;

	// Visualizador de la onda
	UIElement* waveVisualizer;
	std::vector<UIElement*> effectVisualizers;
	std::string waveName;

	// Toca una de las 12 notas disponibles, de semitono en semitono (0 = 440Hz, 11 = 880Hz)
	void playNote(int nota, int src);
	void controlEscalas();
	void controlNotas();
	void cambioSinte();
	void controlVibrato(float deltaTime);
	void controlTremolo(float deltaTime);
	void controlPortamento(float deltaTime);
	void controlFiltros(float deltaTime);
	void controlGrabacion(float deltaTime);

	int getFreeSource();

	// Graficos
	void renderWave();
	void renderEffect(unsigned int effect, bool render);
	void createEffects();
	void createControls();
	void createVisualizers();

	// Callbacks para los botones
	void addEffect(int index);
	static void addReverbCallback(Component*);
	static void addEchoCallback(Component*);
	static void addChorusCallback(Component*);
	static void addDistorsionCallback(Component*);
	static void addWahWahCallback(Component*);
};

#endif

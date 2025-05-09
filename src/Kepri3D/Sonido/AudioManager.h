#ifndef __SOUND_MANAGER__
#define __SOUND_MANAGER__

class Entity;

#include "al.h"
#include "CoreAudio.h"

const float DOPPLER_FACTOR = 1.0f;

class AudioManager
{
public:
	static AudioManager* Instance();

	// Setters
	void setListener(Entity* e);
	void setGlobalVolume(float vol);

	// Getters
	inline Entity* getListener() { return listener; }
	inline float getGlobalVolume() const { return m_volume; }

	/* Limpia la instancia; debe llamarse explícitamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

	void Update(float deltaTime);

	/* Abre un dispositivo de captura y empieza a grabar, un máximo de segundos dado */
	void record(int record_freq, float maxTime = 10.0f);
	AudioSample* stopRecord(AudioSample* buffer, int& numSamples);
	inline bool isRecording() const { return m_recording; }

private:
	AudioManager() : listener(nullptr), m_volume(1) {}
	~AudioManager() {}

	static AudioManager* s_instance;

	/* Puntero al Listener (solo puede haber 1 por escena) */
	Entity* listener;

	// Volumen total 
	float m_volume;

	bool m_recording = false;

	//glm::dvec3* m_position;
};

#endif
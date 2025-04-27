#ifndef __SOUND_MANAGER__
#define __SOUND_MANAGER__

class Entity;
struct Filter;

#include "al.h"

const float DOPPLER_FACTOR = 0.8f;


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

	// Efectos
	void createEffect();

	inline ALuint getEffect(int i) { return effects[i]; }
	inline ALuint getSlot(int i) { return effectSlots[i]; }

	/* Limpia la instancia; debe llamarse explícitamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

	void Update(float deltaTime);

private:
	AudioManager() : listener(nullptr), m_volume(1) {}
	~AudioManager() {}

	static AudioManager* s_instance;

	/* Puntero al Listener (solo puede haber 1 por escena) */
	Entity* listener;

	// Volumen total 
	float m_volume;

	// Temporal
	ALuint effectSlots[4] = { 0 }; // deberian ser 'ALuint'
	ALuint effects[2] = { 0 };

	//glm::dvec3* m_position;
};

#endif
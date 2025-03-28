#ifndef __SOUND_MANAGER__
#define __SOUND_MANAGER__

#include <string>

class Entity;

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

	void Update();

private:
	AudioManager() : listener(nullptr), m_volume(1) {}
	~AudioManager() {}

	static AudioManager* s_instance;

	/* Puntero al Listener (solo puede haber 1 por escena) */
	Entity* listener;

	// Volumen total 
	float m_volume;
	//glm::dvec3* m_position;
};

#endif
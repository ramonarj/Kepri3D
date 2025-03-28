#ifndef __SOUND_MANAGER__
#define __SOUND_MANAGER__

#include <string>

class Entity;

class AudioManager
{
public:
	static AudioManager* Instance();

	void setListener(Entity* e);

	/* Limpia la instancia; debe llamarse explícitamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

	void Update();

private:
	AudioManager() {}
	~AudioManager() {}

	static AudioManager* s_instance;

	/* Puntero al Listener (solo puede haber 1 por escena) */
	Entity* listener;
	//glm::dvec3* m_position;
};

#endif
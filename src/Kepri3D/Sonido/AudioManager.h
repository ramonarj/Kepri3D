#ifndef __SOUND_MANAGER__
#define __SOUND_MANAGER__

#include <string>
#include <iostream>

class AudioManager
{
public:
	static AudioManager* Instance();


	/* Limpia la instancia; debe llamarse explícitamente */
	inline void Clean() { delete s_instance; s_instance = nullptr; }

private:
	AudioManager() {}
	~AudioManager() {}

	static AudioManager* s_instance;

	void LoadAndPlay(const std::string& wavPath);
};

#endif
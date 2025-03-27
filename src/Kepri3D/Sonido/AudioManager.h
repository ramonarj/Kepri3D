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
	~AudioManager() { std::cout << "Destroy" << std::endl; }

	static AudioManager* s_instance;

	void LoadAndPlay(const std::string& wavPath);
	char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);
};

#endif
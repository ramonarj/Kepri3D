#ifndef __AUDIO__
#define __AUDIO__

#include <string>

struct Audio
{
	enum AudioFormat { WAV = 0, MP3 = 1};

	Audio(const std::string& filePath, AudioFormat formato);
	~Audio(){}

private:
	friend class AudioSource;
	unsigned int bufferId;

	// M�todos de carga
	static char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);
};

#endif
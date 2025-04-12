#ifndef __AUDIO__
#define __AUDIO__

#include <string>

enum WaveForm { Seno, Cuadrado, Sierra, Triangular, Ruido };

struct Audio
{
	enum AudioFormat { WAV = 0, MP3 = 1};

	Audio(const std::string& filePath, AudioFormat formato);
	/* Crea un audio conteniendo una forma de onda básica */
	Audio(WaveForm tipoOnda, float freq);
	~Audio(){}

private:
	friend class AudioSource;
	unsigned int bufferId;

	// Métodos de carga
	static char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);
};

#endif
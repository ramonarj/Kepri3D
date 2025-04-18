#ifndef __AUDIO__
#define __AUDIO__

#include <string>

enum WaveForm { Seno, Cuadrado, Sierra, Triangular, Ruido };

struct Audio
{
	enum AudioFormat { WAV = 0, MP3 = 1};

	Audio(const std::string& filePath, AudioFormat formato);
	/* Crea un audio conteniendo una forma de onda b�sica */
	Audio(WaveForm tipoOnda, float freq);
	~Audio(){}

	/* Genera datos PCM para una onda dada, con resoluci�n de 8bps */
	static unsigned char* generateWave(WaveForm tipoOnda, float freq, float sampleRate);
	static unsigned char* generateWave(WaveForm tipoOnda, float freq, float sampleRate, float duracion);
private:
	friend class AudioSource;
	unsigned int bufferId;

	// M�todos de carga
	static char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);
};

#endif
#ifndef __AUDIO__
#define __AUDIO__

#include <string>
#include "CoreAudio.h"

enum WaveForm { Seno, Cuadrado, Sierra, Triangular, Ruido };

struct Audio
{
	enum AudioFormat { WAV = 0, MP3 = 1};

	Audio(const std::string& filePath, AudioFormat formato);
	/* Crea un audio conteniendo una forma de onda básica */
	Audio(WaveForm tipoOnda, float freq);
	Audio(AudioSample* samples, int numSamples, int sampleRate);
	~Audio();

	/* Genera datos PCM para una onda dada, con resolución de 8bps */
	static AudioSample* generateWave(WaveForm tipoOnda, float freq, float sampleRate);
	static AudioSample* generateWave(WaveForm tipoOnda, float freq, float sampleRate, float duracion);

	/* Elimina silencios al principio y al final del audio */
	static AudioSample* cleanSamples(AudioSample* samples, int& numSamples);

	/* Normaliza el audio */
	static void normalize(AudioSample*& samples, int numSamples);
private:
	friend class AudioSource;
	unsigned int bufferId;

	// Métodos de carga
	static char* loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size);
};

#endif
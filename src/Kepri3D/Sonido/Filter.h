#ifndef __AUDIO_FILTER__
#define __AUDIO_FILTER__

#include "al.h"

enum FilterType { HighPass = 0, LowPass = 1, BandPass = 2 };
struct Filter
{
	/* 'cutFreq' debe estar entre 0 y 1 */
	Filter(FilterType type, float cutFreq);
	~Filter();

	void setFrequency(float newFreq);

	// Atributos
	ALuint filterId;
	FilterType type;
	float cutFreq;
};

#endif
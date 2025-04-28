#ifndef __AUDIO_FILTER__
#define __AUDIO_FILTER__

#include "al.h"

enum FilterType { LowPass = 1, HighPass = 2, BandPass = 3 };
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

// - - - - - - - - - - - - - - - - 

enum EffectType { Reverb = 1, Chorus = 2, Distorsion = 3, Echo = 4 };
struct Effect
{
	Effect(EffectType type);
	~Effect();

	// Atributos
	ALuint effectId;
	ALuint slotId;
	EffectType type;

private:
	static unsigned int s_effectSlots;
};

#endif
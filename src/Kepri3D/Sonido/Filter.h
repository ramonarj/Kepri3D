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

	static void fetchPointers();
};

// - - - - - - - - - - - - - - - - 

struct Effect
{
	enum EffectType { Reverb = 1, Chorus = 2, Distorsion = 3, Echo = 4, WahWah = 10 };

	virtual ~Effect();

	// Métodos
	void setGain(float f);

	// Atributos
	ALuint effectId;
	ALuint slotId;
	EffectType type;

	static void fetchPointers();

protected:
	Effect(EffectType type);
	void reconnect();
private:
	static unsigned int s_effectSlots;
};


// - - - - Los distintos efectos - - - - //
struct ReverbFX : public Effect
{
	ReverbFX();
	~ReverbFX(){}

	void setDecayTime(float f);
};

struct ChorusFX : public Effect
{
	ChorusFX();
	~ChorusFX() {}

	void setFeedback(float f);
};

struct EchoFX : public Effect
{
	EchoFX();
	~EchoFX() {}

	void setDelay(float f);
	void setFeedback(float f);
};

struct DistorsionFX : public Effect
{
	DistorsionFX();
	~DistorsionFX() {}
};

struct WahWahFX : public Effect
{
	WahWahFX();
	~WahWahFX() {}

	void setAttackTime(float f);
};

#endif
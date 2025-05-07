#include "Filter.h"

#include "CoreAudio.h"
#include "AudioManager.h"
#include "alext.h"
#include "Utils.h"

#include <iostream>

/* Punteros a todas las funciones necesarias de la extensi�n EFX */
// Ranuras
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
// Efectos
LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
LPALEFFECTI alEffecti;
LPALEFFECTF alEffectf;
// Filtros
LPALGENFILTERS alGenFilters;
LPALDELETEFILTERS alDeleteFilters;
LPALISFILTER alIsFilter;
LPALFILTERI alFilteri;
LPALFILTERF alFilterf;

Filter::Filter(FilterType type, float cutFreq) : type(type), filterId(0)
{
	// Crear el filtro
	alGetError();
	alGenFilters(1, &filterId);

	// Gesti�n de errores
	checkALError("No se pudo crear el filtro");
	if (!alIsFilter(filterId)) { return; }

	// Darle los par�metros adecuados
	alFilteri(filterId, AL_FILTER_TYPE, AL_FILTER_NULL + type);
	checkALError("No hay soporte para filtros de este tipo");
	setFrequency(cutFreq);
}

void Filter::setFrequency(float newFreq)
{
	Kepri::clamp(newFreq, 0, 1);

	switch(type)
	{
	case LowPass:
	{
		alFilterf(filterId, AL_LOWPASS_GAIN, 1 - newFreq);
		alFilterf(filterId, AL_LOWPASS_GAINHF, newFreq);
		break;
	}
	case HighPass:
	{
		alFilterf(filterId, AL_HIGHPASS_GAIN, 1 - newFreq);
		alFilterf(filterId, AL_HIGHPASS_GAINLF, newFreq);
		break;
	}
	case BandPass:
	{
		float freq1 = newFreq - 0.05;
		float freq2 = newFreq + 0.05;
		alFilterf(filterId, AL_BANDPASS_GAIN, 0.75);
		alFilterf(filterId, AL_BANDPASS_GAINLF, freq1);
		alFilterf(filterId, AL_BANDPASS_GAINHF, freq2);
		break;
	}
	default:
		break;
	}

	this->cutFreq = newFreq;
	std::cout << cutFreq << std::endl;
}

Filter::~Filter()
{
	alDeleteFilters(1, &filterId);
}

void Filter::fetchPointers()
{
	// Filtros
	alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
	alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
	alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
	alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
	alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");


	if (!(alGenFilters && alIsFilter && alFilteri && alFilterf))
		std::cout << "ERROR: Punteros a las funciones de OpenAL no encontrados";
}

// - - - - - - - - - - - - - - - - - - - - 

unsigned int Effect::s_effectSlots = 0;

Effect::Effect(EffectType type) : type(type), effectId(0)
{
	// 1) Crear una ranura para el nuevo efecto
	alGetError();
	alGenAuxiliaryEffectSlots(1, &slotId);
	checkALError("No se pudo crear la ranura para el efecto");
	s_effectSlots++;

	// 2) Crear el efecto 
	alGenEffects(1, &effectId);
	checkALError("No se pudo crear el efecto");
	if (!alIsEffect(effectId)) { return; }

	// 3) Darle el tipo adecuado
	alEffecti(effectId, AL_EFFECT_TYPE, AL_EFFECT_NULL + type);
	checkALError("No hay soporte para efectos de este tipo");
}

void Effect::setGain(float gain)
{
	Kepri::clamp(gain, 0, 1);
	alAuxiliaryEffectSlotf(slotId, AL_EFFECTSLOT_GAIN, gain);
}

void Effect::reconnect()
{
	alAuxiliaryEffectSloti(slotId, AL_EFFECTSLOT_EFFECT, effectId);
	checkALError("No se pudo cargar el efecto en la ranura");
}

Effect::~Effect()
{
	// Borrar el efecto y su ranura auxiliar
	alDeleteEffects(1, &effectId);
	alDeleteAuxiliaryEffectSlots(1, &slotId);
	s_effectSlots--;
}

void Effect::fetchPointers()
{
	// Ranuras
	alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
	alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
	alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
	alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");

	// Efectos
	alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
	alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
	alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
	alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");

	// Comprobar que ha ido bien
	if (!(alGenEffects && alDeleteEffects && alIsEffect))
	{
		std::cout << "ERROR:Effect:Punteros a las funciones de OpenAL no encontrados";
	}
	if (!(alGenAuxiliaryEffectSlots && alAuxiliaryEffectSloti && alEffecti && alEffectf))
		std::cout << "ERROR:Effect:Punteros a las funciones de OpenAL no encontrados";
}

// - - - - 

ReverbFX::ReverbFX() : Effect(Reverb)
{
	// Valores por defecto
	alEffectf(effectId, AL_REVERB_DECAY_TIME, 2.0f);

	// Hay que volver a hacer la conexi�n
	reconnect();
}

void ReverbFX::setDecayTime(float f) {
	alEffectf(effectId, AL_REVERB_DECAY_TIME, f);
	reconnect();
}

// - - - - - - - - - - - 

ChorusFX::ChorusFX() : Effect(Chorus)
{
	// Valores por defecto
	reconnect();
}

void ChorusFX::setFeedback(float f)
{
	Kepri::clamp(f, -0.99, 0.99);

	alEffectf(effectId, AL_CHORUS_FEEDBACK, f);
	reconnect();
}

// - - - - - - - - - - - 

EchoFX::EchoFX() : Effect(Echo)
{
	// Valores por defecto
	alEffectf(effectId, AL_ECHO_DELAY, 2.0f);
	reconnect();
}

void EchoFX::setDelay(float f)
{
	alEffectf(effectId, AL_ECHO_DELAY, f);
	reconnect();
}

void EchoFX::setFeedback(float f)
{
	Kepri::clamp(f, 0, 1);

	alEffectf(effectId, AL_ECHO_FEEDBACK, f);
	reconnect();
}

// - - - - - - - - - - - 

DistorsionFX::DistorsionFX() : Effect(Distorsion)
{
	// Valores por defecto
	reconnect();
}

// - - - - - - - - - - - 

WahWahFX::WahWahFX() : Effect(WahWah)
{
	// Valores por defecto
	reconnect();
}


void WahWahFX::setAttackTime(float f)
{
	Kepri::clamp(f, 0.0001, 1);

	alEffectf(effectId, AL_AUTOWAH_ATTACK_TIME, f);
	reconnect();
}

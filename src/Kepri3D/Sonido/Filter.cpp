#include "Filter.h"

#include "AudioManager.h"
#include "alext.h"

#include <iostream>

/* Punteros a todas las funciones necesarias de la extensión EFX */
// Ranuras
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
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

	// Gestión de errores
	if (alGetError() != AL_NO_ERROR) { printf("Error: could not generate filter\n"); }
	if (!alIsFilter(filterId)) { return; }

	// Darle los parámetros adecuados
	alFilteri(filterId, AL_FILTER_TYPE, AL_FILTER_NULL + type);
	if (alGetError() != AL_NO_ERROR) { printf("Error: no hay soporte para este tipo de filtro\n"); }
	setFrequency(cutFreq);
}

void Filter::setFrequency(float newFreq)
{
	if (newFreq < 0) { newFreq = 0; }
	else if (newFreq > 1) { newFreq = 1; }

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
	if (alGetError() != AL_NO_ERROR) { printf("Error: no se pudo crear la ranura para el efecto\n"); }
	s_effectSlots++;

	// 2) Crear el efecto 
	alGenEffects(1, &effectId);
	if (alGetError() != AL_NO_ERROR) { printf("Error: no se pudo crear el efecto \n"); }
	if (!alIsEffect(effectId)) { return; }

	// Darle los parámetros adecuados
	alEffecti(effectId, AL_EFFECT_TYPE, AL_EFFECT_NULL + type);
	if (alGetError() != AL_NO_ERROR) { printf("Error: no hay soporte para este tipo de filtro\n"); }

	// Específico del efecto
	switch(type)
	{
	case Reverb:
		alEffectf(effectId, AL_REVERB_DECAY_TIME, 2.0f);
		break;
	default:
		break;
	}
	// alEffecti(effects[1], AL_FLANGER_PHASE, 180);


	// 3) Insertar el efecto en la ranura
	alAuxiliaryEffectSloti(slotId, AL_EFFECTSLOT_EFFECT, effectId);
	if (alGetError() != AL_NO_ERROR) { printf("Error: could not load effect into effect slot\n"); }
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
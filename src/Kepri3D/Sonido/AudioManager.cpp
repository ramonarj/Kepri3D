#include "AudioManager.h"

#include "Audio.h"

#include <cassert>
#include <iostream>
#include <checkML.h>
#include "Entity.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "alut.h"

AudioManager* AudioManager::s_instance = nullptr;

void initOpenAL(bool useEffects = false);
void assignFunctionPointers();

// Temporal
ALuint effectSlots[4] = { 0 }; // deberian ser 'ALuint'
ALuint effects[2] = { 0 };
ALuint filters[1] = { 0 };

/* Punteros a todas las funciones necesarias de la extensión EFX */
LPALGENEFFECTS alGenEffects;
LPALDELETEEFFECTS alDeleteEffects;
LPALISEFFECT alIsEffect;
// Efectos
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
LPALEFFECTI alEffecti;
LPALEFFECTF alEffectf;
// Filtros
LPALGENFILTERS alGenFilters;
LPALISFILTER alIsFilter;
LPALFILTERI alFilteri;
LPALFILTERF alFilterf;

AudioManager* AudioManager::Instance()
{
	if (s_instance == nullptr)
	{
		s_instance = new AudioManager();

		initOpenAL(true);
		// Prueba efectos
		s_instance->createEffect();
	}
	return s_instance;
}

void assignFunctionPointers()
{
	alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
	alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
	alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");

	// Efectos
	alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
	alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
	alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
	alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
	// Filtros
	alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
	alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
	alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
	alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");

	/* Check function pointers are valid */
	if (!(alGenEffects && alDeleteEffects && alIsEffect))
	{
		std::cout << "ERROR: Punteros a las funciones de OpenAL no encontrados";
	}
	if (!(alGenAuxiliaryEffectSlots && alAuxiliaryEffectSloti && alEffecti && alEffectf))
		std::cout << "ERROR: Punteros a las funciones de OpenAL no encontrados";
	if (!(alGenFilters && alIsFilter && alFilteri && alFilterf))
		std::cout << "ERROR: Punteros a las funciones de OpenAL no encontrados";
}

void initOpenAL(bool useEffects)
{
	// - Con ALUT - 	
	/*
	* alutInit(0, nullptr);
	* alutLoadWAVFile
	* ALuint Buffer = 1;// = alutCreateBufferFromFile("../wavdata/holaMundo.wav");
	*/

	// - Con ALC -
	ALCcontext* pContext = nullptr;

	// 1) Abrir el dispositivo
	ALCdevice* pDevice = alcOpenDevice((ALCchar*)"DirectSound3D");
	assert(pDevice != nullptr);

	// 2) Crear el contexto
	// a) Sin efectos
	if (!useEffects) 
	{
		pContext = alcCreateContext(pDevice, NULL); // NULL porque no activamos ningún flag
		alcMakeContextCurrent(pContext);
	}
	// b) Con efectos EFX (reverb, etc.)
	else
	{
		ALint attribs[4] = { 0 };
		ALCint iSends = 0;

		/* Query for Effect Extension */
		if (alcIsExtensionPresent(pDevice, "ALC_EXT_EFX") == AL_FALSE)
		{
			printf("ERROR: EFX Extension not found!\n");
			return;
		}

		/* Use Context creation hint to request 4 Auxiliary sends per Source */
		attribs[0] = ALC_MAX_AUXILIARY_SENDS;
		attribs[1] = 4;
		pContext = alcCreateContext(pDevice, attribs);
		assert(pContext != nullptr);

		/* Activate the context */
		alcMakeContextCurrent(pContext);

		/* Retrieve the actual number of Aux Sends available on each Source */
		alcGetIntegerv(pDevice, ALC_MAX_AUXILIARY_SENDS, 1, &iSends);
		printf("Device supports %d Aux Sends per Source\n", iSends);

		/* Get the Effect Extension function pointers */
		assignFunctionPointers();

		/* EFX available and ready to be used ! */
	}

	// 3) Otros parámetros
	// Modelo de atenuación (inverso/lineal/exponencial/ninguno, y con/sin clamp)
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	// Cantidad de efecto Doppler
	alDopplerFactor(DOPPLER_FACTOR);
	//alSpeedOfSound(1000);

	// Gestión de errores
	if (alGetError() != AL_NO_ERROR)
		std::cout << "ERROR OPENAL" << std::endl;
}

void AudioManager::setListener(Entity* e)
{
	listener = e;

	// Puntero a la posición de la entidad
	//double* ptr = (double*)&e->getModelMat();
	//m_position = (glm::dvec3*)&ptr[12];
}

void AudioManager::setGlobalVolume(float vol)
{
	if (vol > 1 || vol < 0) { return; }

	m_volume = vol;
	alListenerf(AL_GAIN, vol);
}

void AudioManager::Update(float deltaTime)
{
	if (listener == nullptr) { return; }

	// Posición del frame anterior
	glm::vec3 vel; alGetListener3f(AL_POSITION, &vel.x, &vel.y, &vel.z);

	glm::dvec3 pos = listener->getPosition();
	glm::dvec3 forward = -listener->forward();
	glm::dvec3 up = listener->up();
	// Orientación = 'forward' seguido de 'up'
	ALfloat ori[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };

	// Actualizar la posición y orientación del listener
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	alListenerfv(AL_ORIENTATION, ori);

	// Calcular la velocidad del listener y actualizarla si es necesario
	vel = ((glm::vec3)pos - vel) / deltaTime;
	if (!isnan(vel.x)) {
		//alListener3f(AL_VELOCITY, vel.x, vel.y, vel.z);
	} // Esto queda regular en la mayoría de los casos (en juegos de carreras sí interesaría)

	// Control de errores
	if (alGetError() != AL_NO_ERROR)
		std::cout << "ERROR OPENAL" << std::endl;
}


void AudioManager::createEffect() 
{
	// Ranuras = 
	// Efectos = reverb, distorsión, compresor, etc
	// Filtros = HPF, LPF, BPF, Notch
	ALuint uiLoop;

	/* (1) Try to create 4 Auxiliary Effect Slots */
	alGetError();
	for (uiLoop = 0; uiLoop < 4; uiLoop++)
	{
		alGenAuxiliaryEffectSlots(1, &effectSlots[uiLoop]);
		if (alGetError() != AL_NO_ERROR)
			break;
	}
	printf("Generated %d Aux Effect Slots\n", uiLoop);

	/* (2) Try to create 2 Effects */
	for (uiLoop = 0; uiLoop < 2; uiLoop++)
	{
		alGenEffects(1, &effects[uiLoop]);
		if (alGetError() != AL_NO_ERROR)
			break;
	}
	printf("Generated %d Effects\n", uiLoop);

	/* a) Reverb */
	alGetError();
	if (alIsEffect(effects[0]))
	{
		alEffecti(effects[0], AL_EFFECT_TYPE, AL_EFFECT_REVERB);
		if (alGetError() != AL_NO_ERROR)
			printf("Reverb Effect not supported\n");
		// change Decay Time
		else
			alEffectf(effects[0], AL_REVERB_DECAY_TIME, 2.0f);
	}

	/* b) Flanger */
	alGetError();
	if (alIsEffect(effects[1]))
	{
		alEffecti(effects[1], AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
		if (alGetError() != AL_NO_ERROR)
			printf("Flanger effect not supported\n");
		// change Phase
		else
			alEffecti(effects[1], AL_FLANGER_PHASE, 180);
	}

	/* (2) Try to create a Filter */
	alGetError();
	alGenFilters(1, &filters[0]);
	if (alGetError() != AL_NO_ERROR)
		printf("Error: could not generate filter\n");
	if (alIsFilter(filters[0]))
	{
		/* Set Filter type to Low-Pass and set parameters */
		alFilteri(filters[0], AL_FILTER_TYPE, AL_FILTER_LOWPASS);
		if (alGetError() != AL_NO_ERROR)
			printf("Low Pass Filter not supported\n");
		else
		{
			alFilterf(filters[0], AL_LOWPASS_GAIN, 0.5f);
			alFilterf(filters[0], AL_LOWPASS_GAINHF, 0.5f);
		}
	}


	/* (3) Attach Effect/Filter to Auxiliary Effect Slot */
	alAuxiliaryEffectSloti(effectSlots[0], AL_EFFECTSLOT_EFFECT, effects[0]);
	if (alGetError() != AL_NO_ERROR)
		printf("Error: could not load effect into effect slot\n");
}
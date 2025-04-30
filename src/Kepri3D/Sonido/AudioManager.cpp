#include "AudioManager.h"

#include "Audio.h"
#include "Filter.h"

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

AudioManager* AudioManager::Instance()
{
	if (s_instance == nullptr)
	{
		s_instance = new AudioManager();

		initOpenAL(true);
	}
	return s_instance;
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

	const ALCchar* string = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);
	std::cout << "Sound Device: " << string << std::endl;

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
		Effect::fetchPointers();
		Filter::fetchPointers();

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
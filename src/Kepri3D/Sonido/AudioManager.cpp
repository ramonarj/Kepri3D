#include "AudioManager.h"

#include "Audio.h"

#include <cassert>
#include <iostream>
#include <iostream>
#include <checkML.h>
#include "Entity.h"

#include "al.h"
#include "alut.h"

AudioManager* AudioManager::s_instance = nullptr;

AudioManager* AudioManager::Instance()
{
	if (s_instance == nullptr)
	{
		s_instance = new AudioManager();

		// - - Inicializar OpenAL - - // // OpenAL Programmers's Guide, pag 10
		// - Con ALC -
		// Abrir el dispositivo
		ALCdevice* Device = alcOpenDevice((ALCchar*)"DirectSound3D");
		assert(Device != nullptr);
		// Crear el contexto con los flags de configuracion (NULL suele ser suficiente)
		ALCcontext* Context = alcCreateContext(Device, NULL);
		alcMakeContextCurrent(Context);
		// Gestión de errores
		if (alGetError() != AL_NO_ERROR)
			std::cout << "ERROR OPENAL" << std::endl;

		// - Con ALUT - 
		//alutInit(0, nullptr);

		/*
		//alutLoadWAVFile
		// creamos buffer
		ALuint Buffer = 1;// = alutCreateBufferFromFile("../wavdata/holaMundo.wav");
		*/
	}
	return s_instance;
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

void AudioManager::Update()
{
	if (listener == nullptr) { return; }

	glm::dvec3 pos = listener->getPosition();
	glm::dvec3 forward = -listener->forward();
	glm::dvec3 up = listener->up();
	// Orientación = 'forward' seguido de 'up'
	ALfloat ori[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };

	// Actualizar la posición y orientación del listener
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	alListenerfv(AL_ORIENTATION, ori);

	// Control de errores
	if (alGetError() != AL_NO_ERROR)
		std::cout << "ERROR OPENAL" << std::endl;
}
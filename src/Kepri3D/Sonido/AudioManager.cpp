#include "AudioManager.h"

#include "Audio.h"

#include <cassert>
#include <iostream>
#include <checkML.h>

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
		// Crear el contexto con los flags de configuracion
		// (NULL suele ser suficiente)
		ALCcontext* Context = alcCreateContext(Device, NULL);
		alcMakeContextCurrent(Context);
		// Gestión de errores
		if (alGetError() != AL_NO_ERROR) //limpiamos el bit de error
			std::cout << "ERROR OPENAL" << std::endl;

		// - Con ALUT - 
		//alutInit(0, nullptr);

		// Prueba de concepto
		//s_instance->LoadAndPlay("judia.wav");
		//s_instance->LoadAndPlay("C:\\Users\\Ramón\\Desktop\\Kepri3D\\bin\\judi.wav");

		/*
		//alutLoadWAVFile
		// creamos buffer
		ALuint Buffer = 1;// = alutCreateBufferFromFile("../wavdata/holaMundo.wav");
		if (alGetError() != AL_NO_ERROR)
			std::cout << "ERROR OPENAL" << std::endl;
		// creamos source
		ALuint Source;
		alGenSources(1, &Source);
		// enganchamos source al buffer
		alSourcei(Source, AL_BUFFER, Buffer);
		// reproducimos
		alSourcePlay(Source);
		*/
	}
	return s_instance;
}

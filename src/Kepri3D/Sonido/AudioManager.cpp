#include "AudioManager.h"

#include <cassert>
#include <iostream>
#include <fstream>
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
		s_instance->LoadAndPlay("C:\\Users\\Ramón\\Desktop\\Kepri3D\\bin\\judia.wav");

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

void AudioManager::LoadAndPlay(const std::string& wavPath)
{
	// Parsear el WAV y guardar los datos en un puntero
	int channel, sampleRate, bps, size;
	char* data = loadWAV(wavPath.c_str(), channel, sampleRate, bps, size);

	// Crear buffer de sonido con el formato correcto y rellenarlo
	unsigned int bufferid, format;
	alGenBuffers(1, &bufferid);
	if (channel == 1)
	{
		if (bps == 8)
		{
			format = AL_FORMAT_MONO8;
		}
		else {
			format = AL_FORMAT_MONO16;
		}
	}
	else {
		if (bps == 8)
		{
			format = AL_FORMAT_STEREO8;
		}
		else {
			format = AL_FORMAT_STEREO16;
		}
	}
	alBufferData(bufferid, format, data, size, sampleRate);
	delete data; // Liberar la memoria

	// Crear source, asignarle el sonido y reproducir
	unsigned int sourceid;
	alGenSources(1, &sourceid);
	alSourcei(sourceid, AL_BUFFER, bufferid);
	alSourcePlay(sourceid);
}

bool isBigEndian()
{
	int a = 1;
	return !((char*)&a)[0];
}

int convertToInt(char* buffer, int len)
{
	int a = 0;
	if (!isBigEndian())
		for (int i = 0; i < len; i++)
			((char*)&a)[i] = buffer[i];
	else
		for (int i = 0; i < len; i++)
			((char*)&a)[3 - i] = buffer[i];
	return a;
}

char* AudioManager::loadWAV(const char* fn, int& chan, int& samplerate, int& bps, int& size)
{
	char buffer[4];
	std::ifstream file(fn, std::ios::binary);
	file.read(buffer, 4);
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "this is not a valid WAVE file" << std::endl;
		return NULL;
	}
	file.read(buffer, 4);
	file.read(buffer, 4);      //WAVE
	file.read(buffer, 4);      //fmt
	file.read(buffer, 4);      //16
	file.read(buffer, 2);      //1
	file.read(buffer, 2);
	chan = convertToInt(buffer, 2);
	file.read(buffer, 4);
	samplerate = convertToInt(buffer, 4);
	file.read(buffer, 4);
	file.read(buffer, 2);
	file.read(buffer, 2);
	bps = convertToInt(buffer, 2);
	file.read(buffer, 4);      //data
	file.read(buffer, 4);
	size = convertToInt(buffer, 4);

	char* data = new char[size];
	file.read(data, size);

	// Cerrar el archivo y devolver los datos
	file.close();
	return data;
}
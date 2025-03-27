#include "Audio.h"

#include <fstream>
#include <iostream>

#include "al.h"
#include "alut.h"

Audio::Audio(const std::string& filePath, AudioFormat formato)
{
	// Parsear el archivo y guardar los datos en un puntero
	int channel, sampleRate, bps, size;
	char* data;
	if (formato == WAV)
		data = loadWAV(filePath.c_str(), channel, sampleRate, bps, size);
	else
		data = nullptr;

	// Tipo de formato (mono/est�reo y bitrate [8/16 bits/sample])
	unsigned int format;
	// Sonido monof�nico
	if (channel == 1)
	{
		if (bps == 8)
			format = AL_FORMAT_MONO8;
		else 
			format = AL_FORMAT_MONO16;
	}
	// Sonido estereof�nico (>1 canal)
	else 
	{
		if (bps == 8)
			format = AL_FORMAT_STEREO8;
		else 
			format = AL_FORMAT_STEREO16;
	}

	// Crear buffer de sonido y rellenarlo con los datos y formato correcto
	alGenBuffers(1, &bufferId);
	alBufferData(bufferId, format, data, size, sampleRate);

	delete data; // Liberar la memoria
}

// - - - - - - - - - - - - - - - 

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

char* Audio::loadWAV(const char* filePath, int& chan, int& samplerate, int& bps, int& size)
{
	char buffer[4];
	std::ifstream file(filePath, std::ios::binary);
	// RIFF
	file.read(buffer, 4);
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "Error: " << filePath << " no es un archivo WAVE v�lido" << std::endl;
		return NULL;
	}
	//
	file.read(buffer, 4);
	file.read(buffer, 4);      //WAVE
	file.read(buffer, 4);      //fmt
	file.read(buffer, 4);      //16
	file.read(buffer, 2);      //1
	// N�mero de canales
	file.read(buffer, 2);
	chan = convertToInt(buffer, 2);
	// Sample rate
	file.read(buffer, 4);
	samplerate = convertToInt(buffer, 4);
	// ?
	file.read(buffer, 4);
	file.read(buffer, 2);
	// Bits per sample
	file.read(buffer, 2);
	bps = convertToInt(buffer, 2);
	//
	file.read(buffer, 4);   //data
	// Tama�o total
	file.read(buffer, 4);
	size = convertToInt(buffer, 4);
	// Datos
	char* data = new char[size];
	file.read(data, size);

	// Cerrar el archivo y devolver los datos
	file.close();
	return data;
}
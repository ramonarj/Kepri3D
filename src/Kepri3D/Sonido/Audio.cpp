#include "Audio.h"

#include <fstream>
#include <iostream>

#include "CoreAudio.h"
#include "Utils.h" // PI

Audio::Audio(const std::string& filePath, AudioFormat formato)
{
	// Parsear el archivo y guardar los datos en un puntero
	int channel, sampleRate, bps, size;
	char* data;
	if (formato == WAV)
		data = loadWAV(filePath.c_str(), channel, sampleRate, bps, size);
	else
		data = nullptr;

	// Tipo de formato (mono/estéreo y bitrate [8/16 bits/sample])
	// OpenAL NO soporta resoluciones mayores a 16bits/sample(24,32)
	unsigned int format;
	// Sonido monofónico
	if (channel == 1)
	{
		if (bps == 8) format = AL_FORMAT_MONO8;
		else format = AL_FORMAT_MONO16;
	}
	// Sonido estereofónico (>1 canal)
	else 
	{
		if (bps == 8) format = AL_FORMAT_STEREO8;
		else  format = AL_FORMAT_STEREO16;
	}

	// Crear buffer de sonido y rellenarlo con los datos y formato correcto
	alGenBuffers(1, &bufferId);
	alBufferData(bufferId, format, data, size, sampleRate);

	// control errores
	checkALError("No se pudo crear el Audio Buffer para " + filePath);

	delete data; // Liberar la memoria
}

Audio::Audio(WaveForm tipoOnda, float freq)
{
	int sampleRate = 44100;
	// Duración variable (depende de la frecuencia); hacemos que el audio contenga solamente 1 ciclo de la onda
	// menos para el ruido, que lo hacemos de 1 segundo
	float dur = tipoOnda == Ruido ? 1.0 : 1.0 / freq;

	// Crear (sintetizar) la onda
	int size = dur * sampleRate;
	unsigned char* data = generateWave(tipoOnda, freq, sampleRate);

	// Crear buffer de sonido y rellenarlo con los datos y formato correcto
	alGenBuffers(1, &bufferId);
	alBufferData(bufferId, AL_FORMAT_MONO8, data, size, sampleRate);

	// Comprobación de errores
	checkALError("No se pudo crear la onda de audio");

	delete data; 
}

unsigned char* Audio::generateWave(WaveForm tipoOnda, float freq, float sampleRate)
{
	// Duración variable (depende de la frecuencia); hacemos que el audio contenga solamente 1 ciclo de la onda
	// menos para el ruido, que lo hacemos de 1 segundo
	float dur = tipoOnda == Ruido ? 1.0 : 1.0 / freq;

	return generateWave(tipoOnda, freq, sampleRate, dur);
}

unsigned char* Audio::generateWave(WaveForm tipoOnda, float freq, float sampleRate, float duracion)
{
	// 0 = pico de onda negativo // 128 = onda con amplitud cero // 255 = pico de amplitud positivo
	float maxVal = 255;
	// Crear (sintetizar) la onda
	int size = duracion * sampleRate;
	unsigned char* data = new unsigned char[size];
	float val;
	switch (tipoOnda)
	{
	case Seno:
	{
		for (int i = 0; i < size; i++)
		{
			val = (float)i / (float)sampleRate; //entre 0-1
			float dato = 127 * (sin(val * 2 * PI * freq) + 1);
			data[i] = dato;
		}
		break;
	}
	case Cuadrado:
	{
		for (int i = 0; i < size; i++)
		{
			val = (float)i / ((float)sampleRate / freq);
			val -= (int)val; //entre 0-1
			if (val <= 0.5) val = 1;
			else val = 0;
			data[i] = (float)(255.0 * val);
		}
		break;
	}
	case Sierra:
	{
		for (int i = 0; i < size; i++)
		{
			val = (float)i / ((float)sampleRate / freq);
			val -= (int)val; //entre 0-1
			if (val <= 0.5) data[i] = 128 + 255 * val;
			else data[i] = 128 * (val * 2.0 - 1);
		}
		break;
	}
	case Triangular:
	{
		for (int i = 0; i < size; i++)
		{
			val = (float)i / ((float)sampleRate / freq);
			val -= (int)val; //entre 0-1
			if (val < 0.25) val = val * 2 + 0.5;
			else if (val < 0.75) val = 1 - ((val - 0.25) / 0.5);
			else val = (val - 0.75) * 2;

			data[i] = (float)(255.0 * val);
		}
		break;
	}
	case Ruido:
	{
		for (int i = 0; i < size; i++)
			data[i] = float(rand() % 255);
		break;
	}
	default:
		break;
	}

	return data;
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
	// Los archivos RIFF están formados por bloques, que se componen de:
	// - Identificador del bloque (4 bytes)
	// - Entero sin signo Little-Endian con la longitud del bloque (4 bytes)
	// - Información del bloque (longitud variable)
	// - Relleno (1 byte) -> solo si la longitud del bloque no es par

	char buffer[4];
	std::ifstream file(filePath, std::ios::binary);
	// Etiqueta 'RIFF'; debe tenerla para ser un WAV válido
	file.read(buffer, 4);
	if (strncmp(buffer, "RIFF", 4) != 0)
	{
		std::cout << "Error: " << filePath << " no es un archivo WAVE válido" << std::endl;
		return NULL;
	}
	//
	file.read(buffer, 4);
	// Etiqueta "WAVEfmt "
	file.read(buffer, 4);
	file.read(buffer, 4);
	//
	file.read(buffer, 4);      //16
	file.read(buffer, 2);      //1
	// Número de canales
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
	// Etiqueta 'data'
	file.read(buffer, 4);
	// Tamaño total
	file.read(buffer, 4);
	size = convertToInt(buffer, 4);
	// Datos
	char* data = new char[size];
	file.read(data, size);

	// Cerrar el archivo y devolver los datos
	file.close();
	return data;
}
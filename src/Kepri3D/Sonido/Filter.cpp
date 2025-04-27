#include "Filter.h"

//#include "AudioManager.h"
#include "AudioManager.cpp"

Filter::Filter(FilterType type, float cutFreq) : filterId(0)
{
	this->type = type;

	// Crear el filtro
	alGetError();
	alGenFilters(1, &filterId);
	// Gestión de errores
	if (alGetError() != AL_NO_ERROR) { printf("Error: could not generate filter\n"); }

	if (!alIsFilter(filterId)) { return; }

	// Darle los parámetros adecuados
	switch (type) 
	{
		case LowPass:
			alFilteri(filterId, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
			break;
		case BandPass:
			alFilteri(filterId, AL_FILTER_TYPE, AL_FILTER_BANDPASS);
			break;
		default: 
			break;
	}
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
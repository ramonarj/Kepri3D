#include "CoreAudio.h"

void checkALError(const std::string& message)
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
		std::cout << "[Error OpenAL|Codigo 0x" << std::uppercase << std::hex << error << "] " << message << std::endl;
}
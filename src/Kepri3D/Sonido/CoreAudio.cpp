#include "CoreAudio.h"

#include "alc.h"

void checkALError(const std::string& message)
{
	ALenum error = alGetError();
	if (error != AL_NO_ERROR)
		std::cout << "[Error OpenAL|Codigo 0x" << std::uppercase << std::hex << error << std::dec << "] " << message << std::endl;
}

void checkALCError(ALCdevice* device, const std::string& message)
{
	ALCenum error = alcGetError(device);
	if (error != ALC_NO_ERROR)
		std::cout << "[Error ALC|Codigo 0x" << std::uppercase << std::hex << error << std::dec << "] " << message << std::endl;
}
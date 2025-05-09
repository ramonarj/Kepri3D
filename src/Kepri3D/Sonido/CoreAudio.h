#ifndef __CORE_AUDIO__
#define __CORE_AUDIO__

#include <string>
#include <iostream>

struct ALCdevice;

typedef unsigned char AudioSample;

//1000 Hz
const int RECORDING_FREQ = 22050;

void checkALError(const std::string& message = "");
void checkALCError(ALCdevice* device, const std::string& message = "");

#endif
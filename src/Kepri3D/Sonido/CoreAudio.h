#ifndef __CORE_AUDIO__
#define __CORE_AUDIO__

#include "al.h"
#include <string>
#include <iostream>

struct ALCdevice;

void checkALError(const std::string& message = "");
void checkALCError(ALCdevice* device, const std::string& message = "");

#endif
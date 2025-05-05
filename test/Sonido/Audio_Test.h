#ifndef __AUDIO_SUITE__
#define __AUDIO_SUITE__

#include "../TestSuite.h"

class AudioSuite : public TestSuite
{
public:
	void setup() override;
protected:
	// Tests
	Test test_generateWave();
	Test test2() {}
};

#endif
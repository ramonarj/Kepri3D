#ifndef __AUDIO_TEST__
#define __AUDIO_TEST__

#include <vector>
#include "../TestSuite.h"

class AudioSuite : public TestSuite
{
public:
	void setup() override;
protected:
	//void setup() override;

	// Tests
	Test test_generateWave();
	Test test2() {}
};

#endif
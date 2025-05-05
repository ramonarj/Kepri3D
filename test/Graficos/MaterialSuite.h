#ifndef __MATERIAL_TEST__
#define __MATERIAL_TEST__

#include "../TestSuite.h"

class MaterialSuite : public TestSuite
{
public:
	void setup() override;
protected:

	// Tests
	Test getAmbient();
	Test getDiffuse();
	Test getSpecular();
	Test getEmission();
	Test getBrillo();

	Test getTexture();
	Test setTexture();
};

#endif
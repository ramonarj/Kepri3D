#ifndef __ENTITY_TEST__
#define __ENTITY_TEST__

#include "TestSuite.h"

class EntitySuite : public TestSuite
{
public:
	void setup() override;
protected:
	// Tests
	Test test_addComponent();

	Test test_right();
	Test test_up();
	Test test_forward();

	Test test_setParent();
};

#endif
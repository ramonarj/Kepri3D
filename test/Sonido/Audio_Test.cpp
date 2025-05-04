#include "Audio_Test.h"

#include "Sonido/Audio.h"
#include <iostream>
#include <assert.h>

void test_generateWave()
{
	unsigned char* a = Audio::generateWave(Seno, 1, 100);
	//assert(a[0] == 0);

	a = Audio::generateWave(Cuadrado, 1, 100);
	assert(a[0] == 255);

	a = Audio::generateWave(Sierra, 1, 100);
	//assert(a[0] == 0);

	a = Audio::generateWave(Triangular, 1, 100);
	//assert(a[0] == 0);

	a = Audio::generateWave(Ruido, 1, 100);
	//assert(a[0] == 0);
}
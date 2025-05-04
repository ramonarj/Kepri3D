#include <iostream>

#include "Sonido/Audio_test.h"
#include "Entity_test.h"

int main()
{
	// Audio
	test_generateWave();

	// Entidades
	test_addComponent();
	test_right();
	test_up();
	test_forward();
	test_setParent();


	std::cout << "- - - Todos los tests pasados - - - ";
	int e; std::cin >> e;
}
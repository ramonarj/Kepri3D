#include <iostream>

#include "Sonido/Audio_test.h"
#include "Entity_test.h"
#include "Graficos/MaterialSuite.h"

#ifdef _DEBUG
#	define TEST_CONFIG "Debug"
#else
#	define TEST_CONFIG "Release"
#endif

int main()
{
	// Audio
	TestSuite::runTests<AudioSuite>();

	// Entidades
	TestSuite::runTests<EntitySuite>();

	// Materiales
	TestSuite::runTests<MaterialSuite>();

	// Log
	std::cout << "Configuracion: " << TEST_CONFIG << std::endl;
	std::cout << "- - - Todos los tests ejecutados (" << TestSuite::testsPasados() << "/" << TestSuite::testsTotales() 
		<< ") - - -" << std::endl;

	std::vector<SuiteInfo> suites = TestSuite::suitesInfo();
	for(int i = 0; i < suites.size(); i++)
	{
		std::cout << "    ->Suite '" << suites[i].nombre << "': " << suites[i].numTests << std::endl;
	}
	int e; std::cin >> e;
}
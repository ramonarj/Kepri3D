#ifndef __TEST_SUITE__
#define __TEST_SUITE__

#include <assert.h>
#include <vector>
#include <iostream>

typedef void(*Callback)();

#define Test static void

struct SuiteInfo
{
	std::string nombre = "Suite ??";
	unsigned int numTests = 0;
};

class TestSuite
{
public:
	// Ejecutar una suite concreta //
	template<typename T>
	static void runTests();

	// Métricas sobre los tests hechos //
	static const std::vector<SuiteInfo>& suitesInfo() { return s_suitesInfo; }
	static unsigned int testsTotales() { return s_testsTotales; }
	static unsigned int testsPasados() { return s_testsPasados; }
	static unsigned int testsFallados() { return s_testsFallados; }

protected:
	virtual void setup() = 0;
	inline void addTest(Callback test) { m_callbacks.push_back(test); }
	inline void addTests(const std::vector<Callback>& tests) { for (Callback test : tests) m_callbacks.push_back(test); }

	unsigned int numTests = 0;

private:
	std::vector<Callback> m_callbacks;

	static std::vector<SuiteInfo> s_suitesInfo;
	static unsigned int s_testsTotales;
	static unsigned int s_testsPasados;
	static unsigned int s_testsFallados;
};

template<typename T>
void TestSuite::runTests()
{
	// El método es estático por comodidad al llamarlo desde el main.
	
	// Creamos la suite concreta
	T* suite = new T();

	// Preparaciones
	suite->setup();

	// Ejecutar todos los tests
	for (Callback test : suite->m_callbacks)
	{
		test();
		suite->numTests++;
		s_testsTotales++;
		s_testsPasados++;
	}

	// Registrar la información sobre la suite ejecutada
	SuiteInfo info; 
	info.nombre = std::string(typeid(*suite).name()).substr(6);
	info.numTests = suite->m_callbacks.size();
	s_suitesInfo.push_back(info);

	// Limpiar
	delete suite;
}

#endif
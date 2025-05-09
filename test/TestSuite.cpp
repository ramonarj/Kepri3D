#include "TestSuite.h"

int TestSuite::s_argc = 0;
char** TestSuite::s_argv = nullptr;

unsigned int TestSuite::s_testsTotales = 0;
unsigned int TestSuite::s_testsPasados = 0;
unsigned int TestSuite::s_testsFallados = 0;
std::vector<SuiteInfo> TestSuite::s_suitesInfo;
#ifndef __UTILS__
#define __UTILS__

#include <vector>
#include <map>
#include <string>
#include <iostream>

// PI
#define PI 3.14159265358979323846

// Imprime el nombre de una variable por pantalla
#define NOMBRE(VAR) std::cout << #VAR << std::endl;

enum ReferenceSystem
{
	LOCAL = 1,
	GLOBAL = 2
};

/**
 * Imprime por consola una matriz cuadrada de una forma cómoda para la vista.
 *
 * Los datos deben estar contiguos, sin espaciamiento entre ellos, y
 * en orden 'column-major' (por columnas, y no por filas)
 *
 * @param matrixPtr Puntero al primer dato de la matriz
 */
template<typename T = double, int size = 4>
void PrintMatrix(void* matrixPtr)
{
	// Filas
	for (int i = 0; i < size; i++)
	{
		// Columnas
		std::cout << "(";
		for (int j = 0; j < size; j++)
		{
			//std::cout << std::left << std::setw(3);
			T valor = ((T*)matrixPtr)[j * size + i];
			// Para evitar fallos de redondeo con valores por 10 a la -17 (que son 0 en realidad)
			if (valor < 0.00001 && valor > -0.00001)
				valor = 0;
			std::cout << valor << " ";
		}
		std::cout << ")\n";
	}
	std::cout << "\n";
}

/**
 * Limpia un vector de punteros a memoria dinámica, llamando a sus destructoras y limpiando el vector
 *
 */
template<typename T>
void CleanVector(std::vector<T*> & v)
{
	for (T* it : v)
	{
		delete it;
	}
	v.clear();
}

/**
 * Limpia un diccionario de punteros a memoria dinámica, llamando a sus destructoras y limpiando el diccionario
 *
 */
template<typename T>
void CleanMap(std::map<std::string, T*>& map)
{
	auto it = map.begin();
	while (it != map.end())
	{
		delete it->second;
		it++;
	}
	map.clear();
}

/* Convierte un archivo en una cadena de texto, incluyendo los saltos de línea que haya */
std::string FileToString(const char* fileName);

#endif
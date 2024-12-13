#ifndef __UTILS__
#define __UTILS__

#include <iomanip> 

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
// Imprime el nombre y contenido de una variable por pantalla
#define NOMBRE(VAR) std::cout << #VAR << std::endl;

#endif
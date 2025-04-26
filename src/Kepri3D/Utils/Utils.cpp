#include "Utils.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <complex>

std::string FileToString(const char* fileName)
{
	// Abrir el archivo
	std::ifstream stream(fileName);
	if (!stream.is_open())
	{
		throw std::ios_base::failure("No se pudo abrir el archivo '" + std::string(fileName) + "'");
	}

	// Leer todo el archivo línea a línea
	std::string linea;
	std::string fileContent = "";
	while (!stream.eof())
	{
		std::getline(stream, linea);
		fileContent = fileContent + linea + '\n';
	}

	// Cerrar el archivo
	stream.close();

	// Devolver el texto
	return fileContent;
}

void PrintVector(const glm::vec3& v)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2)
		<< "(" << v.x << "," << v.y << "," << v.z << ")";

	std::cout << stream.str();
}

float Kepri::lerp(float a, float b, float l)
{
	if (l < 0) l = 0;
	else if (l > 1) l = 1;
	return a + (b - a) * l;
}

glm::vec3 Kepri::lerp(const glm::vec3& a, const glm::vec3& b, float l)
{
	if (l < 0) l = 0;
	else if (l > 1) l = 1;
	return a + (b - a) * l;
}

void Kepri::DFT(std::vector<float> ondaTotal, float intervalo_muestreo,
	std::vector<float>& frecuencias, std::vector<float>& amplitudes, std::vector<float>& fases, 
	bool debug, float MIN_AMPLITUDE)
{
	using namespace std::complex_literals;

	int N = ondaTotal.size();

	// 1) Calcular la raíz primitiva para mayor comodidad
	std::complex<double> raiz_primitiva = pow(E, 2.0 * PI * 1i / (double)N);

	// 2) Calcular la lista de complejos, ya en el dominio de la frecuencia
	std::complex<double>* X = new std::complex<double>[N];
	for(int k = 0; k < N; k++)
	{
		X[k] = 0;
		// Calcular el sumatorio
		for (int n = 0; n < N; n++) 
		{
			X[k] += (double)ondaTotal[n] * pow(raiz_primitiva, - n * k);
		}
	}

	// 3) Calcular la amplitud, frecuencia y fase de cada onda contribuyente
	amplitudes.reserve(N); frecuencias.reserve(N); fases.reserve(N);
	for (int k = 0; k < N; k++)
	{
		// a) Amplitud; hacer el módulo y tener en cuenta el nº de muestras
		amplitudes.push_back(std::abs(X[k]) / (double)N);
		// b) Frecuencia
		frecuencias.push_back(k / (intervalo_muestreo * N));
		// c) Fase
		fases.push_back(atan2(X[k].imag(), X[k].real()) + PI / 2.0); // devolvemos senos, no cosenos
	}

	// 4) Aplicar el teorema de Nyquist; no podemos detectar ondas con frecuencia superior o igual a Fm/2
	amplitudes.resize(N / 2 + 1); // tener en cuenta A(0)
	frecuencias.resize(N / 2 + 1);
	fases.resize(N / 2 + 1);
	// Para compensar las frecuencias descartadas (son simétricas respecto a N/2)
	for (int k = 1; k < amplitudes.size(); k++)
		amplitudes[k] *= 2;

	// 5) Limpiar los vectores para devolver solo ondas que aporten un mínimo de amplitud
	int tot = amplitudes.size();
	for(int k = 0; k < amplitudes.size(); k++)
	{
		if (amplitudes[k] < MIN_AMPLITUDE)
		{
			amplitudes.erase(amplitudes.begin() + k);
			frecuencias.erase(frecuencias.begin() + k);
			fases.erase(fases.begin() + k);
			k--;
		}
	}

	// Escribir los resultados
	if(debug)
	{
		std::cout << "|~~~~~~~~~~~~~~~~[Transformada Discreta de Fourier]~~~~~~~~~~~~~~~~|" << std::endl;
		std::cout << "|-| Parametros |-| Duracion: " << intervalo_muestreo * N << "s |-| " << N << " muestras"
			<< " |-| Fmax: " << N / 2.0 - 0.5 << "Hz |-|" << std::endl;
		std::cout << std::setfill('_') << std::setw(68) << "" << std::endl << std::setfill(' ');
		// Cada una de las componentes
		for (int i = 0; i < frecuencias.size(); i++)
		{
			std::cout << "| Onda " << std::setw(2) << i << " | Frecuencia: " << std::fixed << std::setprecision(0) <<
				std::right << std::setw(5) << frecuencias[i] << "Hz | ";
			std::cout << std::fixed << std::setprecision(3) << "Amplitud: " << amplitudes[i] <<
				" | Fase: " << std::setw(6) << fases[i] / PI << "pi |" << std::endl;
		}
		std::cout << std::setfill('_') << std::setw(68) << "" << std::endl << std::setfill(' ');
	}
	// Limpiar
	delete[] X;
}
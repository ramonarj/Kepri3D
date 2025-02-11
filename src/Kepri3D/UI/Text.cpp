#include "Text.h"

#include "Mesh.h"
#include "Camera.h"


Text::Text(const std::string& texto, Canvas* canvas, const glm::vec4& color) : grosor(1.0)
{
	this->texto = texto;
	for (int i = 0; i < texto.size(); i++)
	{
		if (texto[i] != ' ' && texto[i] != '\n')
			letters.push_back(Mesh::generateLetter(texto[i], color));
	}

	//m_mesh = Mesh::generateText("AM");
	//m_mesh = Mesh::generateLetter('C');
	// 
	double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();
	//m_mesh = Mesh::generateRectangle((double)width / canvas->getWidth() * 2,
	//	(double)height / canvas->getHeight() * 2 / aspectRatio);

	// Dimensiones
	width = 50;
	height = 50;
	scale({ 0.05f, 0.05f, 1 });

	// Referencia al canvas
	setCanvas(canvas);
}

Text::~Text()
{
	for (Mesh* m : letters)
		delete m;
}

void Text::render(glm::dmat4 const& viewMat)
{
	glLineWidth(grosor);

	// Cargar la matriz de modelado
	glMatrixMode(GL_MODELVIEW);

	// Dibujar las mallas de cada una de las letras
	int col = 0;
	int fil = 0;
	int k = 0;
	for(int i = 0; i < texto.size(); i++)
	{
		// Saltar las líneas, justificando a la izquierda
		if (texto[i] == '\n')
		{
			fil++;
			col = 0;
		}
		// Saltar los espacios
		else if(texto[i] != ' ')
		{
			glLoadMatrixd(value_ptr(glm::translate(modelMat, { col + SPACING * col, -fil - SPACING_Y * fil, 0 })));
			letters[k]->draw();
			k++; col++;
		}
		else
			col++;
	}

	// 2) Renderizar mis hijos
	for (Entity* e : m_children)
		if (e->isActive())
			e->render(viewMat);
	
	glLineWidth(1.0);
}

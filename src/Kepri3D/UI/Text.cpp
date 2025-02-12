#include "Text.h"

#include "Mesh.h"
#include "Camera.h"

void Text::setText(const std::string& text)
{
	clearText();
	this->texto = text;
	for (int i = 0; i < texto.size(); i++)
	{
		if (texto[i] != ' ' && texto[i] != '\n')
			letters.push_back(Mesh::generateLetter(texto[i], color));
	}
}


Text::Text(const std::string& texto, Canvas* canvas, const glm::vec4& color) : grosor(1.0)
{
	// Inicializar el array de mallas
	this->color = color;
	setText(texto);

	double aspectRatio = (double)canvas->getWidth() / canvas->getHeight();

	// Dimensiones
	width = 50;
	height = 50;
	scale({ 0.05f, 0.05f, 1 });

	// Referencia al canvas
	setCanvas(canvas);
}

void Text::clearText()
{
	for (Mesh* m : letters)
		delete m;
	letters.clear();
}

Text::~Text()
{
	clearText();
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

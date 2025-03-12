#include "Text.h"

#include "Mesh.h"
#include "Camera.h"
#include "Texture.h"
#include "Shader.h"

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

void Text::render(Shader* sh)
{
	glLineWidth(grosor);

	// Indicar al shader que es texto
	sh->setInt("text", true);

	// Deshacer la posición de los hijos relativa al padre
	glm::dmat4 model = modelMat;
	Entity* parent = m_parent;
	while (parent != nullptr)
	{
		model = parent->getModelMat() * model;
		parent = parent->getParent();
	}

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
			// Cargar la matriz de modelado al shader
			glm::dmat4 matLetra = glm::translate(model, { col + SPACING * col, -fil - SPACING_Y * fil, 0 });
			sh->setMat4d("model", matLetra);
			// Dibujar
			letters[k]->draw();
			k++; col++;
		}
		else
			col++;
	}
	glLineWidth(1.0);

	// 2) Renderizar mis hijos
	for (Entity* e : m_children)
		if (e->isActive())
			e->render(sh);
}

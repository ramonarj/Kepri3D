#ifndef __TEXT__
#define __TEXT__

#include "UIElement.h"
class Mesh;

const float SPACING = 0.25f;
const float SPACING_Y = 0.7f;

class Text : public UIElement
{
public:
	/* Constructora por defecto */
	Text(const std::string& text, Canvas* canvas, const glm::vec4& color = {1, 1, 1, 1});
	~Text();

	void render(glm::dmat4 const& viewMat) override;

	void setGrosor(float grosor) { this->grosor = grosor; }

	/* Cambia el texto */
	void setText(const std::string& text);

private:
	/* Cadena de texto indicada por el usuario */
	std::string texto;

	/* Color del texto */
	glm::vec4 color;

	/* Lista de mallas de cada letra y n�mero */
	//TODO: hacer un diccionario est�tico para compartir letras entre distintos textos
	std::vector<Mesh*> letters;

	/* Grosor de la l�nea con que se dibujar�n las letras */
	float grosor;

	// M�todos auxiliares
	void clearText();
};

#endif
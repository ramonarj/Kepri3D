#ifndef __TEXT__
#define __TEXT__

#include "UIElement.h"

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

private:
	/* Cadena de texto indicada por el usuario */
	std::string texto;

	/* Lista de mallas de cada letra y número */
	//TODO: hacer un diccionario estático para compartir letras entre distintos textos
	std::vector<Mesh*> letters;

	/* Grosor de la línea con que se dibujarán las letras */
	float grosor;
};

#endif
#ifndef __TEXT__
#define __TEXT__

#include "UIElement.h"

const float SPACING = 0.2f;

class Text : public UIElement
{
public:
	/* Constructora por defecto */
	Text(const std::string& text, Canvas* canvas, const glm::vec4& color = {1, 1, 1, 1});
	~Text();

	void render(glm::dmat4 const& viewMat) override;

	void setGrosor(float grosor) { this->grosor = grosor; }

private:
	std::string texto;
	std::vector<Mesh*> letters;

	float grosor;
};

#endif
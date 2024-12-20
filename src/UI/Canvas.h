#ifndef __CANVAS__
#define __CANVAS__

#include <vector>
#include <glm.hpp>

class UIElement;
class Canvas
{
public:
	Canvas();

	void render(glm::dmat4 const& viewMat);

	/* Añade un elemento al canvas */
	inline void addElement(UIElement* e) { m_elements.push_back(e); }

private:
	std::vector<UIElement*> m_elements;
};
#endif
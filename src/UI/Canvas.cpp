#include "Canvas.h"

#include "UIElement.h"


Canvas::Canvas()
{

}

void Canvas::render(glm::dmat4 const& viewMat)
{
	for (UIElement* e : m_elements)
		e->render(viewMat);
}
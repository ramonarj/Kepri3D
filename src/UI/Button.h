#ifndef __BUTTON__
#define __BUTTON__

#include "../Mesh.h"
#include "../Camera.h"

#include "UIElement.h"

class Button : public UIElement
{
public:
	/* Constructora por defecto */
	Button(const std::string& textureName, Canvas* canvas);

	/* Establece el callback del bot�n */
	inline void setCallback(void(*callback)()) { m_callback = callback; }

	void update(GLuint deltaTime) override;

private:
	/* Puntero a la funci�n que llama el bot�n al clicarse */
	void(*m_callback)();
};

#endif
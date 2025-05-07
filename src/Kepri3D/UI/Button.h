#ifndef __BUTTON__
#define __BUTTON__

#include "UIElement.h"

class Component;
class Texture;

typedef void(*Callback)();
typedef void(*ComponentCallback)(Component* c);

class Button : public UIElement
{
public:
	/* Constructora por defecto */
	//Button(const std::string& textureName, Canvas* canvas);

	Button(unsigned int w, unsigned int h, Canvas* canvas);

	/* Establece el callback del bot�n */
	inline void setCallback(Callback cb) { m_callback = cb; }

	// Lo mismo pero para una funci�n miembro (est�tica) de un componente. TODO: mejorar
	inline void setCallback2(ComponentCallback cb, Component* caller)
	{
		m_callback2 = cb;
		m_caller = caller;
	}


	void setTexture(const std::string& textureID);
	void setHoverTexture(const std::string& textureID);

	void update(float deltaTime) override;

private:
	/* Puntero a la funci�n que llama el bot�n al clicarse */
	Callback m_callback;;
	//
	ComponentCallback m_callback2;
	Component* m_caller;

	// Cuando se pasa el rat�n por encima del bot�n
	Texture* m_texture;
	Texture* m_hoverTex;
	bool m_hover;

	bool insideRect(int mouseX, int mouseY);
	void callAllCallbacks();
};

#endif
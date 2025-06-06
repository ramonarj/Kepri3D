#ifndef __CANVAS__
#define __CANVAS__

#include <vector>
#include <glm.hpp>
#include <glew.h>

class UIElement;
class Shader;

class Canvas
{
public:
	Canvas();
	~Canvas();

	void render();
	void update(float deltaTime);

	/* A�ade un elemento al canvas */
	void addElement(UIElement* e);

	/* Establece el tama�o de referencia del canvas */
	inline void setSize(int w, int h) { width = w; height = h; }

	// Getters
	/* Ancho del canvas, en p�xeles */
	inline int getWidth() const { return width; }

	/* Alto del canvas, en p�xeles */
	inline int getHeight() const { return height; }

	/* Aspect ratio (altura / ancho) */
	inline double aspectRatio() const { return (double)width / height; }

	void resize(int newWidth, int newHeight);

private:
	std::vector<UIElement*> m_elements;

	/* Tama�o de referencia del canvas (no tiene por qu� coincidir con el tama�o de pantalla) */
	int width;
	int height;

	Shader* canvasSh;
};
#endif
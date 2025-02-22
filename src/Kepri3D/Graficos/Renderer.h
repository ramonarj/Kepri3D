#ifndef __RENDERER__
#define __RENDERER__

#include "Component.h"
class Mesh;

class Renderer : public Component
{
public:
	Renderer(Mesh* mesh);
	~Renderer();

	Mesh* getMesh() const { return m_mesh; }

	void draw();

	virtual void update(GLuint deltaTime) override {}

	/* Cómo debe pintarse la malla por delante y detrás (GL_FILL/GL_LINE/GL_POINT) */
	void setPolygonMode(GLenum front, GLenum back);

	// Sombras
	inline void castShadows(bool b) { m_castShadows = b; }
	inline void receiveShadows(bool b) { m_receiveShadows = b; }
	inline bool castShadows() const { return m_castShadows; }
	inline bool receiveShadows() const { return m_receiveShadows; }
private:
	/* Malla/s que se encarga de pintar */
	Mesh* m_mesh;

	/* Forma en que debe pintarse */
	GLenum m_polyModeFront;
	GLenum m_polyModeBack;

	/* Sombras (si emite / recibe) */
	bool m_receiveShadows;
	bool m_castShadows;
};

#endif

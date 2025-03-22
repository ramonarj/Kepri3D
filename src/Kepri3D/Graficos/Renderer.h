#ifndef __RENDERER__
#define __RENDERER__

#include "Component.h"
#include "Material.h"

class Shader;
class Mesh;

class Renderer : public Component
{
public:
	Renderer(Mesh* mesh);
	~Renderer();

	Mesh* getMesh() const { return m_mesh; }

	void draw(Shader* sh);
	// Fixed pipeline version
	void drawFixed();

	// Dibuja el volumen de la malla
	void drawVolume(Shader* sh);

	virtual void update(float deltaTime) override {}

	/* Cómo debe pintarse la malla por delante y detrás (GL_FILL/GL_LINE/GL_POINT) */
	void setPolygonMode(GLenum front, GLenum back);

	/* Devuelve el material que usael renderer */
	Material* getMaterial() { return &m_materials[0]; }

	/* Establece el material */
	void setMaterial(const std::string& materialID);
	void setMaterial(int i, const std::string& materialID);

	// Sombras - getters y setters
	inline void castShadows(bool b) { m_castShadows = b; }
	inline void receiveShadows(bool b) { m_receiveShadows = b; }
	inline bool castShadows() const { return m_castShadows; }
	inline bool receiveShadows() const { return m_receiveShadows; }

	// Debug
	static bool visibleBounds;
private:
	/* Malla que se encarga de pintar */
	Mesh* m_mesh;

	/* Número de submallas a pintar (para ahorrar tiempo luego) */
	GLuint m_numSubmallas;

	/* Materiales usado para pintar esa malla */
	Material* m_materials;

	/* Forma en que debe pintarse */
	GLenum m_polyModeFront;
	GLenum m_polyModeBack;

	/* Sombras (si emite / recibe) */
	bool m_receiveShadows;
	bool m_castShadows;

	/* Malla para pintar la Bounding Box */
	Mesh* m_volumeMesh;
};

#endif

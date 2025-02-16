#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <string>
#include <glew.h>
#include <glm.hpp>
#include "Framebuffer.h"
#include "Game.h"

class Entity;
class Camera;
class Light;
class Canvas;
class Shader;
class Mesh;
class Skybox;

// Clase abstracta que representa una escena de juego, con sus entidades, luces, etc.
class Scene
{
public:
	/* Constructora por defecto*/
	Scene();

	/* Destructora */
	~Scene();

	/* Carga todos los recursos necesarios para esta escena */
	virtual void loadResources() = 0;

	/* Crea las entidades y componentes que aparecerán en escena */
	virtual void init() = 0;

	/* Pinta todas las entidades */
	void render();

	/* Actualiza todas las entidades */
	void update(GLuint deltaTime);

	/* Añade una entidad a la escena */
	void AddEntity(Entity* e, bool isTranslucid = false);

	/* Añade una luz a la escena */
	inline void AddLight(Light* l) { m_lights.push_back(l); }

	/* Añade un efecto de postprocesado a la escena */
	void AddComposite(Shader* sh, bool active = true);

	/* Establece el skybox de la escena */
	inline void SetSkybox(Skybox* skybox) { m_skybox = skybox; }

	/* Establece la cámara de la escena */
	inline void setCamera(Camera* cam) { this->m_camera = cam; }

	/* Cambia el tamaño de los framebuffers */
	void resize(int width, int height);

	/* Devuelve el nombre de la escena */
	inline const std::string& getName() const { return m_name; }

	/* Cambia entre el modelo Phong y el Blinn-Phong */
	inline void switchBlinnPhong() { blinn = !blinn; }

	// Para ser accesibles desde callbacks
	static bool skyboxActive;
	static bool mipmapsActive;
	static bool compositesActive;
	static Shader* normalsShader;

#ifdef __DEBUG_INFO__
	/* Número de luces en la escena */
	GLuint numberOfLights() const { return m_lights.size(); }
	glm::ivec2 fbSize;
#endif

protected:
	/* Nombre de la escena */
	std::string m_name;

	/* Lista de entidades */
	std::vector<Entity*> m_entities;

	/* Lista de luces */
	std::vector<Light*> m_lights;

	/* Cámara activa */
	Camera* m_camera;

	/* Skybox */
	Skybox* m_skybox;

	/* Canvas de la escena */
	Canvas* m_canvas;

	/* Malla para pintar los efectos */
	Mesh* m_effectsMesh;

	/* Lista de efectos de postprocesado que se aplicarán (en este orden) a la escena */
	std::vector<Shader*> m_composites;

	// Framebuffers usados para los efectos
	Framebuffer* frameBuf;
	Framebuffer* frameBuf2;
	// Framebuffer usado para el multisampling
	Framebuffer* msBuf;

private:
	// Sub-métodos del render() para que sea más legible
	void loadLights();
	void renderSkybox(const glm::dmat4& projViewMat);
	void renderEntities(const glm::dmat4& projViewMat);
	void renderNormals(const glm::dmat4& projViewMat);
	void renderCanvas();
	void renderEffects();

	/* Manda todos los uniforms necesarios al shader de la entidad dada */
	void sendUniforms(Entity* e);

	// Modelo de iluminación. 0 = Phong, 1 = Blinn-phong
	bool blinn = false;
};

#endif
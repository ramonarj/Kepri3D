#ifndef __SCENE__
#define __SCENE__

#include <vector>
#include <string>
#include <glew.h>
#include <glm.hpp>
#include "Game.h"

class Entity;
class Camera;
class Light;
class Canvas;
class Shader;
class Mesh;
class Skybox;
class Renderer;
class Uniformbuffer;
class Framebuffer;
class Component;
class Viewport;

const int MAX_LUCES = 5;
// Lo que ocupa el struct "Light" del FS teniendo en cuenta alineamiento
const int LIGHT_STRUCT_SIZE = 128;

// Clase abstracta que representa una escena de juego, con sus entidades, luces, etc.
class Scene
{
public:
	/* Constructora por defecto*/
	Scene();

	/* Destructora */
	~Scene();

	static void clean();

	// Métodos abstractos
	/* Carga todos los recursos necesarios para esta escena */
	virtual void loadResources() = 0;

	/* Crea las entidades y componentes que aparecerán en escena */
	virtual void setup() = 0;

	//
	void init();

	/* Pinta el resultado de todas las cámaras activas a sus respectivos puertos de vista */
	void render();

	/* Actualiza todas las entidades */
	void update(float deltaTime);
	void callFixedUpdates(float fixedTime);

	/* Añade una entidad a la escena */
	void AddEntity(Entity* e);
	// Camara
	Camera* addCamera(Viewport* vp);
	Camera* getCamera(int i = 0) const;
	inline unsigned int numberOfCameras() const { return m_cameras.size(); }

	/* Añade un efecto de postprocesado a la escena */
	void AddComposite(Shader* sh, bool active = true);

	/* Cambia el tamaño de los framebuffers */
	void resize(int width, int height);	

	/* Cambia entre el modelo Phong y el Blinn-Phong */
	inline void switchBlinnPhong() { blinn = !blinn; }

	// Setters
	inline void SetSkybox(Skybox* skybox) { m_skybox = skybox; }
	inline void setCamera(Camera* cam) { this->m_camera = cam; }

	// Getters
	inline const std::string& getName() const { return m_name; }
	inline Entity* getEntity(int n) { return m_entities[n]; }
	inline const std::vector<Entity*>& getEntities() { return m_entities; }
	inline Skybox* getSkybox() { return m_skybox; }
	inline Canvas* getCanvas() { return m_canvas; }

	void toggleShadows();

	// Para ser accesibles desde callbacks
	static bool skyboxActive;
	static bool mipmapsActive;
	static bool compositesActive;
	static bool msaa_active;
	static Shader* normalsShader;

#ifdef __DEBUG_INFO__
	/* Número de entidades translúcidas */
	GLuint numberOfTrans() const { return m_transEntities.size(); }
	GLuint activeLights() const;
	GLuint culledEntities = 0;
	static glm::ivec2 fbSize;
#endif

protected:
	/* Nombre de la escena */
	std::string m_name;

	/* Lista de entidades */
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_opaqueEntities;
	std::vector<Entity*> m_transEntities;

	/* Lista de luces */
	std::vector<Light*> m_lights;

	/* Lista de renderers */
	std::vector<Renderer*> m_renderers;

	/* Todas las cámaras de la escena, y la que está activa */
	std::vector<Camera*> m_cameras;

	/* Cámara activa */
	static Camera* m_camera;

	/* Skybox */
	Skybox* m_skybox;

	/* Canvas de la escena */
	Canvas* m_canvas = nullptr;

	/* Malla para pintar los efectos */
	static Mesh* m_effectsMesh;

	/* Lista de efectos de postprocesado que se aplicarán (en este orden) a la escena */
	std::vector<Shader*> m_composites;

	// Framebuffers usados para los efectos
	static Framebuffer* frameBuf;
	static Framebuffer* frameBuf2;
	// Framebuffer usado para el multisampling
	static Framebuffer* msBuf;
	// Framebuffer con MRT
	static Framebuffer* mrtBuf;
	// Puntero al FB activo, para mayor comodidad
	static Framebuffer* activeFB;

	/* Uniform Buffer Objects para uniforms comunes a muchos shaders (luces, cámara) */
	static Uniformbuffer* m_uboMatrices;
	static Uniformbuffer* m_uboLuces;
	
	/* Composite para debug de shadow maps */
	static Shader* m_shadowComp;

private:
	friend struct Editor;
	friend class Game;

	/* Crea e inicializa los framebuffers correspondientes */
	static void setupStatics(Viewport* vp);

	/* Llama al start() de todos los componentes */
	void startComponents();
	void bindUBOs();

	// Sub-métodos del render() para que sea más legible
	void render(Camera* cam);
	void loadLights();
	void loadMatrices();
	void renderOpaques();
	void renderTranslucids();
	void renderNormals();
	void renderCanvas();
	void renderSkybox();
	void renderEffects();

	void renderEntities(const std::vector<Entity*>& entityList);

	/* Manda todos los uniforms necesarios al shader de la entidad dada */
	void sendUniforms(Shader* sh);
	void bakeShadows();
	void debugDepthMap();
	void Blit(Framebuffer* readFB, Framebuffer* writeFB);
	bool onFrustrum(Entity* e);

	// Modelo de iluminación. 0 = Phong, 1 = Blinn-phong
	bool blinn = true;
	// Sombras
	// 0 = sin sombras, 1 = sombras duras, 2 = sombras suaves
	int shadowsState = 0;
	bool frustrumCulling = false;
};

#endif
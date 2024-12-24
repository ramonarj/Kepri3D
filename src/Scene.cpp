#include "Scene.h"

#include <glm.hpp>

#include "Entity.h"
#include "Camera.h"
#include "Light.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "UI/Button.h"
#include "UI/Canvas.h"

//#include <glew.h>
#include <freeglut.h>

using namespace glm;

GLuint totalTime = 0;
bool Scene::shadersActive = false;

// Código fuente del vertex shader
const char* vertexShaderSource = "#version 330 core\n"
"in vec4 vertex;\n"
"uniform mat4 modelMat;\n" 
"void main()\n"
"{\n"
"gl_Position = vertex;\n"
"}";
// Código fuente del Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(0.3f, 0.8f, 0.2f, 1.0f);\n"
"}";

void Scene::AddEntity(Entity* e, bool isTranslucid)
{
	//if (!isTranslucid)
	//	m_opaqueEntities.push_back(e);
	//else
	//	m_translucentEntities.push_back(e);
	m_entities.push_back(e);
}

void Scene::initGLSubsystems()
{
	// Color de fondo (el predeterminado es negro)
	glClearColor(1.0, 1.0, 1.0, 0);  // (alpha=1 -> opaque)

	/* OpenGL basic settings */
	// Activa el Z-buffer. Si no lo activáramos, se pintaría todo con el algoritmo
	// del pintor (lo más reciente tapa lo antiguo)
	glEnable(GL_DEPTH_TEST);

	// Activa el uso de texturas 2D
	glEnable(GL_TEXTURE_2D);

	// Activa las transparencias, indicando qué canal usar para ello (SRC_ALPHA).
	// Si no llamáramos a glBlendFunc, se usarían los parámetros por defecto (0, 1) y no
	// habría transparencias
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //src, dest

	// Activa el descarte de fragmentos cuyo alfa no cumpla una cierta condición dada
	// NOTA: en el pipeline, va primero el alpha test y después el blend
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.2);

	/* Antialiasing; tanto para líneas, como para polígonos */
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	
	/* Culling; descarta el dibujado de los polígonos traseros. 
	Lo suyo sería tenerlo siempre activo, y las entidades que necesiten renderizar las 2 caras,
	lo desactivan y vuelven a activar en su 'render' */
	glEnable(GL_CULL_FACE); 

	/* Iluminación, activa el uso de las luces y sombreado */
	glEnable(GL_LIGHTING);
	// Tipo de modelo de sombreado -> GL_FLAT (flat), GL_SMOOTH (gouraud)
	glShadeModel(GL_SMOOTH);
	Material::setShadingType(GL_SMOOTH); // importante especificarlo para materiales también

	// Para no iluminar las caras traseras de las mallas. Por defecto está a false
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	// Punto de vista para la reflexión especular de los materiales
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	// Establecer la luz ambiente de toda la escena. Por defecto es (0.2, 0.2, 0.2, 1).
	GLfloat amb[4]{ 0.1, 0.1, 0.4, 1 };
	//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);
	// Normalizar los vectores normales. Si hacemos bien el cálculo de las normales en IndexMesh, no haría ni falta.
	//glEnable(GL_NORMALIZE);
	// Usar los colores de los vértices
	//glEnable(GL_COLOR_MATERIAL);
}

void Scene::init()
{
	// Activa el depth test, uso de texturas, iluminación, etc.
	initGLSubsystems();

	// CARGA DE RECURSOS
	/* Mallas que vamos a usar */
	ResourceManager::Instance()->loadMesh("Torre.obj", "torre");
	ResourceManager::Instance()->loadMesh("Peon.obj", "peon");

	/* Texturas que vamos a usar */
	ResourceManager::Instance()->loadTexture("earth24.bmp", "earth");
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("orientacion.bmp", "orientacion");
	ResourceManager::Instance()->loadTexture("Zelda.bmp", "zelda");
	ResourceManager::Instance()->loadTexture("terrenoTex.bmp", "terreno");
	ResourceManager::Instance()->loadTexture("caja.bmp", "caja");
	ResourceManager::Instance()->loadTexture("cobre.bmp", "cobre");

	ResourceManager::Instance()->loadTexture("botonCulling.bmp", "botonCulling");
	ResourceManager::Instance()->loadTexture("botonBlending.bmp", "botonBlending");
	ResourceManager::Instance()->loadTexture("botonLighting.bmp", "botonLighting");
	ResourceManager::Instance()->loadTexture("botonTextures.bmp", "botonTextures");
	ResourceManager::Instance()->loadTexture("botonShading.bmp", "botonShading");
	ResourceManager::Instance()->loadTexture("botonShaders.bmp", "botonShader");

	/* Materiales que vamos a usar */
	ResourceManager::Instance()->loadMaterial("copper.material", "cobre");
	ResourceManager::Instance()->loadMaterial("crome.material", "cromo");
	ResourceManager::Instance()->loadMaterial("ruby.material", "ruby");
	
	// Prueba excepciones
	ResourceManager::Instance()->loadTexture("ladrillo.bmp", "ladrillo");
	ResourceManager::Instance()->loadMaterial("plata.material", "plata");

	/* Shaders que vamos a usar */
	if (!GLEW_ARB_shading_language_100 || !GLEW_ARB_vertex_shader || !GLEW_ARB_fragment_shader || !GLEW_ARB_shader_objects)
	{
		std::cerr << "Shaders not available" << std::endl;
	}
	// Identificadores
	unsigned int vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Pasarle el código fuente a los identificadores
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compilarlos
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);
	// Comprobar que la compilación ha sido exitosa
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	// Crear un programa de shaders y asignarle los shaders
	m_shaderProgram = glCreateProgram();
	//glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);
	// Comprobar que ha sido exitoso
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
	}

	// Una vez enlazados, podemos borrar los VS y FS
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/* Luces */
	// Puntual
	Light* m_pointLight = new Light({ 1, 1, 0, 1 });
	m_pointLight->setPosition({ -2.5,0.5,-2.5 });
	m_pointLight->setActive(true);
	AddLight(m_pointLight);

	Light* m_pointLight2 = new Light({ 0, 0, 1, 1 });
	m_pointLight2->setPosition({ 3,3,-3 });
	AddLight(m_pointLight2);

	// Direccional
	Light* m_dirLight = new Light();
	m_dirLight->setDirection({ -1,0 , 0 });
	//m_dirLight->setAmbient({ 0.2, 0.2, 0.2 ,1 });
	m_dirLight->setActive(true);
	AddLight(m_dirLight);

	/* Entidades */
	// Ejes RGB
	//m_entities.push_back(new EjesRGB(0.5));

	// Polígono sin rellenar
	//m_entities.push_back(new Poligono(6, 0.5, false));
	
	// Polígono relleno
	//Poligono* pol = new Poligono(4, 1, true);
	//pol->setTexture("zelda");
	//AddEntity(pol);
	
	// Materiales y cubos
	PruebaMateriales();

	// Peon de ajedrez
	Entity* peon = new Entity();
	peon->setMesh("peon");
	peon->setTexture("cobre");
	peon->setMaterial("cromo");
	peon->setPosition({ 0,0,-2 });
	AddEntity(peon);


	// Torre de ajedrez
	Entity* torre = new Entity();
	torre->setMesh("torre");
	//torre->setTexture("cobre");
	torre->setMaterial("ruby");
	torre->setPosition({ 0,0,-2 });
	AddEntity(torre);
	// Para que el blending del rubí funcione bien (de momento), 
	// tiene que estar la última de las entidades


	/* Canvas */
	m_canvas = new Canvas();
	m_canvas->setSize(800, 600);

	// Botones
	// Culling
	Button* cullButton = new Button("botonCulling", m_canvas);
	cullButton->setPositionUI(0.12, 0.9);
	cullButton->setScaleUI(0.3, 0.3);
	//button->setPositionUI(50, 35);
	cullButton->setCallback(cullingButtonPressed);

	// Blending
	Button* blendButton = new Button("botonBlending", m_canvas);
	blendButton->setPositionUI(0.12, 0.75);
	blendButton->setScaleUI(0.3, 0.3);
	blendButton->setCallback(blendingButtonPressed);

	// Lighting
	Button* lightButton = new Button("botonLighting", m_canvas);
	lightButton->setPositionUI(0.12, 0.6);
	lightButton->setScaleUI(0.3, 0.3);
	lightButton->setCallback(lightingButtonPressed);

	// Textures
	Button* texturesButton = new Button("botonTextures", m_canvas);
	texturesButton->setPositionUI(0.12, 0.45);
	texturesButton->setScaleUI(0.3, 0.3);
	texturesButton->setCallback(texturesButtonPressed);

	// Shading
	Button* shadingButton = new Button("botonShading", m_canvas);
	shadingButton->setPositionUI(0.12, 0.3);
	shadingButton->setScaleUI(0.3, 0.3);
	shadingButton->setCallback(shadingButtonPressed);

	// Shaders
	Button* shaderButton = new Button("botonShader", m_canvas);
	shaderButton->setPositionUI(0.12, 0.15);
	shaderButton->setScaleUI(0.3, 0.3);
	shaderButton->setCallback(shaderButtonPressed);
}

void Scene::render()
{
	// 0) Limpia el color y el depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 1) Cargar las luces; IMPORTANTE hacerlo antes de pintar los objetos a los que puedan iluminar
	for (Light* l : m_lights)
		l->load(m_camera->getViewMat());

	// Activar el/los shader
	if(shadersActive)
		glUseProgram(m_shaderProgram);
	//if(shaderActive)
	//	glUseProgram(m_shaderProgram);
	//int mvpLocation = glGetUniformLocation(m_shaderProgram, "modelMat");
	//glUniformMatrix4dv(mvpLocation, 1, GL_FALSE, glm::value_ptr(m_camera->getViewMat()));

	// 2) Pintar todas las entidades
	for (Entity* e : m_entities)
	{
		e->render(m_camera->getViewMat());
	}
	// Desactivamos los shaders para el canvas
	glUseProgram(0);


	// 3) Pintar el canvas, limpiando antes el Z-buffer para que se pinte encima de tod
	glClear(GL_DEPTH_BUFFER_BIT);
	m_canvas->render(m_camera->getViewMat());

	//ViewportTest();

	// 4) Hacer swap de buffers
	// Hay 2 buffers; uno se está mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void Scene::update(GLuint deltaTime)
{
	for (Entity* e : m_entities)
	{
		e->update(deltaTime);
	}
	m_canvas->update(deltaTime);

	// Animación para la luz
	m_lights[1]->setPosition({15 * cos(totalTime * 0.002), 1, 5 * sin(totalTime * 0.002)});

	m_lights[2]->setDirection({ -cos(totalTime * 0.0001),-sin(totalTime * 0.0001), 0});
	totalTime += deltaTime;
}

void Scene::ViewportTest()
{
	glUseProgram(0);

	//Algunas variables locales para facilitar la puesta en escena
	Viewport* view = m_camera->getVP();
	GLdouble w = view->getW();
	GLdouble h = view->getH();
	// Reducimos el tamaño del puerto a 1/4 de la pantalla y dibujamos en los 3 primeros cuadrantes
	view->setSize(w / 2, h / 2);

	// Ejes RGB
	view->setPosition(0, h / 2);
	m_entities[0]->render(m_camera->getViewMat());

	// Polígono sin rellenar
	view->setPosition(w / 2, h / 2);
	m_entities[1]->render(m_camera->getViewMat());

	// Cubo texturizado
	view->setPosition(0, 0); // 0,0 es abajo a la izquierda
	m_entities[2]->render(m_camera->getViewMat());
	// Podemos dibujar un mismo objeto todas las veces que queramos (ejes extra)
	m_entities[0]->render(m_camera->getViewMat());

	// Cubo orientación
	view->setPosition(w / 2, 0);
	m_entities[3]->render(m_camera->getViewMat());

	view->setSize(w, h); //Volvemos a dejar el viewPort como estaba
}

void Scene::PruebaMateriales()
{
	// Cubo con la misma textura en todas las caras
	Cubo* c = new Cubo(2, true);
	c->setTexture("caja2");
	c->setPosition({ -10,0,0 });
	AddEntity(c);

	// Cubo de orientación (distintas texturas)
	Cubo* c2 = new Cubo(2, true, false);
	c2->setTexture("orientacion");
	c2->setMaterial("orientacion");
	c2->setPosition({ -5,0,0 });
	AddEntity(c2);

	// Cubo default
	Cubo* cuboDef = new Cubo(2, false);
	cuboDef->setMaterial("default");
	cuboDef->setPosition({ 0,0,0 });
	AddEntity(cuboDef);

	// Cubo de rubi
	Cubo* cuboRubi = new Cubo(2, false);
	cuboRubi->setMaterial("cromo");
	cuboRubi->setPosition({ 5,0,0 });
	AddEntity(cuboRubi);

	// Cubo de cobre
	Cubo* cuboCobre = new Cubo(2, true);
	//cuboCobre->setTexture("cobre");
	cuboCobre->setMaterial("cobre");
	cuboCobre->setPosition({ 10,0,0 });
	AddEntity(cuboCobre);

	// Esfera
	Esfera* esfera = new Esfera(1, 8, true);
	esfera->setTexture("earth");
	esfera->setPosition({ 0,2,-3 });
	AddEntity(esfera);

	// Tierra
	Esfera* tierra = new Esfera(3, 20, true);
	tierra->setTexture("earth");
	tierra->setPosition({ 6,15,0 });
	//tierra->rotate(-PI / 2, { 1, 0, 0 }, LOCAL); // 90º horario en eje X local
	tierra->rotate(-PI / 8, { 0, 0, 1 }, GLOBAL);
	AddEntity(tierra);

	// Venus
	//Esfera* venus = new Esfera(4, 20, true);
	//venus->setTexture(*venusTex);
	//venus->setPosition({ 15,10,0 });
	//m_entities.push_back(venus);

	// Rejilla (suelo)
	Grid* grid = new Grid(80, 160, 0.25, 0.25);
	grid->setTexture("cobre");
	grid->setMaterial("cromo");
	grid->setPosition({ 0,-1,0 });
	AddEntity(grid);


	// Terreno
	Terrain* terrain = new Terrain("../bin/assets/terrain.raw", 0.5);
	terrain->setTexture("terreno");
	terrain->setPosition({ 0,-10,0 });
	AddEntity(terrain);
}

void Scene::takePhoto()
{
	// Leemos la información del front buffer (el que está en pantalla), porque 
	// el trasero podría estar dibujándose y tendríamos 'tearing'0
	Texture::loadColorBuffer(m_camera->getVP()->getW(), m_camera->getVP()->getH(), GL_BACK);
	// Sin embargo, con GL_BACK afecta menos al rendimiento (framerate)

	Texture::save("foto.bmp");
}

void Scene::switchBoolParam(GLenum param)
{
	GLboolean value;
	glGetBooleanv(param, &value);
	if (value)
		glDisable(param);
	else
		glEnable(param);
}

void Scene::cullingButtonPressed()
{
	// Activar / desactivar el culling de polígonos traseros
	switchBoolParam(GL_CULL_FACE);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::blendingButtonPressed()
{
	// Activar / desactivar las transparencias en texturas y materiales
	switchBoolParam(GL_BLEND);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::lightingButtonPressed()
{
	// Activar / desactivar la iluminación
	switchBoolParam(GL_LIGHTING);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::texturesButtonPressed()
{
	// Activar / desactivar el uso de texturas
	switchBoolParam(GL_TEXTURE_2D);

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::shadingButtonPressed()
{
	// Cambiar entre sombreado FLAT y SMOOTH
	GLint shadeType;
	glGetIntegerv(GL_SHADE_MODEL, &shadeType);
	if (shadeType == GL_FLAT)
	{
		glShadeModel(GL_SMOOTH);
		Material::setShadingType(GL_SMOOTH);
	}

	else
	{
		glShadeModel(GL_FLAT);
		Material::setShadingType(GL_FLAT);
	}

	InputManager::Instance()->setMousePos(400, 300);
}

void Scene::shaderButtonPressed()
{
	shadersActive = !shadersActive;

	InputManager::Instance()->setMousePos(400, 300);
}

Scene::~Scene()
{
	// Borrar las entidades
	for(Entity * it : m_entities)
	{
		delete it;
	}
	m_entities.clear();

	// Borrar las luces
	for (Light* it : m_lights)
	{
		delete it;
	}
	m_lights.clear();

	// Borrar texturas y materiales
	ResourceManager::Instance()->Clean();

	// Desactivar los parámetros de OpenGL
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
}
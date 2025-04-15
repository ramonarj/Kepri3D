#include "PruebaSonido.h"

#include "../Kepri3D.h"

#include "../AudioMan.h"
#include "../CameraController.h"
#include "../DebugText.h"
#include "../Ambulancia.h"
#include "../Piano.h"

void PruebaSonido::loadResources()
{
	/* - - Texturas - - */
	ResourceManager::Instance()->loadTexture("venus.bmp", "venus");
	ResourceManager::Instance()->loadTexture("UI\\panel.png", "panel");

	ResourceManager::Instance()->loadCubemapTexture({ "skyboxes/city/right.jpg", "skyboxes/city/left.jpg", "skyboxes/city/bottom.jpg",
		"skyboxes/city/top.jpg", "skyboxes/city/front.jpg", "skyboxes/city/back.jpg" }, "citySkybox");

	/* - - Audio - - */
	ResourceManager::Instance()->loadAudio("judia.wav", "judia");
	ResourceManager::Instance()->loadAudio("concha.wav", "concha");
	ResourceManager::Instance()->loadAudio("luigi.wav", "luigi");
	ResourceManager::Instance()->loadAudio("ambulancia.wav", "ambulancia");
	ResourceManager::Instance()->loadAudio("seno.wav", "senoLa");
	ResourceManager::Instance()->loadAudio("senoDo.wav", "senoDo");
}

void PruebaSonido::init()
{
	Camera::setBackgroundColor(0, 0, 0);

	// Luz
	Light* dirLight = new Light(DIRECTIONAL_LIGHT);
	dirLight->setDirection({ 0,-1 , 0 });
	dirLight->setDiffuse({ 1, 1, 1 });
	dirLight->setAmbient({ 0.05, 0.05, 0.2 });
	Entity* dirLightEnt = new Entity({ dirLight });
	AddEntity(dirLightEnt);

	// Venus
	Esfera* venus = new Esfera(3, 20, 40);
	venus->setName("Venus");
	venus->setMaterial("default");
	venus->setTexture("venus");
	venus->setPosition({ -150,10,0 });
	AddEntity(venus);

	// Efecto Doppler
	venus->addComponent(new Ambulancia());

	// Audio Source. IMPORTANTE: EL UPDATE DEL AUDIOSOURCE DEBE EJECUTARSE EL ÚLTIMO, YA QUE USA 
	// EL INCREMENTO EN LA POSICIÓN DE LA ENTIDAD PARA EL EFECTO DOPPLER
	AudioSource* audioComp = new AudioSource("ambulancia"); //seno
	//audioComp->setVolume(0.5);
	audioComp->setLoop(true);
	venus->addComponent(audioComp);
	//audioComp->play();


	// - - AudioManager - - //
	Audio* fxConcha = (Audio*)&ResourceManager::Instance()->getAudio("concha");
	Entity* audioMan = new Entity("AudioMan");
	audioMan->addComponent(new AudioMan(audioComp, fxConcha));
	audioMan->addComponent(new CameraController(m_camera));
	// música
	AudioSource* musicSrc = new AudioSource("luigi");
	audioMan->addComponent(musicSrc); // música de la escena
	musicSrc->setVolume(0.4);
	musicSrc->setLoop(true);
	musicSrc->play();
	AddEntity(audioMan);

	// - - Piano - - //
	//AudioSource* pianoSrc = new AudioSource("senoLa");
	audioMan->addComponent(new Piano());

	// - - Audio sintetizado - - //
	//Audio* seno = new Audio(Triangular, 440.5);
	////Audio* seno = new Audio(Ruido, 440);
	//AudioSource* synthSrc = new AudioSource(seno);
	//audioMan->addComponent(synthSrc);
	//synthSrc->setLoop(true);
	//synthSrc->play();

	// Listener
	AudioManager::Instance()->setListener(m_camera);

	/* - - Skybox - - */
	Skybox* sky = new Skybox("citySkybox");
	SetSkybox(sky);

	/* - - Canvas - - */
	m_canvas = new Canvas();
	m_canvas->setSize(800, 600);

	// Información de Debug
	Entity* debugTxt = new Entity("DebugText");
	debugTxt->setPosition({ 0,0,0 });
	debugTxt->addComponent(new DebugText(m_canvas, "panel"));
	AddEntity(debugTxt);
	debugTxt->setActive(true);


	// Temporal
	for (Entity* e : m_entities)
	{
		if (e->getRenderer() != nullptr)
		{
			Shader* sh = (Shader*)e->getShader();
			if (sh != nullptr)
			{
				sh->bindUniformBlock("Matrices", 0);
				sh->bindUniformBlock("Lights", 1);
			}
		}
	}
}
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
	ResourceManager::Instance()->loadTexture("UI\\boton.png", "boton");

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

void PruebaSonido::setup()
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

	// - - - Tranformada Discreta de Fourier - - - //
	// Prueba de Fourier
	std::vector<float> ondaTotal = { 1, 0.5, -0.5, -1 };
	std::vector<float> frecuencias, amplitudes, fases;
	//Kepri::DFT(ondaTotal, 1, frecuencias, amplitudes, fases);

	// Prueba 2 de Fourier
	float freq_muestreo = 1000; // 100 muestras en un audio de 1 segundo
	std::vector<float> ondaFija;
	std::vector<float> onda1;
	std::vector<float> onda2;
	std::vector<float> onda3;
	std::vector<float> ondaCombinada;
	for(int i = 0; i < freq_muestreo; i++)
	{
		// f(x) = A * sin(wt + 0), siendo w = 2*PI*f
		ondaFija.push_back(-1.25); //0Hz
		onda1.push_back(0.6 * sin(2.0 * PI * 12.0 * ((float)i / freq_muestreo) + PI / 2.0)); //12Hz
		onda2.push_back(2.0 * sin(2.0 * PI * 203.0 * ((float)i / freq_muestreo) - PI / 5.0)); //203Hz
		onda3.push_back(0.1 * sin(2.0 * PI * 60 * ((float)i / freq_muestreo))); //60Hz
		ondaCombinada.push_back(ondaFija[i] + onda1[i] + onda2[i] + onda3[i]);
		//for (int j = 0; j < 10; j++)
		//	ondaCombinada[i] += (0.5 * sin(2.0 * PI * i * (100.0 + j) * ((float)i / freq_muestreo)));
	}
	Kepri::DFT(ondaCombinada, 1.0 / freq_muestreo, frecuencias, amplitudes, fases, true);

	// Reconstrucción de la onda
	std::cout << "* Reconstruyendo la onda...\n";
	std::vector<float> ondaReconstruida; 
	float errorMax = 0;
	// g(x) = sumatorio(Ak * sin(wk*t + 0))
	for (int i = 0; i < freq_muestreo; i++)
	{
		ondaReconstruida.push_back(0);
		for (int k = 0; k < amplitudes.size(); k++)
		{
			float val = amplitudes[k] * sin(2.0 * PI * frecuencias[k] * ((float)i / freq_muestreo) + fases[k]);
			ondaReconstruida[i] += val;
		}
		errorMax = std::max(errorMax, abs(ondaReconstruida[i] - ondaCombinada[i]));
	}
	std::cout << "* Error maximo->" << errorMax << std::endl;
}
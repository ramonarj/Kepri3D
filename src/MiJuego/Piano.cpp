#include "Piano.h"

#include "Kepri3D.h"

#include "AudioMan.h"
#include "CameraController.h"

const int NUM_ESCALAS = 3;
const int NUM_TECLAS = 13;
float frecuencias[NUM_TECLAS] = { 1, 1.06, 1.12, 1.19, 1.26, 1.33, 1.41, 1.5, 1.59, 1.68, 1.78, 1.89, 2 };
char teclas[NUM_TECLAS] = { 'a', 'w', 's', 'e', 'd', 'f', 't', 'g', 'y', 'h', 'u', 'j', 'k' };

Piano::Piano()
{
	onda = new Audio(Seno, 440);
	source = new AudioSource(onda);
	source->setLoop(true);
}
Piano::~Piano()
{
	delete onda;
}

void Piano::start()
{
	entity->addComponent(source);
}

void Piano::update(float deltaTime)
{
	// Activar/desactivar el piano
	if(InputManager::Instance()->getKeyDown('p'))
	{
		pianoActive = !pianoActive;
		// Desactivar los otros componentes
		AudioMan* audioMan = entity->getComponent<AudioMan>();
		CameraController* camController = entity->getComponent<CameraController>();
		audioMan->setActive(!pianoActive);
		camController->setActive(!pianoActive);
	}
	if (!pianoActive) { return; }

	// Subir/bajar la escala
	if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_UP))
		escala = (escala+1) % NUM_ESCALAS;
	else if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_DOWN))
		escala = (NUM_ESCALAS + (escala - 1)) % NUM_ESCALAS;

	// Comprobar si se está tocando alguna de las teclas del instrumento
	int i = 0; bool tocada = false;
	while(i < NUM_TECLAS && !tocada)
	{
		if(InputManager::Instance()->getKey(teclas[i]))
		{
			playNote(i);
			tocada = true;
		}
		i++;
	}

	// Detener la reproducción del audio
	if(!tocada && playing)
	{
		source->pause();
		playing = false;
	}

	// 'Cambiar de instrumento'
	cambioSinte();
}

void Piano::playNote(int nota)
{
	if (!playing) 
	{
		source->play();
		playing = true;
	}

	// Cambiar el pitch de acuerdo a la nota y a la escala
	source->setPitch(frecuencias[nota] * pow(2, escala));
}

void Piano::cambioSinte()
{
	// Teclas 1 - 5
	bool keyPressed = false; int i = 0;
	while (!keyPressed && i < 5)
	{
		if (InputManager::Instance()->getKeyDown('1' + i))
		{
			// Borrar la onda anterior
			delete onda;

			// Poner una con la forma especificada
			onda = new Audio(WaveForm(i), 440);
			source->setAudio(onda);
			keyPressed = true;
		}
		i++;
	}
}
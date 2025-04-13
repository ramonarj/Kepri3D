#include "Piano.h"

#include "Kepri3D.h"

#include "AudioMan.h"
#include "CameraController.h"

const int NUM_ESCALAS = 3;
const int NUM_TECLAS = 13;
const float VIBRATO_FREQ = 40;
const float VIBRATO_RANGE = 0.04; // entre 0.01 y 0.05 está bien
const float PORTAMENTO_VEL = 4;
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
	controlEscalas();

	// Tocar una nota
	controlNotas();

	// 'Cambiar de instrumento'
	cambioSinte();

	// Vibrato
	controlVibrato(deltaTime);

	// Portamento
	controlPortamento(deltaTime);
}

void Piano::playNote(int nota)
{
	// Cambiar el pitch de acuerdo a la nota y a la escala
	float pitch = frecuencias[nota] * pow(2, escala);
	if(pitch != source->getPitch())
	{
		if (portamento)
		{
			porting = true;
			targetPitch = pitch;
		}
		else
		{
			source->setPitch(pitch);
			vibratoNote = pitch;
		}
	}

	// Empezar a tocar nota si no estaba ya tocandose alguna
	if (!playing)
	{
		source->play();
		playing = true;

		porting = false;
		source->setPitch(pitch);
	}
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

void Piano::controlEscalas()
{
	if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_UP))
		escala = (escala + 1) % NUM_ESCALAS;
	else if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_DOWN))
		escala = (NUM_ESCALAS + (escala - 1)) % NUM_ESCALAS;
}

void Piano::controlNotas()
{
	// Comprobar si se está tocando alguna de las teclas del instrumento
	int i = 0; bool tocada = false;
	while (i < NUM_TECLAS && !tocada)
	{
		if (InputManager::Instance()->getKey(teclas[i]))
		{
			playNote(i);
			tocada = true;
		}
		i++;
	}

	// Detener la reproducción del audio
	if (!tocada && playing)
	{
		source->pause();
		playing = false;
		porting = false;
	}
}

void Piano::controlVibrato(float deltaTime)
{
	if (InputManager::Instance()->getKeyDown('v'))
		vibratoNote = source->getPitch();
	if (InputManager::Instance()->getKey('v'))
	{
		vibratoInit += deltaTime;
		source->setPitch(vibratoNote + VIBRATO_RANGE * sin(vibratoInit * VIBRATO_FREQ));
	}
}

void Piano::controlPortamento(float deltaTime)
{
	if (InputManager::Instance()->getKeyDown(9))
		portamento = !portamento;

	if(porting)
	{
		// Avanzar un poco la nota
		float newPitch;
		if(targetPitch < source->getPitch())
			newPitch = source->getPitch() - deltaTime * PORTAMENTO_VEL;
		else
			newPitch = source->getPitch() + deltaTime * PORTAMENTO_VEL;
		source->setPitch(newPitch);
		vibratoNote = newPitch;

		// Fin del portamento
		if (abs(targetPitch - newPitch) < 0.05)
		{
			porting = false;
			source->setPitch(targetPitch);
		}
	}
}
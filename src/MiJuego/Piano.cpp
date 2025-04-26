#include "Piano.h"

#include "Kepri3D.h"

#include "AudioMan.h"
#include "CameraController.h"

const int NUM_ESCALAS = 3;
const int NUM_TECLAS = 13;
const float VIBRATO_FREQ = 40; // LFO
const float VIBRATO_RANGE = 0.04; // entre 0.01 y 0.05 está bien
const float TREMOLO_FREQ = 30; // LFO
const float TREMOLO_RANGE = 1;
const float PORTAMENTO_VEL = 4;
float frecuencias[NUM_TECLAS] = { 1, 1.06, 1.12, 1.19, 1.26, 1.33, 1.41, 1.5, 1.59, 1.68, 1.78, 1.89, 2 };
char teclas[NUM_TECLAS] = { 'a', 'w', 's', 'e', 'd', 'f', 't', 'g', 'y', 'h', 'u', 'j', 'k' };
const std::string INSTRUMENT_NAMES[] = {"Seno", "Cuadrado", "Sierra", "Triangulo", "Ruido"};
enum NoteEffect { Vibrato = 0, Tremolo = 1, Portamento = 2 };

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
	m_instrument = Seno;

	// Crea las mallas para el visualizador
	createVisualizers();

	// Pintado inicial
	renderWave();
	for(int i = 0; i < 3; i++)
		renderEffect(NoteEffect(i), false);
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
		// Ocultar graficos
		waveVisualizer->getRenderer()->setActive(pianoActive);
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

	// Trémolo
	controlTremolo(deltaTime);

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
			m_instrument = i;
			renderWave();
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
	{
		vibratoNote = source->getPitch();
		renderEffect(Vibrato, true);
	}
	if (InputManager::Instance()->getKey('v'))
	{
		vibratoInit += deltaTime;
		source->setPitch(vibratoNote + VIBRATO_RANGE * sin(vibratoInit * VIBRATO_FREQ));
	}
	else { renderEffect(Vibrato, false); }
}

void Piano::controlTremolo(float deltaTime)
{
	if (InputManager::Instance()->getKeyDown('c')) // la 't' ya está pillada por una nota
	{
		noteVolume = source->getVolume();
		renderEffect(Tremolo, true);
	}

	if (InputManager::Instance()->getKey('c'))
	{
		tremoloInit += deltaTime;
		source->setVolume(noteVolume + TREMOLO_RANGE * sin(tremoloInit * TREMOLO_FREQ));
	}
	else
	{
		renderEffect(Tremolo, false);
		source->setVolume(1); // el valor por defecto
	}
}

void Piano::controlPortamento(float deltaTime)
{
	if (InputManager::Instance()->getKeyDown(9))
	{
		portamento = !portamento;
		renderEffect(Portamento, portamento);
	}

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

// - - - - - - - Graficos - - - - - - - - //

void Piano::renderWave()
{
	if (m_instrument > 4) { return; }

	// Crear las muestras de la onda
	unsigned char* datosPtr = Audio::generateWave((WaveForm)m_instrument, 1, 100);
	std::vector<float> data; data.reserve(100);
	for (int i = 0; i < 100; i++)
		data.push_back((float)datosPtr[i] / 127.0);
	delete datosPtr;

	// Cambiar la malla del visualizador
	delete waveVisualizer->getRenderer()->getMesh();
	waveVisualizer->getRenderer()->setMesh(Mesh::generateWaveform(data));
}

void Piano::renderEffect(unsigned int effect, bool render)
{
	effectVisualizers[effect]->getRenderer()->setActive(render);
}

void Piano::createVisualizers()
{
	// Visualizador de ondas
	Canvas* c = Game::Instance()->getScene()->getCanvas();
	waveVisualizer = new UIElement();
	waveVisualizer->setCanvas(c);
	waveVisualizer->addComponent(new Renderer(Mesh::generateWaveform({ 0.1, 0.2, 0.5, 1, 0.5, 0.2, 0.1 })));
	waveVisualizer->setScaleUI(0.3, 0.3);
	waveVisualizer->setPositionUI(0.05, 0.8);
	c->addElement(waveVisualizer);

	//  - - - De efectos - - - //
	// Vibrato
	effectVisualizers.push_back(new UIElement());
	effectVisualizers[0]->setCanvas(c);
	// Crear las muestras de la onda
	unsigned char* datosPtr = Audio::generateWave(Seno, 5, 100, 1);
	std::vector<float> data; data.reserve(100);
	for (int i = 0; i < 100; i++)
		data.push_back((float)datosPtr[i] / 127.0);
	effectVisualizers[0]->addComponent(new Renderer(Mesh::generateWaveform(data, { 0.6, 0.4, 0.0, 1.0 })));
	delete datosPtr;
	effectVisualizers[0]->setScaleUI(0.35, 0.35);
	effectVisualizers[0]->setPositionUI(1.0, 0.75);
	effectVisualizers[0]->setParent(waveVisualizer);

	// Tremolo
	effectVisualizers.push_back(new UIElement());
	effectVisualizers[1]->setCanvas(c);
	// Crear las muestras de la onda
	datosPtr = Audio::generateWave(Seno, 1, 100, 0.5);
	data.clear(); data.reserve(50);
	for (int i = 0; i < 50; i++)
		data.push_back((float)datosPtr[i] / 127.0);
	effectVisualizers[1]->addComponent(new Renderer(Mesh::generateWaveform(data, { 0.4, 0.1, 0.15, 1.0 })));
	delete datosPtr;
	effectVisualizers[1]->setScaleUI(0.35, 0.35);
	effectVisualizers[1]->setPositionUI(1.0, 0.5);
	effectVisualizers[1]->setParent(waveVisualizer);

	// Portamento
	effectVisualizers.push_back(new UIElement());
	effectVisualizers[2]->setCanvas(c);
	// Crear las muestras de la onda
	datosPtr = Audio::generateWave(Triangular, 1, 4);
	data.clear(); data.reserve(4);
	for (int i = 0; i < 2; i++)
		data.push_back((float)datosPtr[i + 2] / 127.0);
	effectVisualizers[2]->addComponent(new Renderer(Mesh::generateWaveform(data, { 0.0, 0.2, 0.85, 1.0 })));
	delete datosPtr;
	effectVisualizers[2]->setScaleUI(0.5, 0.5);
	effectVisualizers[2]->setPositionUI(0.4, 0.4);
	effectVisualizers[2]->setParent(waveVisualizer);
}
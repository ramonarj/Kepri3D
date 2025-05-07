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

void Piano::addReverb()
{
	for (int i = 0; i < MAX_NOTAS; i++)
	{
		// Ponerle el efecto
		if (sources[i]->getAuxSend(0) == nullptr)
		{
			sources[i]->addEffect(reverb, 0);
			botones[0]->setTexture("botonPulsado");
		}
		// Quitarlo	
		else
		{
			sources[i]->removeEffect(0);
			botones[0]->setTexture("boton");
		}
	}
}

void Piano::addEcho()
{
	for (int i = 0; i < MAX_NOTAS; i++)
	{
		// Ponerle el efecto
		if (sources[i]->getAuxSend(1) == nullptr)
		{
			sources[i]->addEffect(echo, 1);
			botones[1]->setTexture("botonPulsado");
		}
		// Quitarlo	
		else
		{
			sources[i]->removeEffect(1);
			botones[1]->setTexture("boton");
		}
	}
}

void Piano::addReverbCallback(Component* c)
{
	Piano* p = static_cast<Piano*>(c);
	p->addReverb();
}

void Piano::addEchoCallback(Component* c)
{
	Piano* p = static_cast<Piano*>(c);
	p->addEcho();
}

Piano::Piano() : reverb(nullptr), echo(nullptr)
{
	onda = new Audio(Seno, 440);
	for (int i = 0; i < MAX_NOTAS; i++)
	{
		sources.push_back(new AudioSource(onda));
		sources[i]->setLoop(true);
		sources[i]->setActive(false);
		teclasPulsadas.push_back(' ');
	}
	unaNota = true;
}
Piano::~Piano()
{
	delete onda;
	for (int i = 0; i < 3; i++)
		delete filtros[i];
	if (reverb != nullptr)
		delete reverb;
	if (echo != nullptr)
		delete echo;
}

void Piano::start()
{
	// El piano tiene tantos AudioSource como teclas se puedan pulsar a la vez, 
	// porque cada AudioSource solo reproduce 1 sonido
	for (int i = 0; i < MAX_NOTAS; i++)
		entity->addComponent(sources[i]);
	m_instrument = Seno;

	// Crea los filtros
	filtros[0] = new Filter(LowPass, 0.5);
	filtros[1] = new Filter(HighPass, 0.5);
	filtros[2] = new Filter(BandPass, 0.5);
	activeFilter = filtros[1];

	// Crea los efectos
	createEffects();

	// Crea las mallas para el visualizador
	createVisualizers();

	// Crea los botones de control
	createControls();
	// TODO: si se crean primero los controles y después los visualizadores, peta

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
		// Mostrar cursos
		InputManager::Instance()->setCursor(pianoActive ? GLUT_CURSOR_RIGHT_ARROW : GLUT_CURSOR_NONE);
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

	// Filtros
	controlFiltros(deltaTime);
}

void Piano::playNote(int nota, int src)
{
	sources[src]->setActive(true);

	// Cambiar el pitch de acuerdo a la nota y a la escala
	float pitch = frecuencias[nota] * pow(2, escala);
	if(pitch != sources[src]->getPitch())
	{
		if (portamento)
		{
			porting = true;
			targetPitch = pitch;
		}
		else
		{
			sources[src]->setPitch(pitch);
			vibratoNote = pitch;
		}
	}

	// Empezar a tocar la nota
	sources[src]->play();
	porting = false;
	sources[src]->setPitch(pitch);
}

void Piano::cambioSinte()
{
	// Teclas 1 - 5
	bool keyPressed = false; int i = 0;
	while (!keyPressed && i < 5)
	{
		if (i != m_instrument && InputManager::Instance()->getKeyDown('1' + i))
		{
			// Borrar la onda anterior
			delete onda;

			// Poner una con la forma especificada
			onda = new Audio(WaveForm(i), 440);
			for(int i = 0; i < MAX_NOTAS; i++)
				sources[i]->setAudio(onda);
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
	int src = getFreeSource();
	// Solo si hay fuentes libres donde alojar sonidos
	if (src >= 0)
	{
		// 1) Pulsación de nuevas teclas
		for (int i = 0; i < NUM_TECLAS; i++)
		{
			if (InputManager::Instance()->getKeyDown(teclas[i]))
			{
				playNote(i, src);
				teclasPulsadas[src] = teclas[i];
				//std::cout << "Pulsada " << teclasPulsadas[src] << std::endl;
			}
		}
	}

	// 2) Levantar una tecla que estuviera pulsada 
	for (int i = 0; i < MAX_NOTAS; i++)
	{
		if (!sources[i]->isActive()) { continue; }
		if (!InputManager::Instance()->getKey(teclasPulsadas[i]))
		{
			// Detener la reproducción del audio
			sources[i]->pause();
			sources[i]->setActive(false);
			//std::cout << "Levantada " << teclasPulsadas[i] << std::endl;
		}
	}
}

void Piano::controlVibrato(float deltaTime)
{
	if (InputManager::Instance()->getKeyDown('v'))
	{
		vibratoNote = sources[0]->getPitch();
		renderEffect(Vibrato, true);
	}
	if (InputManager::Instance()->getKey('v'))
	{
		vibratoInit += deltaTime;
		sources[0]->setPitch(vibratoNote + VIBRATO_RANGE * sin(vibratoInit * VIBRATO_FREQ));
	}
	else { renderEffect(Vibrato, false); }
}

void Piano::controlTremolo(float deltaTime)
{
	if (InputManager::Instance()->getKeyDown('c')) // la 't' ya está pillada por una nota
	{
		noteVolume = sources[0]->getVolume();
		renderEffect(Tremolo, true);
	}

	if (InputManager::Instance()->getKey('c'))
	{
		tremoloInit += deltaTime;
		for(int i = 0; i < MAX_NOTAS; i++)
			sources[i]->setVolume(noteVolume + TREMOLO_RANGE * sin(tremoloInit * TREMOLO_FREQ));
	}
	else
	{
		renderEffect(Tremolo, false);
		sources[0]->setVolume(1); // el valor por defecto
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
		if(targetPitch < sources[0]->getPitch())
			newPitch = sources[0]->getPitch() - deltaTime * PORTAMENTO_VEL;
		else
			newPitch = sources[0]->getPitch() + deltaTime * PORTAMENTO_VEL;
		sources[0]->setPitch(newPitch);
		vibratoNote = newPitch;

		// Fin del portamento
		if (abs(targetPitch - newPitch) < 0.05)
		{
			porting = false;
			sources[0]->setPitch(targetPitch);
		}
	}
}

void Piano::controlFiltros(float deltaTime)
{
	// Activar/desactivar el filtro LPF
	if(InputManager::Instance()->getKeyDown('l'))
	{
		for(int i = 0; i < MAX_NOTAS; i++)
		{
			if (sources[i]->getDirectFilter() == nullptr)
				sources[i]->addFilter(activeFilter);
			else
				sources[i]->removeFilter();
		}
	}

	// Desplazar la frecuencia de corte del filtro
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
	{
		activeFilter->setFrequency(activeFilter->cutFreq - deltaTime);
		// Esto no debería ser necesario
		for (int i = 0; i < MAX_NOTAS; i++)
		{
			if (sources[i]->getDirectFilter() != nullptr)
				sources[i]->addFilter(activeFilter);
		}
	}

	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
	{
		activeFilter->setFrequency(activeFilter->cutFreq + deltaTime);
		// Esto no debería ser necesario
		for (int i = 0; i < MAX_NOTAS; i++)
		{
			if (sources[i]->getDirectFilter() != nullptr)
				sources[i]->addFilter(activeFilter);
		}
	}

	// Añadirle efecto de reverb
	if (InputManager::Instance()->getKeyDown('r'))
	{
		for (int i = 0; i < MAX_NOTAS; i++)
		{
			if (sources[i]->getAuxSend(0) == nullptr)
				sources[i]->addEffect(reverb, 0);
				//sources[i]->addEffect(activeFilter, reverb, 0);
			else
				sources[i]->removeEffect(0);
		}
	}

}

int Piano::getFreeSource()
{
	int src = -1;
	int i = 0; 
	while(i < MAX_NOTAS && src < 0)
	{
		if (!sources[i]->isActive())
			src = i;
		i++;
	}

	return src;
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

void Piano::createEffects()
{
	// Reverb
	reverb = new ReverbFX();
	reverb->setGain(0.05);
	reverb->setDecayTime(5.0f);
	// Eco
	echo = new EchoFX();
	echo->setGain(0.05);
	echo->setDelay(2.0f);
	echo->setFeedback(0.1f);
}

void Piano::createControls()
{
	// Botones
	Canvas* c = Game::Instance()->getScene()->getCanvas();
	Button* boton = new Button(100, 50, c);
	boton->setTexture("boton");

	ComponentCallback cb = &Piano::addReverbCallback;
	boton->setCallback2(cb, this);
	boton->setHoverTexture("botonHover");

	boton->setPositionUI(0.5, 0.8);
	c->addElement(boton);
	botones.push_back(boton);

	// El texto que contiene
	Text* t = new Text("REVERB", c, { 1, 1, 1, 1 });
	t->setPositionUI(0.43, 0.5); // alineado a la izquierda
	t->setScaleUI(0.7, 0.7);
	t->setGrosor(1.5);
	t->setParent(boton);

	// Lo mismo con el eco
	boton = new Button(100, 50, c);
	boton->setTexture("boton");

	cb = &Piano::addEchoCallback;
	boton->setCallback2(cb, this);
	boton->setHoverTexture("botonHover");

	boton->setPositionUI(0.75, 0.8);
	c->addElement(boton);
	botones.push_back(boton);

	// El texto que contiene
	t = new Text("ECHO", c, { 1, 1, 1, 1 });
	t->setPositionUI(0.43, 0.5); // alineado a la izquierda
	t->setScaleUI(0.7, 0.7);
	t->setGrosor(1.5);
	t->setParent(boton);
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
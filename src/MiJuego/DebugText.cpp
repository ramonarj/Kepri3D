#include "DebugText.h"

#include "Kepri3D.h"

#include <iomanip>
#include <sstream>
#include "Piano.h"

DebugText::DebugText(Canvas* canvas, const std::string& backgroundTexID)
{
	visible = true;

	sceneName = Game::Instance()->getScene()->getName();

	if(physicsScene)
	{
		//
		fondo = new Button(600, 200, canvas);
		fondo->setPositionUI(0.5, 0.05);
		fondo->setScaleUI(0.85, 0.85);
		fondo->setTexture(backgroundTexID);
		canvas->addElement(fondo);

		// 
		text = new Text("DEBUG INFO", canvas, { 0, 0, 0, 1 });
		text->setPositionUI(0.15, 0.63);
		text->setGrosor(1.25);
		text->setScaleUI(0.64, 0.64);
		text->setParent(fondo);
	}
	else
	{
		//
		fondo = new Button(400, 200, canvas);
		fondo->setPositionUI(0.5, 0.05);
		fondo->setScaleUI(0.85, 0.7);
		fondo->setTexture(backgroundTexID);
		canvas->addElement(fondo);

		// 
		text = new Text("DEBUG INFO", canvas, { 0, 0, 0, 1 });
		text->setPositionUI(0.27, 0.64);
		text->setGrosor(1.25);
		text->setScaleUI(0.8, 0.8);
		text->setParent(fondo);
		//canvas->addElement(text);
	}
}

std::string VectorToString(const glm::vec3& v)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2)
		<< "(" << v.x << "," << v.y << "," << v.z << ")";

	return stream.str();
}

std::string FloatToString(float f)
{
	std::stringstream stream;
	stream << std::fixed << std::setprecision(2) << f;

	return stream.str();
}

void DebugText::update(float deltaTime)
{
	// Activar/desactivar
	if (InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_F8))
	{
		visible = !visible;
		fondo->setActive(visible);
	}

	// Información de debug una vez hemos acabado de dibujar todo
#ifdef __DEBUG_INFO__
	if(visible)
	{
		// Escena de gráficos
		if(sceneName == "PruebaFisicas")
		{
			glm::vec3 momento = PhysicsSystem::Instance()->momentoTotal;
			
			std::string s = "MOMENTO TOTAL " + VectorToString(momento) + 
				"\nMOMENTO (MAGNITUD) " + FloatToString(glm::length(momento)) +
				"\nRESTITUCION " + FloatToString(COEF_RESTITUCION) +
				"\nENERGIA CINETICA " + 
				"\nRIGIDS DESPIERTOS " + std::to_string(PhysicsSystem::Instance()->rigidsDespiertos) + 
				"\nFIXED TIME STEP " + std::to_string(PhysicsSystem::Instance()->getFixedTime());

			text->setText(s);
		}
		// Escena de físicas
		else if(sceneName == "Prueba")
		{
			Game::DebugInfo info = Game::Instance()->debugInfo;
			std::string s = "FPS " + std::to_string(info.fps) +
				"\nVERTICES " + std::to_string(info.numVerts) +
				"\nTRIANGLES " + std::to_string(info.numTris) +
				"\nTEXTURE BINDS " + std::to_string(info.numTextureBinds) +
				"\nBUFFER SIZE " + std::to_string(info.fbSize.x) + "X" + std::to_string(info.fbSize.y) +
				"\nLIGHTS " + std::to_string(info.lucesActivas) +
				"\nTRANS ENT " + std::to_string(info.numTrans) +
				"\nSHADER CHANGES " + std::to_string(info.programChanges) + 
				"\nCULLED ENTITIES " + std::to_string(info.culledEntities);

			text->setText(s);
		}
		// Escena de audio
		else if (sceneName == "PruebaSonido")
		{
			float vol = AudioManager::Instance()->getGlobalVolume();
			std::string s = "VOLUMEN GLOBAL " + std::to_string(vol) + 
				"\nLISTENER POS " + VectorToString(AudioManager::Instance()->getListener()->getPosition()) + 
				"\nNUM SOURCES " + std::to_string(AudioSource::numSources) +
				"\nDOPPLER FACTOR " + std::to_string(DOPPLER_FACTOR);

			text->setText(s);
		}
	}
#endif
}

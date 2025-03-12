#include "DebugText.h"

#include "Game.h"
#include "Text.h"
#include "Button.h"

DebugText::DebugText(Canvas* canvas, const std::string& backgroundTexID)
{
	//
	fondo = new Button(400, 200, canvas);
	fondo->setPositionUI(0.5, 0.05);
	fondo->setScaleUI(0.85, 0.7);
	fondo->setTexture(backgroundTexID);
	canvas->addElement(fondo);

	// 
	text = new Text("DEBUG INFO", canvas, { 0, 0, 0, 1 });
	text->setPositionUI(0.27, 0.63);
	text->setGrosor(1.25);
	text->setScaleUI(0.8, 0.8);
	text->setParent(fondo);
	//canvas->addElement(text);

	visible = true;
}

void DebugText::update(GLuint deltaTime)
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
		Game::DebugInfo info = Game::Instance()->debugInfo;
		std::string s = "FPS " + std::to_string(info.fps) +
			"\nVERTICES " + std::to_string(info.numVerts) +
			"\nTRIANGLES " + std::to_string(info.numTris) +
			"\nTEXTURE BINDS " + std::to_string(info.numTextureBinds) +
			"\nBUFFER SIZE " + std::to_string(info.fbSize.x) + "X" + std::to_string(info.fbSize.y) +
			"\nLIGHTS " + std::to_string(info.numLuces) +
			"\nTRANS ENT " + std::to_string(info.numTrans) + 
			"\nSHADER CHANGES " + std::to_string(info.programChanges);

		text->setText(s);
	}
#endif
}

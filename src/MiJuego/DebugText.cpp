#include "DebugText.h"

#include "Game.h"
#include "Text.h"
#include "Button.h"

DebugText::DebugText(Canvas* canvas, const std::string& backgroundTexID)
{
	//
	fondo = new Button(backgroundTexID, canvas);
	fondo->setPositionUI(0.5, 0.1);
	fondo->setScaleUI(0.8, 0.6);
	canvas->addElement(fondo);

	// 
	text = new Text("DEBUG INFO", canvas, { 0, 0, 0, 1 });
	text->setPositionUI(0.32, 0.16);
	text->setGrosor(1.25);
	text->setScaleUI(0.5, 0.5);
	canvas->addElement(text);
}

void DebugText::update(GLuint deltaTime)
{
	// Información de debug una vez hemos acabado de dibujar todo
#ifdef __DEBUG_INFO__
	std::string s = "FPS " + std::to_string(Game::Instance()->debugInfo.fps) +
		"\nVERTICES " + std::to_string(Game::Instance()->debugInfo.numVerts) +
		"\nTRIANGLES " + std::to_string(Game::Instance()->debugInfo.numTris) +
		"\nTEXTURE BINDS " + std::to_string(Game::Instance()->debugInfo.numTextureBinds) +
		"\nBUFFER SIZE " + std::to_string(Game::Instance()->debugInfo.fbSize.x) + "X" + 
		std::to_string(Game::Instance()->debugInfo.fbSize.y) +
		"\nLIGHTS " + std::to_string(Game::Instance()->debugInfo.numLuces);

	text->setText(s);
#endif
}

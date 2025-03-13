#ifndef __DEBUG_TEXT__
#define __DEBUG_TEXT__

#include "Component.h"

class Canvas;
class Text;
class Button;

class DebugText : public Component
{
public:
	DebugText(Canvas* canvas, const std::string& backgroundTexID);
	~DebugText(){}

	void update(GLuint deltaTime) override;
private:
	bool visible;
	Text* text;
	Button* fondo;

	bool physicsScene;
};

#endif
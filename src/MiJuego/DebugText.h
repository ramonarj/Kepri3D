#ifndef __DEBUG_TEXT__
#define __DEBUG_TEXT__

#include "Component.h"

class Canvas;
class Text;
class Button;
class UIElement;
class Piano;

class DebugText : public Component
{
public:
	DebugText(Canvas* canvas, const std::string& backgroundTexID);
	~DebugText(){}

	void update(float deltaTime) override;
private:
	bool visible;
	Text* text;
	Button* fondo;

	std::string sceneName;
	bool physicsScene;	
};

#endif
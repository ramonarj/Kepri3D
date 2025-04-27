#ifndef __ANIMATION__
#define __ANIMATION__

#include "Component.h"
#include <map>

class AnimationClip;

class Animacion : public Component
{
public:
	Animacion() : activeClip(nullptr) {}
	Animacion(AnimationClip* animClip);
	~Animacion();

	void update(float deltaTime) override;

	void play(const std::string& clipName);
	void pause(const std::string& clipName);
	void restart(const std::string& clipName);
private:
	std::map<std::string, AnimationClip*> clips;
	AnimationClip* activeClip;
};

#endif
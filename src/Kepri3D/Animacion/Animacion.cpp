#include "Animacion.h"

#include "AnimationClip.h"

Animacion::Animacion(AnimationClip* animClip) : activeClip(animClip)
{
	clips[animClip->name] = animClip;
}

void Animacion::update(float deltaTime)
{
	// Por el momento solo reproduce 1 clip
	activeClip->update(deltaTime);
}

void Animacion::play(const std::string& clipName)
{
	activeClip = clips[clipName];
	activeClip->play();
}

void Animacion::pause(const std::string& clipName)
{
	clips[clipName]->pause();
}

void Animacion::restart(const std::string& clipName)
{
	clips[clipName]->restart();
}

Animacion::~Animacion()
{
	CleanMap(clips);
}
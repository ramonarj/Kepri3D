#ifndef __TERRENO_COMP__
#define __TERRENO_COMP__

#include "Component.h"

class Material;
class Camera;
class Texture;

class Terreno : public Component
{
public:
	Terreno(Material* mat);
	~Terreno(){}

	void update(float deltaTime) override;

	inline void setCamera(Camera* cam) { this->cam = cam; }
	void setHeightMap(const std::string& hMap, float elevacion);

	// Temporalmente públicos
	bool useEyedir;
	float elevacion;
private:
	Material* mat;
	Camera* cam;

	Texture* heightMap;
	int patchSize;
};

#endif
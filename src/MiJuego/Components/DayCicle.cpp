#include "DayCicle.h"

#include "Kepri3D.h"

const glm::vec3 CIELO_DIA = { 1, 1, 1 };
const glm::vec3 CIELO_NOCHE = { 0.08, 0.06, 0.12 };
const glm::vec3 LUZ_DIA = { 1, 1, 1 };
const glm::vec3 LUZ_OCASO = { 0.7, 0.4, 0.2 };

DayCicle::DayCicle(int dayDuration)
{
	// Sol
	this->sol = new Light(DIRECTIONAL_LIGHT);
	sol->setDirection({ -1, 0 , 0 });
	sol->setDiffuse({ 1, 1, 1 });
	sol->setAmbient({ 0.05, 0.05, 0.2 });
	// Añadir el sol a la escena
	Entity* solEnt = new Entity();
	solEnt->addComponent(sol);
	Game::Instance()->getScene()->AddEntity(solEnt);

	// Cielo
	this->cielo = new Skybox("lakeSkybox");
	Game::Instance()->getScene()->SetSkybox(cielo);

	//
	velSol = 0.15;
}

DayCicle::DayCicle(Light* sol, Skybox* cielo)
{
	this->sol = sol;
	this->cielo = cielo;
	this->luna = nullptr;
}

void DayCicle::start()
{
	// Luna
	this->luna = new Light(DIRECTIONAL_LIGHT);
	luna->setDirection({ 1, 0 , 0 });
	luna->setDiffuse({ 0.0, 0.0, 0.0 });
	luna->setSpecular({ 0.2, 0.2, 0.2 });
	luna->setAmbient({ 0.05, 0.05, 0.2});
	luna->setActive(false);
	// Añadir la luna a la escena
	Entity* lunaEnt = new Entity();
	lunaEnt->addComponent(luna);
	Game::Instance()->getScene()->AddEntity(lunaEnt);
}

void DayCicle::update(float deltaTime)
{
	// Pausar el movimiento
	if (InputManager::Instance()->getKeyDown('t'))
	{
		running = !running;
	}

	// Apagar el Sol
	if (InputManager::Instance()->getKeyDown('l'))
	{
		sol->setActive(!sol->isActive());
	}

	// Actualizar la orientación de los rayos y el color difuso
	if (running)
	{
		// Orientación
		sol->setDirection({ -cos(totalTime * velSol),
			-sin(totalTime * velSol), 0 });
		luna->setDirection({ cos(totalTime * velSol),
	sin(totalTime * velSol), 0 });

		// Cambiar el color del cielo y de la luz
		float luminosidad = (sol->getDirection().y + 1) / 2.0f;
		Camera::setBackgroundColor(luminosidad, luminosidad, luminosidad);
		//cielo->getMaterial()->setVec3("color", { 1, 0, 0 });

		// Cambiar el color de la luz
		float l = abs(sol->getDirection().x);
		if(day)
		{
			cielo->getMaterial()->setVec3("color", Kepri::lerp(CIELO_DIA, LUZ_OCASO, l));
			sol->setDiffuse(Kepri::lerp(LUZ_DIA, LUZ_OCASO, l));
		}
		else
		{
			l = (1 - l);
			l += l * 3; // queremos que la noche profunda dure más que el atardecer
			cielo->getMaterial()->setVec3("color", Kepri::lerp(LUZ_OCASO, CIELO_NOCHE, l));
		}
			
		//sol->setDiffuse({ luminosidad, luminosidad, luminosidad, 1.0 });

		// Cambio del día a la noche y viceversa
		if (day && sol->getDirection().y < 0) // ajustar los ángulos para mayor realismo
		{
			day = false;
			sol->setActive(false);
			luna->setActive(true);
			//cielo->getMaterial()->setVec3("color", CIELO_NOCHE);
		}
		else if(!day && sol->getDirection().y > 0)
		{
			day = true;
			sol->setActive(true);
			luna->setActive(false);
			//cielo->getMaterial()->setVec3("color", CIELO_DIA);
		}

		totalTime += deltaTime;
	}
}
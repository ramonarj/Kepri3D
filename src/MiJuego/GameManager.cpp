#include "GameManager.h"

#include "../Kepri3D.h"
#include "Animacion/Animacion.h"

#include "../Scene.h"
#include "CameraController.h"

#include <freeglut.h>
#include <iostream>


void GameManager::setLights(Light* circleLight, Light* spotLight, Light* luzBlinn)
{
	this->circleLight = circleLight;
	this->spotLight = spotLight;
	this->luzBlinn = luzBlinn;

	
	//this->dirLight->setActive(false);

	this->spotLight->getEntity()->setPosition(cam->getPosition());
	this->spotLight->setSpotlightDir(cam->forward());
	this->spotLight->setSpotlightCutoff(65.0f);
	this->spotLight->setSpotlightHardness(100);
	this->spotLight->setAttenuationFactors(0, 0.04, 0.0002);

	this->circleLight->setAttenuationFactors(1, 0.1, 0.0001);
	this->circleLight->setActive(true);
	// 
	//cam->setPosition({ 60, 8, 8 });
	windowDim = { cam->getVP()->getW(),  cam->getVP()->getH() };
}

void GameManager::start()
{
	// Animaciones
	if(circleLight != nullptr)
	{
		animValue = 0;

		// De posición X (double)
		double* posX = (double*) & (circleLight->getEntity()->getModelMat()[3].x);
		//entity->addComponent(new Animacion<double>(posX, 0, 100, 5, true));

		// De posición (dvector3)
		glm::dvec3* pos = (glm::dvec3*)&(circleLight->getEntity()->getModelMat()[3]);
		Animacion<glm::dvec3>* positionAnim = new Animacion<glm::dvec3>(pos, { 0, 0, 0 }, { 10, 10, 10 }, 6, true);
		entity->addComponent(positionAnim);

		// De colores (vector4)
		glm::fvec4* colorLuz = (glm::fvec4*)&(circleLight->getEntity()->getComponent<Light>()->getDiffuse4());
		//entity->addComponent(new Animacion<glm::fvec4>(colorLuz, { 1, 1, 1, 0 }, { 1, 0, 0, 0 }, 1.5, true));

		glm::vec4* colorMat = (glm::vec4*)&(circleLight->getEntity()->getRenderer()->getMaterial()->getDiffuse());
		//entity->addComponent(new Animacion<glm::fvec4>(colorMat, { 1, 1, 1, 1 }, { 1, 0, 0, 1 }, 1.5, true));

		glm::vec4* colorEmision = (glm::vec4*)&(circleLight->getEntity()->getRenderer()->getMaterial()->getEmission());
		//entity->addComponent(new Animacion<glm::fvec4>(colorEmision, { 1, 1, 1, 1 }, { 1, 0, 0, 1 }, 1.5, true));

		// De rotaciones (glm::dmat4)
		/*
		glm::dmat4 ini = cam->getModelMat();
		cam->lookAt({ 100, 15, 0 });
		glm::dmat4 objetivo = cam->getModelMat();
		cam->lookAt({ 0, 0, 0 });
		glm::dmat4* rot = (glm::dmat4*)&(cam->getModelMat());
		entity->addComponent(new Animacion<glm::dmat4>(rot, ini, objetivo, 3, true));
		*/

		// Keyframes
		positionAnim->addKeyframe({ -10, 5, 0 }, 4);
		positionAnim->addKeyframe({ 10, 5, 0 }, 2);
		positionAnim->addKeyframe({ 0, 0, 0 }, 8); // para que no se note el bucle
	}
}

void GameManager::update(float deltaTime)
{
	// Seleccionar entidades usando el RaycastFromScreen
	if (InputManager::Instance()->getMouseKeyDown(LEFT) && !lockedMouse)
	{
		glm::ivec2 mousePos = InputManager::Instance()->getMousePos();
		// Tira un rayo desde el ratón
		Collider* col = PhysicsSystem::Instance()->raycastFromScreenPro(mousePos, 100);
		if (col != nullptr)
		{
			std::cout << "Seleccionado " << col->getEntity()->getName() << std::endl;
			Renderer* rend = col->getEntity()->getComponent<Renderer>();
			if(rend->getPolygonMode(0) == GL_FILL)
				rend->setPolygonMode(GL_LINE, GL_LINE);
			else
				rend->setPolygonMode(GL_FILL, GL_LINE);
		}		
	}

	// Salir de la aplicación / desbloquear el ratón
	if (InputManager::Instance()->getKeyDown(27)) // escape
	{
		// Desbloquear el ratón
		if (lockedMouse)
		{
			lockedMouse = false;
			// Podemos seleccionar el cursor que más nos guste!!
			InputManager::Instance()->setCursor(GLUT_CURSOR_RIGHT_ARROW);

			// Mostrar el panel de botones
			if(botonesMenu != nullptr)
				botonesMenu->setActive(true);

			// Desactivar el componente hermano que controla la cámara
			entity->getComponent<CameraController>()->setActive(false);
		}
		// Salir del juego
		else
			Game::Instance()->exitGame();
	}

	// Controlar las luces
	controlLuces(deltaTime);

	// Controlar el sist. particulas
	controlMovimiento(pSystem, deltaTime); //circleLight->getEntity()

	// Controlar el terreno
	controlTerreno(deltaTime);


	// Bloquear el ratón con el clic derecho
	if (InputManager::Instance()->getMouseKey(RIGHT))
	{
		if (!lockedMouse)
		{
			lockedMouse = true;
			InputManager::Instance()->setCursor(GLUT_CURSOR_NONE);
			InputManager::Instance()->setMousePos(cam->getVP()->getW() / 2, cam->getVP()->getH() / 2);

			// Esconder el panel de botones
			if(botonesMenu != nullptr)
				botonesMenu->setActive(false);
			// Activar el componente hermano
			entity->getComponent<CameraController>()->setActive(true);
		}	
	}


	// Modo pantalla completa (entrar / salir)
	if(InputManager::Instance()->getSpecialKeyDown(GLUT_KEY_F11))
	{
		fullscreen = !fullscreen;
		if (fullscreen)
		{
			windowDim = { cam->getVP()->getW(),  cam->getVP()->getH() };
			glutFullScreen();
		}
		else
			glutReshapeWindow(windowDim.x, windowDim.y);
	}

	// Hacer una foto
	//if (InputManager::Instance()->getKeyDown('f'))
	//{
	//	Texture::save("foto.bmp", GL_BACK);
	//	std::cout << "Se ha hecho una foto" << std::endl;
	//}

	// Cambiar entre el modelo Phong y el Blinn-Phong
	if(InputManager::Instance()->getKeyDown('i'))
		scene->switchBlinnPhong();

	// Fijar la cámara en la luz circular
	if (InputManager::Instance()->getKey('k'))
		cam->lookAt(circleLight->getEntity()->getPosition());
}


void GameManager::controlLuces(float deltaTime)
{
	if (spotLight == nullptr) { return; }

	// 1) Luz de foco (linterna)
	// Posición y dirección
	spotLight->getEntity()->setPosition(cam->getPosition());
	// Un poco feo pero funciona
	spotLight->setSpotlightDir(cam->forward());
	// Encenderla / apagarla
	if (lockedMouse && InputManager::Instance()->getMouseKeyDown(LEFT))
	{
		spotLight->setActive(!spotLight->isActive());
	}

	// 2) Luz puntual
	// Trayectoria circular
	//if (movingLights)
	//	circleLight->setPosition({ 20 * cos(totalTime * lightVel), 1, 8 * sin(totalTime * lightVel) });
	
	// Cambiar el color con el Enter
	if (InputManager::Instance()->getKeyDown(13))
	{
		float r = int(rand() % 255) / 255.0f;
		float g = int(rand() % 255) / 255.0f;
		float b = int(rand() % 255) / 255.0f;

		circleLight->setDiffuse({ r, g, b });
		circleLight->setSpecular({ r, g, b });
		((Material*)circleLight->getEntity()->getMaterial())->setEmission(circleLight->getDiffuse());
	}

	totalTime += deltaTime;
}

void GameManager::controlMovimiento(Entity* e, float deltaTime)
{
	glm::dvec3 direccion = { 0,0,0 };

	// Moverse iza/dcha
	if(InputManager::Instance()->getSpecialKey(GLUT_KEY_RIGHT))
		direccion.x += 1;
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_LEFT))
		direccion.x -= 1;

	// Moverse adelante / atrás
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_UP))
		direccion.z += -1;
	if (InputManager::Instance()->getSpecialKey(GLUT_KEY_DOWN))
		direccion.z += 1;

	// Moverse arriba / abajo
	if (InputManager::Instance()->getKey('9'))
		direccion.y += -1;
	if (InputManager::Instance()->getKey('0'))
		direccion.y += 1;

	// Mover la luz circular
	direccion = direccion * velTorre * (double)deltaTime;
	e->translate(direccion, GLOBAL);
}

void GameManager::controlTerreno(float deltaTime)
{
	if (tessTerrain == nullptr)
		return;

	// Decirle al terreno en cuántos parches dividirse
	if (InputManager::Instance()->getKeyDown('o'))
		tessTerrain->useEyedir = !tessTerrain->useEyedir;

	// Modificar la elevación máxima del terreno
	if (InputManager::Instance()->getKey('h'))
		tessTerrain->elevacion += deltaTime * 5.0;
	if (InputManager::Instance()->getKey('b'))
		tessTerrain->elevacion -= deltaTime * 5.0;
}

void GameManager::centerMouse()
{
	InputManager::Instance()->setMousePos(Game::Instance()->getCamera()->getVP()->getW() / 2.0, 
		Game::Instance()->getCamera()->getVP()->getH() / 2.0);
}
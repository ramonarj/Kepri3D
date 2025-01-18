#include "../checkML.h"
#include "../Game.h"

#include "PruebaScene.h"
#include "SpaceScene.h"

int main(int argc, char*argv[])
{
	// Detección de basura dinámica
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	// 0) Cargar la escena inicial del juego
	Scene* prueba = new PruebaScene();
	//Scene* espacio = new SpaceScene();
	Game::Instance()->loadScene(prueba);

	// 1) Crear e inicializar el juego con las dimensiones dadas
	Game::Instance()->init(argc, argv, 800, 600, "Mi juego");

	// 2) Bucle de 'juego' (lo maneja glut)
	Game::Instance()->run();

	// 3) Destruir el juego y salir
	Game::Instance()->clean();

	return 0;
}
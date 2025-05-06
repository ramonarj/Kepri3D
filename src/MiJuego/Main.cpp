#include "checkML.h"
#include "Kepri3D.h"

#include "Scenes/PruebaScene.h"
#include "Scenes/PruebaSonido.h"
#include "Scenes/PruebaFisicas.h"
#include "Scenes/Sandbox.h"

// Necesario para que no se quejen por la consola
#ifndef _DEBUG
#	ifdef _WIN32
#		pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#	endif
#endif

int main(int argc, char*argv[])
{
	// Detección de basura dinámica
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	// 1) Crear e inicializar el juego con las dimensiones dadas
	Game::Instance()->init(argc, argv, 800, 600, "Mi juego");

	// 2) Cargar la escena inicial
	//Scene* escena = new PruebaScene();
	Scene* escena = new PruebaFisicas();
	//Scene* escena = new PruebaSonido();
	//Scene* escena = new Sandbox();
	Game::Instance()->loadScene(escena);

	// 3) Bucle de 'juego' (lo maneja glut)
	Game::Instance()->run();

	// 4) Destruir el juego y salir
	Game::Instance()->clean();

	return 0;
}
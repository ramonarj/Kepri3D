#include "checkML.h"
#include "Game.h"

int main(int argc, char*argv[])
{
	// Detecci�n de basura din�mica
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 

	// 1) Crear e inicializar el juego con las dimensiones dadas
	Game::Instance()->init(argc, argv, 800, 600);

	// 2) Bucle de 'juego' (lo maneja glut)
	Game::Instance()->run();

	// 3) Destruir el juego y salir
	Game::Instance()->clean();

	return 0;
}
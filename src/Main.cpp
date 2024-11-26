//#include <windows.h>
//#include <gl/GL.h> // la versión incluida en Windows es muy antigua, usaremos la que incluye freeglut

#include <glm.hpp>
#include <freeglut.h>

// Hay que usar:
// 1) OpenGL
// 2) Open GL Mathematics (glm) -> tipos definidos, como vectores y matrices de distinto tipo
// 3) FreeGLUT -> creación de ventanas e input del usuario (reemplazable por SDL, por ejemplo)
// Nota: FreeGLUT ya incluye OpenGL de por sí
// La configuración se mantendrá en Release/x64, al menos por el momento

#include <iostream>
#include "Scene.h"

// Variables globales
Scene scene;

bool g_depthTest = true;

// Predeclaración de callbacks
void display();
void key(unsigned char key, int x, int y);

int main(int argc, char*argv[])
{
	std::cout << "Starting console..." << '\n';

	// 1) Inicializar GLUT y crear la ventana
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(800, 600);   // window size
	//glutInitWindowPosition (140, 140);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);   // | GLUT_STENCIL  

	int win = glutCreateWindow("Informática Gráfica");  // nombre de la ventana

	// 2) Registrar los distintos callbacks (input) 
	//glutReshapeFunc(resize);
	// Se llama cuando se pulsa una tecla
	glutKeyboardFunc(key);
	//glutSpecialFunc(specialKey);
	// Se llama cuando la ventana se redibuja
	glutDisplayFunc(display);

	//glutIdleFunc(update); //Callbacks nuevas
	//glutMouseFunc(mouse);
	//glutMotionFunc(motion);

	// Información de debug
	std::cout << glGetString(GL_VERSION) << '\n';
	std::cout << glGetString(GL_VENDOR) << '\n';

	// 3) Iniciar la escena una vez creado el contexto
	scene.init();  

	// 4) Bucle de 'juego' (lo maneja glut)
	glutMainLoop();

	// 5) Destruir el contexto y salir
	glutDestroyWindow(win);  

	return 0;
}

void display()   // double buffer
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.render();

	glutSwapBuffers();
}

void key(unsigned char key, int x, int y)
{
	switch(key)
	{
	/* Activar/desactivar el uso del Z-buffer (DEPTH_TEST) */
	case 'z':
		if(g_depthTest)
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
		g_depthTest = !g_depthTest;
		break;
	}

	// Obliga a que la ventana se vuelva a pintar
	glutPostRedisplay();
}
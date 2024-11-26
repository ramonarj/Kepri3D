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

// Predeclaración de callbacks
void display();
void key(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);

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
	// Se llama cuando se pulsa una tecla especial (flechas de dirección)
	glutSpecialFunc(specialKey);
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
		if(glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
		break;

	/* Activar/desactivar las transparencias (BLEND) */
	case 'b':
		if (glIsEnabled(GL_BLEND))
			glDisable(GL_BLEND);
		else
			glEnable(GL_BLEND);
		break;

	/* Activar/desactivar el alpha test (ALPHA_TEST) */
	case 'a':
		if (glIsEnabled(GL_ALPHA_TEST))
			glDisable(GL_ALPHA_TEST);
		else
			glEnable(GL_ALPHA_TEST);
		break;
	}

	// Obliga a que la ventana se vuelva a pintar
	glutPostRedisplay();
}

void specialKey(int key, int x, int y)
{
	bool need_redisplay = true;

	switch (key) {
		float alfaValue;
		/* Subir el mínimo de alfa para el ALPHA_TEST */
	case GLUT_KEY_RIGHT:
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
		glAlphaFunc(GL_GREATER, alfaValue + 0.05);
		break;
		/* Bajar el mínimo de alfa para el ALPHA_TEST */
	case GLUT_KEY_LEFT:
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
		glAlphaFunc(GL_GREATER, alfaValue - 0.05);
		break;
	default:
		need_redisplay = false;
		break;
	}//switch

	if (need_redisplay)
		glutPostRedisplay();
}
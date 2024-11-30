//#include <windows.h>
//#include <gl/GL.h> // la versi�n incluida en Windows es muy antigua, usaremos la que incluye freeglut

#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <freeglut.h>

// Hay que usar:
// 1) OpenGL
// 2) Open GL Mathematics (glm) -> tipos definidos, como vectores y matrices de distinto tipo
// 3) FreeGLUT -> creaci�n de ventanas e input del usuario (reemplazable por SDL, por ejemplo)
// Nota: FreeGLUT ya incluye OpenGL de por s�
// La configuraci�n se mantendr� en Release/x64, al menos por el momento

#include <iostream>
#include "Scene.h"
#include "Camera.h"
#include "Entity.h"

// Variables globales
Viewport viewport(800, 600);
// Tambi�n podr�a hacerse con esto
//	/*int windowW = glutGet(GLUT_WINDOW_WIDTH);
//	int windowH = glutGet(GLUT_WINDOW_HEIGHT);*/
Camera camera(&viewport);
Scene scene(&camera);

GLuint last_update_tick = 0;
bool animationsOn = true;
bool fullscreen = false;

// Predeclaraci�n de callbacks
void display();
void key(unsigned char key, int x, int y);
void specialKey(int key, int x, int y);
void resize(int newWidth, int newHeight);
void update();

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

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // | GLUT_MULTISAMPLE);   // | GLUT_STENCIL

	int win = glutCreateWindow("Inform�tica Gr�fica");  // nombre de la ventana

	//glutFullScreen(); // pantalla completa

	// 2) Registrar los distintos callbacks (input) 
	// Se llama cuando la ventana es redimensionada (tirando de los bordes/cambiando a pantalla completa)
	glutReshapeFunc(resize);
	// Se llama cuando se pulsa una tecla
	glutKeyboardFunc(key);
	// Se llama cuando se pulsa una tecla especial (flechas de direcci�n)
	glutSpecialFunc(specialKey);
	// Se llama cuando la ventana se redibuja
	glutDisplayFunc(display);

	// Se llama cada frame, aunque no se reciban eventos de la ventana (para animaciones)
	glutIdleFunc(update);
	//glutMouseFunc(mouse);
	//glutMotionFunc(motion);

	// Informaci�n de debug
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
	// Limpia el color y el depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene.render();

	// Hay 2 buffers; uno se est� mostrando por ventana, y el otro es el que usamos
	// para dibujar con la GPU. Cuando se ha terminado de dibujar y llega el siguiente 
	// frame, se intercambian las referencias y se repite el proceso
	glutSwapBuffers();
}

void key(unsigned char key, int x, int y)
{
	bool need_redisplay = true;

	switch(key)
	{
		float alfaValue;
	/* Tecla de escape: salir de la aplicaci�n */
	case 27:  
		glutLeaveMainLoop();
		break;
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
	case 't':
		if (glIsEnabled(GL_ALPHA_TEST))
			glDisable(GL_ALPHA_TEST);
		else
			glEnable(GL_ALPHA_TEST);
		break;
	// Pruebas con la c�mara
	case 'a':
		camera.moveLR(-0.02);
		break;
	case 'd':
		camera.moveLR(0.02);
		break;
	case 'w':
		camera.moveFB(0.02);
		break;
	case 's':
		camera.moveFB(-0.02);
		break;
	// Roll
	case '9':
		camera.roll(-0.1);
		break;
	case '0':
		camera.roll(0.1);
		break;
	// Movimiento arriba de la c�mara
	case 32: // espacio
		camera.moveUD(0.02);
		break;
	/* Bajar el m�nimo de alfa para el ALPHA_TEST */
	case '1':
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
        glAlphaFunc(GL_GREATER, alfaValue - 0.05);
		break;
	/* Subir el m�nimo de alfa para el ALPHA_TEST */
	case '2':
		glGetFloatv(GL_ALPHA_TEST_REF, &alfaValue);
		glAlphaFunc(GL_GREATER, alfaValue + 0.05);
		break;
	/* Enter: activar/desactivar animaciones */
	case 13:
		animationsOn = !animationsOn;
		break;
	default:
		need_redisplay = false;
		break;
	}

	// Obliga a que la ventana se vuelva a pintar
	if (need_redisplay)
		glutPostRedisplay();
}


void specialKey(int key, int x, int y)
{
	bool need_redisplay = true;

	switch (key) {
	/* Rotaciones locales de la c�mara (pitch, yaw, roll) */
	case GLUT_KEY_RIGHT:
		camera.yaw(-0.1);
		break;
	case GLUT_KEY_LEFT:
		camera.yaw(0.1);
		break;
	case GLUT_KEY_UP:
		camera.pitch(0.1);
		break;
	/* Movimiento arriba/abajo de la c�mara */
	case GLUT_KEY_DOWN:
		camera.pitch(-0.1);
		break;
	case GLUT_KEY_SHIFT_L:
		camera.moveUD(-0.05);
		break;
	/* Modo pantalla completa (entrar/salir) */
	case GLUT_KEY_F11:
		fullscreen = !fullscreen;
		if(fullscreen)
			glutFullScreen();
		else
			glutReshapeWindow(800, 600);
		break;
	default:
		need_redisplay = false;
		break;
	}//switch

	if (need_redisplay)
		glutPostRedisplay();
}

void resize(int newWidth, int newHeight)
{
	// Resize Viewport 
	viewport.setSize(newWidth, newHeight);
	// Resize Scene Visible Area 
	//camera.setSize(viewPort.getW(), viewPort.getH());    // scale unchanged
}

void update()
{
	GLuint deltaTime = glutGet(GLUT_ELAPSED_TIME) - last_update_tick;
	last_update_tick = glutGet(GLUT_ELAPSED_TIME);
	if(animationsOn)
	{
		scene.update(deltaTime); //Llamamos al update y al render
		display();
	}
}
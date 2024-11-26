#include <windows.h>
#include <gl/GL.h>

#include <glm.hpp>

#include <freeglut.h>

// Hay que usar:
// 1) OpenGL
// 2) Open GL Mathematics (glm) -> tipos definidos, como vectores y matrices de distinto tipo
// 3) FreeGLUT -> creación de ventanas e input del usuario (reemplazable por SDL, por ejemplo)

// La configuración se mantendrá en Release/x64, al menos por el momento

#include <iostream>

int main(int argc, char*argv[])
{
	std::cout << "Starting console..." << '\n';

	// Initialization
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitWindowSize(800, 600);   // window size
	//glutInitWindowPosition (140, 140);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);   // | GLUT_STENCIL  

	int win = glutCreateWindow("Informática Gráfica");  // window's identifier

	// Registrar los callbacks
	//glutReshapeFunc(resize);
	//glutKeyboardFunc(key);
	//glutSpecialFunc(specialKey);
	//glutDisplayFunc(display);

	//glutIdleFunc(update); //Callbacks nuevas
	//glutMouseFunc(mouse);
	//glutMotionFunc(motion);


	std::cout << glGetString(GL_VERSION) << '\n';
	std::cout << glGetString(GL_VENDOR) << '\n';

	//scene.init();    // after creating the context

	glutMainLoop();


	//cin.sync();   cin.get();
	glutDestroyWindow(win);  // Destroy the context 

	return 0;
}
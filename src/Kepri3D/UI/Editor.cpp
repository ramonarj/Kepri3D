#include "Editor.h"

#include "Kepri3D.h"

int entSeleccion;

Editor::Editor(Game* game, int w, int h)
{
	this->pGame = game;

	glutInitWindowSize(w, h);
	glutInitWindowPosition(250 + 800, 100);
	editorWindow = glutCreateWindow("Kepri3D Editor");

	// Callbacks
	glutDisplayFunc(editorDisplay);
	//glutFullScreen();
}

// Callbacks para los eventos del editor
void editorDisplay()
{
	//Game::Instance()->render();
}

void accionesMenu(int accion)
{
	Entity* ent = Game::Instance()->getScene()->getEntity(glutGetMenu() - 1);
	switch (accion)
	{
		// Activa/inactiva
	case 0:
		ent->setActive(!ent->isActive());
		break;
		// Modo wireframe
	case 1:
		ent->getComponent<Renderer>()->setPolygonMode(GL_LINE, GL_LINE);
		break;
		// Mostrar Bounding Box
	case 2:
		ent->getComponent<Renderer>()->visibleBounds = !ent->getComponent<Renderer>()->visibleBounds;
		break;
		// Añadirle un Rigid
	case 3:
	{
		Rigid* r = new Rigid(ent->getModelMat());
		ent->addComponent(r);
		PhysicsSystem::Instance()->addRigid(r);
		break;
	}

	default:
		break;
	}
}

void creationMenu(int eleccion)
{
	switch (eleccion)
	{
		// Crear un cubo
	case 0:
	{
		Game::Instance()->getScene()->AddEntity(new Cubo(2));
		break;
	}
	// Crear una esfera
	case 1:
		Game::Instance()->getScene()->AddEntity(new Esfera());
		break;
		// Crear un cilindro
	case 2:
		Game::Instance()->getScene()->AddEntity(new Cilindro());
		break;
		// Crear un toro
	case 3:
		Game::Instance()->getScene()->AddEntity(new Toro());
		break;
	default:
		break;
	}

	glutPostRedisplay();
}

void Editor::updateMenuInfo()
{
	glutSetWindow(editorWindow);

	int disp, entidades;

	/* Submenú de acciones */ //(desafortunadamente, hay que crear uno por entidad)
	std::vector<int>acciones;
	for (Entity* e : pGame->scene->m_entities)
	{
		acciones.push_back(glutCreateMenu(accionesMenu));
		glutAddMenuEntry("Activar", 0);
		glutAddMenuEntry("Wireframe", 1);
		glutAddMenuEntry("Bounding Box", 2);
		glutAddMenuEntry("Add Rigid", 3);
	}
	/* Submenú 2 */
	disp = glutCreateMenu(accionesMenu);
	glutAddMenuEntry("B", 0);

	/* Menu que muestra todas las entidades de la escena */
	entidades = glutCreateMenu(accionesMenu);
	int i = 0;
	for (Entity* e : pGame->scene->m_entities)
	{
		glutAddSubMenu(e->getName().c_str(), acciones[i]);
		i++;
		// Convierte un submenú a una entrada normal
		//glutChangeToMenuEntry(1, "hola", 0);
		//glutChangeToSubMenu(1, "hola", 0);
	}
	glutAttachMenu(GLUT_LEFT_BUTTON);

	/* Menu para crear entidades */
	int creacion = glutCreateMenu(creationMenu);
	glutAddMenuEntry("Crear cubo", 0);
	glutAddMenuEntry("Crear esfera", 1);
	glutAddMenuEntry("Crear cilindro", 2);
	glutAddMenuEntry("Crear toro", 3);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

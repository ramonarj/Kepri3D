#ifndef __COMPONENT__
#define __COMPONENT__

#include <glew.h>
#include "Entity.h"

/* Clase abstracta para que herede cualquier componente */
class Component
{
public:
	Component() : m_active(true), entity(nullptr) {}
	virtual ~Component(){}

	// Getters
	/* Devuelve true si el componente se actualiza cada frame, false e.o.c. */
	inline bool isActive() const { return m_active; }

	// Setters
	/* Activa / desactiva este componente */
	inline void setActive(bool b) { m_active = b; }

	/* Establece la referencia a nuestra entidad */
	inline void setEntity(Entity* e) { this->entity = e; }

	/* Método virtual puro que debe implementar cada tipo de componente */
	virtual void update(GLuint deltaTime) = 0;
protected:
	bool m_active;

	Entity* entity;
};

#endif
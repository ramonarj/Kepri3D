#ifndef __COMPONENT__
#define __COMPONENT__

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

	/* Devuelve la entidad contenedora de este componente */
	inline Entity* getEntity() const { return entity; }

	// Setters
	/* Activa / desactiva este componente */
	inline void setActive(bool b) { m_active = b; }

	/* Establece la referencia a nuestra entidad */
	inline void setEntity(Entity* e) { this->entity = e; }

	// Callbacks
	/* Se llama una vez al inicio de la escena */
	virtual void start() {}
	/* M�todo virtual puro que debe implementar cada tipo de componente. Se llama cada frame */
	virtual void update(float deltaTime) = 0;
	/* Callback que se llama en cada paso de la simulaci�n f�sica */
	virtual void fixedUpdate(float fixedDeltatime) {}
	/* Callback para cuando la entidad colisione con otra */
	virtual void onCollision(Collider* other) {}
	/* Callback para cuando penetre en un collider tipo trigger */
	virtual void onTrigger(Collider* other) {}
protected:
	bool m_active;

	Entity* entity;
};

#endif
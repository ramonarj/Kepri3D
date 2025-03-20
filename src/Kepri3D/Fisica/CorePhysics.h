#ifndef __CORE_PHYSICS__
#define __CORE_PHYSICS__

#include <glm.hpp>

#define DOUBLE_PRECISION

/* Establecer la precisión que tendrá el sistema (float/double) */
#ifdef DOUBLE_PRECISION
	// Doble
	typedef double real;
	typedef glm::dvec2 vector2;
	typedef glm::dvec3 vector3;
	typedef glm::dvec4 vector4;
#else
	// Simple
	typedef float real;
	typedef glm::vec2 vector2;
	typedef glm::vec3 vector3;
	typedef glm::vec4 vector4;
#endif


#endif
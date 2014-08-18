#ifndef PHYSICS_H
#define PHYSICS_H

#include "Types.hpp"
#include "HelperLibString.hpp"

#ifdef ODE
#include "ode/ode.h"
#endif

class Physics{
	private:
		#ifdef ODE
			dWorldID worldId;
			dSpaceID spaceId;
		#endif
	public:
		FloatVector3d gravity;
		String collide;
		Physics();
		~Physics();
		bool init();
		bool scriptInit();
		void perframe();
		bool registerObject(Object*);	//register an obj with the physics engine
		bool removeObject(Object*);
};

extern Physics physics;

#endif


#ifndef SPHERE_HPP
#define SPHERE_HPP 1

#include "Types.hpp"
#include "objects.hpp"
#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"
#include "animation.hpp"
#include <iostream>

class Sphere{
	public:
		FloatVector3d pos;
		FloatVector3d scale;
		FloatVector3d rot;
		FloatVector2d detail;
		FloatVector2d extent;

		bool visible;
		bool backface;

		SceneGraphNode* sceneGraphNode;

		String name;
		Sphere();
		void draw();
};


#endif
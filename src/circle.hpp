#ifndef CIRCLE_HPP
#define CIRCLE_HPP 1


#include "Types.hpp"
#include "objects.hpp"
#include "Containers.hpp"
#include "HelperLibString.hpp"
#include <iostream>

class Circle{
	public:
		float thickness;
		FloatVector4d color;
		FloatVector3d pos;
		FloatVector2d scale;
		bool percent;
		bool keepAspectRatio;
		bool visible;
		String name;
		Circle();
		void draw();

		SceneGraphNode* sceneGraphNode;
	private:

};

#endif

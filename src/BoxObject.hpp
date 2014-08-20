#ifndef BOX_HPP
#define BOX_HPP 1

#include "Types.hpp"
#include "objects.hpp"
#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"
#include "animation.hpp"
#include <iostream>

class Box{
	public:
		float thickness;
		FloatVector4d color;
		FloatVector3d pos;
		FloatVector3d scale;
		//bool percent;
		//bool keepAspectRatio;
		bool visible;
		String name;
		Box();
		void draw();

		SceneGraphNode* sceneGraphNode;
	private:
};

#endif

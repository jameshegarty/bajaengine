#ifndef LINE_H
#define LINE_H

#include "script.hpp"

class SceneGraphNode;

class Line3d{
	private:
	
	public:
		FloatVector3d pos;
		FloatVector3d scale;
		FloatVector4d color;
		float thickness;
		bool visible;
		
		SceneGraphNode* sceneGraphNode;

		String name;
		
		Line3d();
		void draw();
};

#endif


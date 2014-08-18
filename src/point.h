#ifndef POINT_H
#define POINT_H 

#include "script.hpp"

class SceneGraphNode;

class Point3d{
	private:
	
	public:
		FloatVector3d pos;
		FloatVector4d color;
		float scale;
		String name;
		bool visible;
		
		SceneGraphNode* sceneGraphNode;

		void draw();
		
		Point3d();
};


#endif


#ifndef CURVE_H
#define CURVE_H 

class SceneGraphNode;

#include "script.hpp"

class Curve{
	public:

		SceneGraphNode* sceneGraphNode;

		Array<FloatVector3d> controlPoints;

		FloatVector3d getPos(float);

		bool hasBasePose;
		FloatVector3d basePos;
		FloatVector3d baseRot;
		FloatVector3d baseScale;

		bool closed;
		unsigned int degree;
		bool visible;
		FloatVector3d pos;
		FloatVector3d rot;
		FloatVector3d scale;
		String name;
		Curve();
		void draw();
		void drawRaw();
};

int luaGetCurvePos(lua_State *L);

#endif

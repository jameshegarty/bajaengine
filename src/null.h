#ifndef NULL_H
#define NULL_H

#include "Types.hpp"
#include "objects.hpp"
#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"
#include "animation.hpp"
#include <iostream>

#ifndef XSICONVERT
	#include "script.hpp"
#endif

class ObjectAddress;
class SceneGraphNode;

class NullObject{
	private:
		Object getObj();

	public:
		SceneGraphNode* sceneGraphNode;

		String constraintPosName;
		unsigned int constraintPosType;
		String constraintRotName;
		unsigned int constraintRotType;
		String constraintScaleName;
		unsigned int constraintScaleType;
		ObjectAddress* constraintPos;
		ObjectAddress* constraintRot;
		ObjectAddress* constraintScale;

		Array<ObjectAnimation> animations;

		unsigned char type;	//0=point light,1=spot light
		unsigned int flags;

		bool visible;

		bool hasBasePose;
		FloatVector3d basePos;
		FloatVector3d baseRot;
		FloatVector3d baseScale;

		unsigned int sceneGraphLevel;

		FloatVector3d pos;
		FloatVector3d rot;
		FloatVector3d scale;
		FloatVector3d color;

		String name;

		void write(std::ofstream& out);
		unsigned long size();

		void draw();
		void update();
		void drawRaw();

		NullObject();
};

#endif

#ifndef IKCHAIN_H
#define IKCHAIN_H

#include "Types.hpp"
#include "Containers.hpp"
#include "HelperLibString.hpp"
#include "animation.hpp"
#include <iostream>

class ObjectAddress;
class SceneGraphNode;
class IkRoot;

const int ikSplit=16;

class IkJoint{
	public:
		SceneGraphNode* sceneGraphNode;

		Array<ObjectAnimation> animations;
		String name;
		bool visible;
		FloatVector3d pos;
		FloatVector3d rot;
		FloatVector3d scale;
		FloatVector3d preferedRot;


		unsigned int sceneGraphLevel;
		String constraintPosName;
		unsigned int constraintPosType;
		String constraintRotName;
		unsigned int constraintRotType;
		String constraintScaleName;
		unsigned int constraintScaleType;
		ObjectAddress* constraintPos;
		ObjectAddress* constraintRot;
		ObjectAddress* constraintScale;

		bool hasBasePose;
		FloatVector3d basePos;	//the root
		FloatVector3d baseRot;
		FloatVector3d baseScale;

		FloatVector3d localBasePos;		//local to the root global pos
		FloatVector3d localBaseRot;		//local to the root global rot
		FloatVector3d localBaseScale;	//local to the root global scale


		void write(std::ofstream& out);
		IkRoot* root;
		String rootName;

		float length;
		void draw();
		
		IkJoint();
};

class IkRoot{
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


		unsigned int sceneGraphLevel;
		Array<ObjectAnimation> animations;
		String name;
		bool visible;
		Array<IkJoint*> joints;
		Array<String> jointNames;
		FloatVector3d pos;	//the root
		FloatVector3d rot;
		FloatVector3d scale;

		void write(std::ofstream& out);

		bool hasBasePose;
		FloatVector3d basePos;	//the root
		FloatVector3d baseRot;
		FloatVector3d baseScale;

		void draw();
		void update();
		IkRoot();
};

class IkEffector{
	private:
		FloatVector3d lastPos;
		void solve();
		void solveOnce();
		FloatVector3d ikFsolve();
		FloatVector2d ikFsolve2d();
		float ikFsolveFirstOrder(int k);
		void fkSolve();
	public:	
		SceneGraphNode* sceneGraphNode;
		Array<ObjectAnimation> animations;
		String name;
		bool visible;
		FloatVector3d pos;
		FloatVector3d rot;
		FloatVector3d scale;

		unsigned int sceneGraphLevel;

		String constraintPosName;
		unsigned int constraintPosType;
		String constraintRotName;
		unsigned int constraintRotType;
		String constraintScaleName;
		unsigned int constraintScaleType;
		ObjectAddress* constraintPos;
		ObjectAddress* constraintRot;
		ObjectAddress* constraintScale;

		IkEffector();
		void write(std::ofstream& out);

		IkRoot* root;
		String rootName;
		void draw();
		void update();	//solve the chain
};


#endif


#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"
#include "objects.hpp"
#include "ObjectAddress.hpp"

class SceneGraphNode{
	public:
		bool draw;
		ObjectAddress object;
		SceneGraphNode* parent;
		SafeArray<SceneGraphNode> children;
		SceneGraphNode();
		SceneGraphNode** sceneGraphNode();
};

class SceneGraph{
	private:
		FloatVector3d global(ObjectAddress node,int t,FloatVector3d);
		FloatVector3d local(ObjectAddress node,FloatVector3d v,int t);
		Matrix4d4d getMatrixMain(SceneGraphNode* from,SceneGraphNode* to,bool reverse);
	public:
		SceneGraphNode root;
		Map<String,SceneGraphNode*> nameMap;

		void clearDrawList(SceneGraphNode* a=NULL);
		void enableDrawList(SceneGraphNode* a=NULL);

		bool nonstatic(SceneGraphNode*);	//returns true if any of the parents are nonstatic

		bool scriptInit();

		Matrix4d4d getMatrix(SceneGraphNode* from,SceneGraphNode* to);
		Matrix4d4d getMatrixReverse(SceneGraphNode* from,SceneGraphNode* to);

		void modelviewTransform(SceneGraphNode* from, SceneGraphNode* to);

		FloatVector3d globalPos(ObjectAddress object);	//returns the global pos for this obj, instead of the local pos stored in 'pos'
		FloatVector3d globalRot(ObjectAddress object);
		FloatVector3d globalScale(ObjectAddress object);

		FloatVector3d globalPos(ObjectAddress object,FloatVector3d pos);	//returns the global pos of 'pos', relative to the given parent.  if pos=[0,0,0], this is the same as globalPos()

		FloatVector3d globalBasePos(ObjectAddress object);	//returns the global pos for this obj, instead of the local pos stored in 'pos'
		FloatVector3d globalBaseRot(ObjectAddress object);
		FloatVector3d globalBaseScale(ObjectAddress object);

		FloatVector3d localPos(ObjectAddress object, FloatVector3d globalPos);
		FloatVector3d localRot(ObjectAddress object, FloatVector3d globalRot);
		FloatVector3d localScale(ObjectAddress object, FloatVector3d globalScale);

		bool remove(SceneGraphNode*);
		void draw(void (*callbackFunction)(ObjectAddress), bool preTransform=true);
};

#endif


#include "SceneGraph.hpp"
#include "HelperLibMath.hpp"
#include "level.hpp"
#include "instance.hpp"
#include "sort.hpp"

SceneGraph sceneGraph;

SceneGraphNode::SceneGraphNode(){
	draw=false;
	parent=NULL;
}


void drawRecurse(void (*callbackFunction)(ObjectAddress), SceneGraphNode* node,bool preTransform){
#ifndef XSICONVERT

	FloatVector3d pos,rot,scale;

	if(node->object.type==OBJECT){

		if(node->object.object->envelopes.size()>0){
			pos=node->object.object->basePos;
			rot=node->object.object->baseRot;
			scale=node->object.object->baseScale;
		}else{
			pos=node->object.object->pos;
			rot=node->object.object->rot;
			scale=node->object.object->scale;
		}

	}else if(node->object.type==NULL3D){
		pos=node->object.null->pos;
		rot=node->object.null->rot;
		scale=node->object.null->scale;
	}else if(node->object.type==IK_ROOT){
		pos=node->object.ikRoot->pos;
		rot=node->object.ikRoot->rot;
		scale=node->object.ikRoot->scale;
	}else if(node->object.type==IK_JOINT){
		pos=node->object.ikJoint->pos;
		rot=node->object.ikJoint->rot;
		scale=node->object.ikJoint->scale;
	}else if(node->object.type==IK_EFFECTOR){
		pos=FloatVector3d();
		rot=FloatVector3d();
		scale=FloatVector3d(1,1,1);
	}else if(node->object.type==LIGHT){
		pos=node->object.light->pos;
		rot=node->object.light->rot;
		scale=node->object.light->scale;
	}else{

	}

	if(node->draw && !preTransform){

		if(node->object.type==OBJECT){
			if(node->object.object->envelopeVerticesCount>1){
			
				for(int i=0; i<node->object.object->envelopeVerticesCount; i++){
					node->object.object->drawCurrentEnvelope=i;
					callbackFunction(node->object);
				}
			}else{
				callbackFunction(node->object);
			}
		}else{
			callbackFunction(node->object);
		}
	}

#ifndef SOFTWARE_TRANSFORMS

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
	glRotatef(rot.z,0.0f,0.0f,1.0f);
	glRotatef(rot.y,0.0f,1.0f,0.0f);
	glRotatef(rot.x,1.0f,0.0f,0.0f);
	glScalef(scale.x,scale.y,scale.z);

	if(node->object.type==IK_EFFECTOR){
		IkRoot* r=node->object.ikEffector->root;

		if(r->sceneGraphNode!=node->parent){
			//we only really care where the root is
			//so if the root isn't our parent, we need to trace to the root
			//argh


			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glLoadIdentity();
			level->camera->transform();
			sceneGraph.modelviewTransform(&sceneGraph.root,r->sceneGraphNode);


		}

		for(int i=0; i<r->joints.size(); i++){

			IkJoint* j=r->joints[i];

			glRotatef(j->rot.z,0.0f,0.0f,1.0f);
			glRotatef(j->rot.y,0.0f,1.0f,0.0f);
			glRotatef(j->rot.x,1.0f,0.0f,0.0f);
			glScalef(j->scale.x,j->scale.y,j->scale.z);

			glTranslatef(j->length,0,0);
			

		}
	}

#endif

	if(node->draw && preTransform){

		if(node->object.type==OBJECT){
			if(node->object.object->envelopeVerticesCount>1){
			
				for(int i=0; i<node->object.object->envelopeVerticesCount; i++){
					node->object.object->drawCurrentEnvelope=i;
					callbackFunction(node->object);
				}
			}else{
				callbackFunction(node->object);
			}
		}else{
			callbackFunction(node->object);
		}
	}

	Array<SortItem> sortids;

	FloatVector3d p;
	
	for(int i=0; i<node->children.size(); i++){
		if(node->children[i].object.type==OBJECT){
			p=node->children[i].object.object->box.center()-level->camera->pos;
			sortids.pushBack(SortItem(i,math.dotProduct(p,p)));
		}else{
			sortids.pushBack(SortItem(i,0));
		}
	}

	Array<int> res=sort.sort(sortids,true);

	for(int i=0; i<res.size(); i++){
		drawRecurse(callbackFunction,&node->children[res[i]],preTransform);
	}
	
	if(node->object.type==IK_EFFECTOR){
		IkRoot* r=node->object.ikEffector->root;

		if(r->sceneGraphNode!=node->parent){
			glPopMatrix();
		}
	}

#ifndef SOFTWARE_TRANSFORMS
	glPopMatrix(); 
#endif
#endif
}

void SceneGraph::draw(void (*callbackFunction)(ObjectAddress),bool preTransform){
#ifndef XSICONVERT
	for(int i=0; i<root.children.size(); i++){

		drawRecurse(callbackFunction,&root.children[i],preTransform);
	}
#endif
}

FloatVector3d SceneGraph::global(ObjectAddress node,int t,FloatVector3d gpos){

	SceneGraphNode* p;

	if(node.type==OBJECT){
		p=node.object->sceneGraphNode;
	}else if(node.type==IK_JOINT){
		p=node.ikJoint->sceneGraphNode;
	}else if(node.type==IK_EFFECTOR){
		p=node.ikEffector->sceneGraphNode;
	}else if(node.type==IK_ROOT){
		p=node.ikRoot->sceneGraphNode;
	}else if(node.type==NULL3D){
		p=node.null->sceneGraphNode;
	}else if(node.type==CURVE){
		p=node.curve->sceneGraphNode;
	}else{

	}

	FloatVector3d pos,rot,scale;

	Matrix4d4d m;
	m.loadIdentity();

	Array<SceneGraphNode*> path;

	while(p!=&root){
	
		path.pushBack(p);

		p=p->parent;
	}

	for(int i=path.size()-1; i>=0; i--){
		p=path[i];

		if(p->object.type==NULL3D){
			if( (t==4 || t==5 || t==6) && p->object.null->hasBasePose){
				pos=p->object.null->basePos;
				rot=p->object.null->baseRot;
				scale=p->object.null->baseScale;
			}else{
				pos=p->object.null->pos;
				rot=p->object.null->rot;
				scale=p->object.null->scale;
			}
		}else if(p->object.type==CURVE){
			if( (t==4 || t==5 || t==6) && p->object.curve->hasBasePose){
				pos=p->object.curve->basePos;
				rot=p->object.curve->baseRot;
				scale=p->object.curve->baseScale;
			}else{
				pos=p->object.curve->pos;
				rot=p->object.curve->rot;
				scale=p->object.curve->scale;
			}
		}else if(p->object.type==OBJECT){
			if(p->object.object->envelopes.size()>0){
				pos=p->object.object->basePos;
				rot=p->object.object->baseRot;
				scale=p->object.object->baseScale;
			}else{
				pos=p->object.object->pos;
				rot=p->object.object->rot;
				scale=p->object.object->scale;
			}
		}else if(p->object.type==IK_ROOT){
			if( (t==4 || t==5 || t==6) && p->object.ikRoot->hasBasePose){
				pos=p->object.ikRoot->basePos;
				rot=p->object.ikRoot->baseRot;
				scale=p->object.ikRoot->baseScale;
			}else{
				pos=p->object.ikRoot->pos;
				rot=p->object.ikRoot->rot;
				scale=p->object.ikRoot->scale;
			}
		}else if(p->object.type==CAMERA){
			pos=p->object.camera->pos;
			rot=p->object.camera->rot;
			scale=p->object.camera->scale;
		}else if(p->object.type==IK_EFFECTOR){
			pos=FloatVector3d();
			rot=FloatVector3d();
			scale=FloatVector3d(1,1,1);
		}else{
		
		}


		m.translate(pos.x,pos.y,pos.z);
		m.rotateZ(rot.z);
		m.rotateY(rot.y);
		m.rotateX(rot.x);

		if(p->object.type==IK_EFFECTOR){
			IkRoot* r=p->object.ikEffector->root;


			for(int i=0; i<r->joints.size(); i++){

				IkJoint* j=r->joints[i];

				if(t==4 || t==5 || t==6){
					m.rotateZ(j->baseRot.z);
					m.rotateY(j->baseRot.y);
					m.rotateX(j->baseRot.x);
					
					m.translate(j->length,0,0);
				}else{
					m.rotateZ(j->rot.z);
					m.rotateY(j->rot.y);
					m.rotateX(j->rot.x);
					
					m.translate(j->length,0,0);
				}
			}
		}
	}

	if(t==1){
		return FloatVector3d(m.entries[12],m.entries[13],m.entries[14]);
	}else if(t==2){
		return m.getRotation();
	}else if(t==4){
		return FloatVector3d(m.entries[12],m.entries[13],m.entries[14]);
	}else if(t==5){
		return m.getRotation();
	}else if(t==7){
		m.translate(gpos.x,gpos.y,gpos.z);
		return FloatVector3d(m.entries[12],m.entries[13],m.entries[14]);
	}

	return FloatVector3d();
}

FloatVector3d SceneGraph::local(ObjectAddress node,FloatVector3d v,int t){

	SceneGraphNode* p;

	if(node.type==OBJECT){
		p=node.object->sceneGraphNode;
	}else if(node.type==IK_JOINT){
		p=node.ikJoint->sceneGraphNode;
	}else if(node.type==IK_EFFECTOR){
		p=node.ikEffector->sceneGraphNode;
	}else if(node.type==IK_ROOT){
		p=node.ikRoot->sceneGraphNode;
	}else if(node.type==NULL3D){
		p=node.null->sceneGraphNode;
	}else if(node.type==CURVE){
		p=node.curve->sceneGraphNode;
	}else{

	}


	FloatVector3d pos,rot,scale;

	Matrix4d4d m;
	m.loadIdentity();

	Array<SceneGraphNode*> path;

	while(p!=&root){
	
		path.pushBack(p);

		p=p->parent;
	}

	for(int i=1; i<path.size(); i++){
		p=path[i];

		if(p->object.type==NULL3D){
			pos=p->object.null->pos;
			rot=p->object.null->rot;
			scale=p->object.null->scale;
		}else if(p->object.type==OBJECT){
			if(p->object.object->envelopes.size()>0){
				pos=p->object.object->basePos;
				rot=p->object.object->baseRot;
				scale=p->object.object->baseScale;
			}else{
				pos=p->object.object->pos;
				rot=p->object.object->rot;
				scale=p->object.object->scale;
			}
		}else if(p->object.type==IK_ROOT){
			pos=p->object.ikRoot->pos;
			rot=p->object.ikRoot->rot;
			scale=p->object.ikRoot->scale;
		}else if(p->object.type==CAMERA){
			pos=p->object.camera->pos;
			rot=p->object.camera->rot;
			scale=p->object.camera->scale;
		}else if(p->object.type==IK_EFFECTOR){
			pos=FloatVector3d();
			rot=FloatVector3d();
			scale=FloatVector3d(1,1,1);
		}else if(p->object.type==IK_JOINT){
			pos=p->object.ikJoint->pos;
			rot=p->object.ikJoint->rot;
			scale=p->object.ikJoint->scale;
		}else if(p->object.type==CURVE){
			pos=p->object.curve->pos;
			rot=p->object.curve->rot;
			scale=p->object.curve->scale;
		}else{
		
		}
		
		
		m.rotateX(-rot.x);
		m.rotateY(-rot.y);
		m.rotateZ(-rot.z);

		m.translate(-pos.x,-pos.y,-pos.z);

		if(p->object.type==IK_EFFECTOR){
			IkRoot* r=p->object.ikEffector->root;

			for(int i=r->joints.size()-1; i>=0; i--){

				IkJoint* j=r->joints[i];

				
				m.translate(-j->length,0,0);
				m.rotateX(-j->rot.x);
				m.rotateY(-j->rot.y);
				m.rotateZ(-j->rot.z);

			}

		}


	}


	if(t==1){
		m.translate(v.x,v.y,v.z);

		return FloatVector3d(m.entries[12],m.entries[13],m.entries[14]);
	}else if(t==2){
		m.rotateZ(v.z);
		m.rotateY(v.y);
		m.rotateX(v.x);

		return m.getRotation();
	}

	return FloatVector3d();
}


FloatVector3d SceneGraph::globalBaseRot(ObjectAddress node){

	return global(node,5,FloatVector3d());
}

FloatVector3d SceneGraph::globalBasePos(ObjectAddress node){
	return global(node,4,FloatVector3d());
}


FloatVector3d SceneGraph::globalBaseScale(ObjectAddress node){
	return global(node,6,FloatVector3d());
}


FloatVector3d SceneGraph::globalRot(ObjectAddress node){

	return global(node,2,FloatVector3d());
}

FloatVector3d SceneGraph::globalPos(ObjectAddress node){
	return global(node,1,FloatVector3d());
}

FloatVector3d SceneGraph::globalPos(ObjectAddress node,FloatVector3d pos){
	return global(node,7,pos);
}


FloatVector3d SceneGraph::globalScale(ObjectAddress node){
	return global(node,3,FloatVector3d());
}


FloatVector3d SceneGraph::localRot(ObjectAddress node,FloatVector3d v){

	return local(node,v,2);
}

FloatVector3d SceneGraph::localPos(ObjectAddress node,FloatVector3d v){
	return local(node,v,1);
}


FloatVector3d SceneGraph::localScale(ObjectAddress node,FloatVector3d v){
	return local(node,v,3);
}

void SceneGraph::clearDrawList(SceneGraphNode* node){

	if(node==NULL){
		node=&root;
	}

	node->draw=false;

	for(int i=0; i<node->children.size(); i++){
		clearDrawList(&node->children[i]);
	}
}

void SceneGraph::enableDrawList(SceneGraphNode* node){

	if(node==NULL){
		node=&root;
	}

	node->draw=true;

	for(int i=0; i<node->children.size(); i++){
		enableDrawList(&node->children[i]);
	}
}


void SceneGraph::modelviewTransform(SceneGraphNode* from, SceneGraphNode* to){
#ifndef XSICONVERT

	Array<SceneGraphNode*> path;

	SceneGraphNode* cnode=to;

	bool forceout=false;

	while(cnode!=from && !forceout){
		path.pushBack(cnode);

		if(cnode->parent!=NULL){
			cnode=cnode->parent;
		}else{
			forceout=true;
		}
	}

	
	path.reverse();

	for(int i=0; i<path.size(); i++){
		SceneGraphNode* node=path[i];
		FloatVector3d pos,rot,scale;

		if(node->object.type==OBJECT){

			if(node->object.object->envelopes.size()>0){
				pos=node->object.object->basePos;
				rot=node->object.object->baseRot;
				scale=node->object.object->baseScale;
			}else{
				pos=node->object.object->pos;
				rot=node->object.object->rot;
				scale=node->object.object->scale;
			}

		}else if(node->object.type==NULL3D){
			pos=node->object.null->pos;
			rot=node->object.null->rot;
			scale=node->object.null->scale;
		}else if(node->object.type==IK_ROOT){
			pos=node->object.ikRoot->pos;
			rot=node->object.ikRoot->rot;
			scale=node->object.ikRoot->scale;
		}else if(node->object.type==IK_JOINT){
			pos=node->object.ikJoint->pos;
			rot=node->object.ikJoint->rot;
			scale=node->object.ikJoint->scale;
		}else if(node->object.type==IK_EFFECTOR){
			pos=FloatVector3d();
			rot=FloatVector3d();
			scale=FloatVector3d(1,1,1);
		}else if(node->object.type==LIGHT){
			pos=node->object.light->pos;
			rot=node->object.light->rot;
			scale=node->object.light->scale;
		}else{

		}


		glTranslatef(pos.x,pos.y,pos.z);
		glRotatef(rot.z,0,0,1);
		glRotatef(rot.y,0,1,0);
		glRotatef(rot.x,1,0,0);
		glScalef(scale.x,scale.y,scale.z);
	

		if(node->object.type==IK_EFFECTOR){
			IkRoot* r=node->object.ikEffector->root;

			//if the effector's parent isn't the root
			//we need to recurse here.  see the main draw function above.
			// need to implement this sometime probably


			for(int i=0; i<r->joints.size(); i++){
				IkJoint* j=r->joints[i];

				glRotatef(j->rot.z,0,0,1);
				glRotatef(j->rot.y,0,1,0);
				glRotatef(j->rot.x,1,0,0);
				glScalef(j->scale.x,j->scale.y,j->scale.z);
				glTranslatef(j->length,0,0);
				
			}

		}

	}

#endif
}


Matrix4d4d SceneGraph::getMatrixMain(SceneGraphNode* from, SceneGraphNode* to, bool reverse){
	Array<SceneGraphNode*> path;

	SceneGraphNode* cnode=to;

	while(cnode!=from){
		path.pushBack(cnode);
		cnode=cnode->parent;
	}

	path.reverse();

	Matrix4d4d final;

	final.loadIdentity();

	for(int i=0; i<path.size(); i++){
		SceneGraphNode* node=path[i];
		FloatVector3d pos,rot,scale;

		if(node->object.type==OBJECT){

			if(node->object.object->envelopes.size()>0){
				pos=node->object.object->basePos;
				rot=node->object.object->baseRot;
				scale=node->object.object->baseScale;
			}else{
				pos=node->object.object->pos;
				rot=node->object.object->rot;
				scale=node->object.object->scale;
			}

		}else if(node->object.type==NULL3D){
			pos=node->object.null->pos;
			rot=node->object.null->rot;
			scale=node->object.null->scale;
		}else if(node->object.type==IK_ROOT){
			pos=node->object.ikRoot->pos;
			rot=node->object.ikRoot->rot;
			scale=node->object.ikRoot->scale;
		}else if(node->object.type==IK_JOINT){
			pos=node->object.ikJoint->pos;
			rot=node->object.ikJoint->rot;
			scale=node->object.ikJoint->scale;
		}else if(node->object.type==IK_EFFECTOR){
			pos=FloatVector3d();
			rot=FloatVector3d();
			scale=FloatVector3d(1,1,1);
		}else if(node->object.type==LIGHT){
			pos=node->object.light->pos;
			rot=node->object.light->rot;
			scale=node->object.light->scale;
		}else{

		}


		if(reverse){
			final.rotateX(-rot.x);
			final.rotateY(-rot.y);
			final.rotateZ(-rot.z);
			final.translate(-pos.x,-pos.y,-pos.z);
		}else{
			final.translate(pos.x,pos.y,pos.z);
			final.rotateZ(rot.z);
			final.rotateY(rot.y);
			final.rotateX(rot.x);
			final.scale(scale.x,scale.y,scale.z);
		}

		if(node->object.type==IK_EFFECTOR){
			IkRoot* r=node->object.ikEffector->root;

			for(int i=0; i<r->joints.size(); i++){
				IkJoint* j=r->joints[i];

				if(reverse){
					final.translate(-j->length,0,0);
					final.rotateX(-j->rot.x);
					final.rotateY(-j->rot.y);
					final.rotateZ(-j->rot.z);
				}else{
					final.rotateZ(j->rot.z);
					final.rotateY(j->rot.y);
					final.rotateX(j->rot.x);
					final.scale(j->scale.x,j->scale.y,j->scale.z);
					final.translate(j->length,0,0);
				}
			}

		}

	}

	return final;
}

Matrix4d4d SceneGraph::getMatrix(SceneGraphNode* from, SceneGraphNode* to){
	return getMatrixMain(from,to,false);
}

Matrix4d4d SceneGraph::getMatrixReverse(SceneGraphNode* from, SceneGraphNode* to){
	return getMatrixMain(from,to,true);
}


bool SceneGraph::nonstatic(SceneGraphNode* node){

	bool nonstatic=false;


	while(node!=&root){
		if(node->object.type==NULL3D){
			nonstatic=true;
		}else if(node->object.type==IK_ROOT){
			nonstatic=true;
		}else if(node->object.type==IK_JOINT){
			nonstatic=true;
		}else if(node->object.type==IK_EFFECTOR){
			nonstatic=true;
		}else if(node->object.object->flags & OBJECT_STATIC){
			nonstatic=true;
		}

		node=node->parent;
	}

	return nonstatic;
}


static int luaSceneGraphGlobalPos(lua_State *L){

	return 0;
}


static int luaSceneGraphGlobalRot(lua_State *L){

	return 0;
}

static int luaSceneGraphGlobalScale(lua_State *L){

	return 0;
}

bool SceneGraph::scriptInit(){
#ifndef XSICONVERT
	LuaType* luaa=script().add("sceneGraph");
		script().add("globalPos",luaSceneGraphGlobalPos,luaa);
		script().add("globalRot",luaSceneGraphGlobalRot,luaa);
		script().add("globalScale",luaSceneGraphGlobalScale,luaa);
#endif

	return true;
}

bool SceneGraph::remove(SceneGraphNode* node){
	if(node->children.size()>0){
		for(int i=0; i<node->children.size(); i++){
			node->parent->children.pushBack(node->children[i]);
			node->parent->children[node->parent->children.size()-1].parent=node->parent;
			*(node->children[i].sceneGraphNode())=&node->parent->children[node->parent->children.size()-1];
		}
	}

	for(int i=0; i<node->parent->children.size(); i++){
		if(&node->parent->children[i]==node){
			node->parent->children.erase(i);
			break;
		}
	}

	return true;
}

SceneGraphNode** SceneGraphNode::sceneGraphNode(){

	if(object.type==OBJECT){
		return &object.object->sceneGraphNode;
	}else if(object.type==NULL3D){
		return &object.null->sceneGraphNode;
	}else{

	}

	return NULL;
}

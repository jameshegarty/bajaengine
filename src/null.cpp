#include "null.h"
#include "GlHeader.hpp"
#include "level.hpp"

void NullObject::draw(){
	if(visible){
#ifndef SOFTWARE_TRANSFORMS
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
			
	
	glRotatef(rot.z,0.0f,0.0f,1.0f);
	glRotatef(rot.y,0.0f,1.0f,0.0f);
	glRotatef(rot.x,1.0f,0.0f,0.0f);

	glScalef(scale.x,scale.y,scale.z);



#endif

	glBegin(GL_LINES);
		glVertex3f(0,1,0);
		glVertex3f(0,-1,0);

		glVertex3f(1,0,0);
		glVertex3f(-1,0,0);

		glVertex3f(0,0,1);
		glVertex3f(0,0,-1);
	glEnd();

#ifndef SOFTWARE_TRANSFORMS
	glPopMatrix();
#endif
	}
}


void NullObject::drawRaw(){

	if(visible){
	glBegin(GL_LINES);
		glVertex3f(0,1,0);
		glVertex3f(0,-1,0);

		glVertex3f(1,0,0);
		glVertex3f(-1,0,0);

		glVertex3f(0,0,1);
		glVertex3f(0,0,-1);
	glEnd();

	}
}

void NullObject::update(){
	if(constraintPos!=NULL){

		if(constraintPos->type==NULL3D){
			pos=constraintPos->null->pos;
		}else if(constraintPos->type==IK_ROOT){
			pos=constraintPos->ikRoot->pos;
		}else if(constraintPos->type==IK_JOINT){
			pos=constraintPos->ikJoint->pos;
		}else{

		}
	}

	if(constraintRot!=NULL){
		if(constraintRot->type==NULL3D){
			rot=constraintRot->null->rot;
		}else if(constraintRot->type==IK_ROOT){
			rot=constraintRot->ikRoot->rot;
		}else if(constraintRot->type==IK_JOINT){
			rot=constraintRot->ikJoint->rot;
		}else{

		}
	}	

	if(constraintScale!=NULL){
		if(constraintScale->type==NULL3D){
			scale=constraintScale->null->scale;
		}else if(constraintScale->type==IK_ROOT){
			scale=constraintScale->ikRoot->scale;
		}else if(constraintScale->type==IK_JOINT){
			scale=constraintScale->ikJoint->scale;
		}else{

		}
	}
}

NullObject::NullObject(){
	constraintPos=NULL;
	constraintRot=NULL;
	constraintScale=NULL;
	hasBasePose=false;
	scale.x=1;
	scale.y=1;
	scale.z=1;
}

Object NullObject::getObj(){
	Object obj;
	
	obj.type=3;
	obj.pos=pos;
	obj.rot=rot;
	obj.scale=scale;
	obj.sceneGraphNode=sceneGraphNode;
	obj.sceneGraphLevel=sceneGraphLevel;
	obj.name=name;

	return obj;
}

void NullObject::write(std::ofstream& out){
	Object obj=getObj();
	obj.write(out);
}


unsigned long NullObject::size(){
	Object obj=getObj();
	return obj.size();
}


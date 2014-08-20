#include "ikchain.hpp"

#ifdef _WIN32
#include <gl/gl.h>
#include "gl/extensions.h"
#elif LINUX
	#include <gl.h>
#else
	#include <GLUT/glut.h>
#endif

#include "HelperLibMath.hpp"
#include "level.hpp"
#include "SceneGraph.hpp"

void IkJoint::draw(){

	if(!visible){return;}

			float px=length/float(8);
			float len=length;

			glBegin(GL_LINES);
	
			//the diamond
			glVertex3f(px,float(1)/float(4),0);
			glVertex3f(px,0,float(1)/float(4));

			glVertex3f(px,float(1)/float(4),0);
			glVertex3f(px,0,-float(1)/float(4));

			glVertex3f(px,-float(1)/float(4),0);
			glVertex3f(px,0,float(1)/float(4));

			glVertex3f(px,-float(1)/float(4),0);
			glVertex3f(px,0,-float(1)/float(4));

			glVertex3f(px,0,float(1)/float(4));
			glVertex3f(px,0,-float(1)/float(4));

			glVertex3f(px,float(1)/float(4),0);
			glVertex3f(px,-float(1)/float(4),0);

			//
			glVertex3f(0,0,0);
			glVertex3f(px,0,float(1)/float(4));

			glVertex3f(0,0,0);
			glVertex3f(px,0,-float(1)/float(4));

			glVertex3f(0,0,0);
			glVertex3f(px,-float(1)/float(4),0);

			glVertex3f(0,0,0);
			glVertex3f(px,float(1)/float(4),0);

			//

			glVertex3f(len,0,0);
			glVertex3f(px,0,float(1)/float(4));

			glVertex3f(len,0,0);
			glVertex3f(px,0,-float(1)/float(4));

			glVertex3f(len,0,0);
			glVertex3f(px,-float(1)/float(4),0);

			glVertex3f(len,0,0);
			glVertex3f(px,float(1)/float(4),0);
			glEnd();
}

void IkRoot::update(){

}

void IkRoot::draw(){

	if(!visible){return;}

	glBegin(GL_LINES);
		glVertex3f(0,1,0);
		glVertex3f(0,-1,0);

		glVertex3f(1,0,0);
		glVertex3f(-1,0,0);

		glVertex3f(0,0,1);
		glVertex3f(0,0,-1);
	glEnd();


	FloatVector3d p;

	FloatVector3d follow;

	glColor4f(0,0,float(0.4),1);

	
	Matrix4d4d m;
	m.loadIdentity();

	for(int i=0; i<joints.size(); i++){
		m.rotateZ(joints[i]->rot.z);
		m.rotateY(joints[i]->rot.y);
		m.rotateX(joints[i]->rot.x);
		
		FloatVector4d lol=m*FloatVector4d(1,0,0,0);

		p.x+=lol.x*joints[i]->length;
		p.y+=lol.y*joints[i]->length;
		p.z+=lol.z*joints[i]->length;

		glBegin(GL_LINES);
			glVertex3f(p.x,p.y+1,p.z);
			glVertex3f(p.x,p.y-1,p.z);

			glVertex3f(p.x+1,p.y,p.z);
			glVertex3f(p.x-1,p.y,p.z);

			glVertex3f(p.x,p.y,p.z+1);
			glVertex3f(p.x,p.y,p.z-1);
		glEnd();

	}

	glColor4f(0,float(0.4),0,1);

}

IkRoot::IkRoot(){
	constraintPos=NULL;
	constraintRot=NULL;
	constraintScale=NULL;
}

void IkEffector::draw(){
	if(!visible){return;}

	glColor4f(float(0.4),0,0,1);
	glBegin(GL_LINES);
		glVertex3f(0,1,0);
		glVertex3f(0,-1,0);

		glVertex3f(1,0,0);
		glVertex3f(-1,0,0);

		glVertex3f(0,0,1);
		glVertex3f(0,0,-1);
	glEnd();

	glColor4f(0,float(0.4),0,1);
}

FloatVector3d IkEffector::ikFsolve(){
	FloatVector3d pos;

	/*for(int i=0; i<root->joints.size(); i++){
		pos.x=pos.x+root->joints[i]->length*math.cos(v.rot)
		pos.y=pos.y+root->joints[i]->length*math.sin(v.rot)
	}*/

	return pos;
}

FloatVector2d IkEffector::ikFsolve2d(){
	FloatVector2d pos;

	float lastrot=0;

	for(int i=0; i<root->joints.size(); i++){
		pos.x=pos.x+root->joints[i]->length*math.cos(root->joints[i]->rot.z+lastrot);
		pos.y=pos.y+root->joints[i]->length*math.sin(root->joints[i]->rot.z+lastrot);
		lastrot+=root->joints[i]->rot.z;
	}

	return pos;
}

float IkEffector::ikFsolveFirstOrder(int k){
	FloatVector2d currentpos=ikFsolve2d();

	float fin=2*(currentpos.x-pos.x)*(-root->joints[k]->length*math.sin(root->joints[k]->rot.z)) + 2*(currentpos.y-pos.y)*(root->joints[k]->length*math.cos(root->joints[k]->rot.z));

	return fin;
}

void IkEffector::solveOnce(){
	//J={}
	Array<float> J;

	float dt=0;

	//for k,v in pairs(chain) do
	for(int i=0; i<root->joints.size(); i++){
		J.pushBack();
		J[i]=ikFsolveFirstOrder(i);

		dt=dt+J[i]*J[i];
	}

	dt=math.sqrt(dt);

	if(dt==0){return;}

    for(int k=0; k<root->joints.size(); k++){
       	J[k]=J[k]/dt;
		J[k]=J[k]*0.005;
		J[k]=-J[k];


		root->joints[k]->rot.z=root->joints[k]->rot.z+J[k];
		//chain[k].rot=chain[k].rot+J[k]
	}

}

void IkEffector::solve(){

	int maxIter=100;
	float err=31;

	while(err>1 && maxIter>0){
		FloatVector2d newpos=ikFsolve2d();
		float a=newpos.x-pos.x;
		float b=newpos.y-pos.y;
		float dist=a*a + b*b;
		err=dist;

		solveOnce();
		pos=ikFsolve2d();
		a=newpos.x-pos.x;
		b=newpos.y-pos.y;
		dist=a*a + b*b;
		err=dist;

		maxIter=maxIter-1;
	}
}
	
void IkEffector::fkSolve(){
	FloatVector3d p;

	/*Matrix4d4d mp;
	mp.loadIdentity();

	name;

	for(int i=0; i<root->joints.size(); i++){

		mp.rotateZ(root->joints[i]->rot.z);
		mp.rotateY(root->joints[i]->rot.y);
		mp.rotateX(root->joints[i]->rot.x);

		FloatVector4d lol=mp*FloatVector4d(1,0,0,0);

		p.x+=lol.x*root->joints[i]->length;
		p.y+=lol.y*root->joints[i]->length;
		p.z+=lol.z*root->joints[i]->length;
	}*/

	//pos=p;
	//pos=p;
}


void IkEffector::update(){

	//solve();
}

IkJoint::IkJoint(){
	constraintPos=NULL;
	constraintRot=NULL;
	constraintScale=NULL;
}

IkEffector::IkEffector(){
	constraintPos=NULL;
	constraintRot=NULL;
	constraintScale=NULL;
}

void IkJoint::write(std::ofstream& out){

}

void IkRoot::write(std::ofstream& out){

}

void IkEffector::write(std::ofstream& out){

}

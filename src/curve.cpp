#include "curve.hpp"

#ifdef _WIN32
#include <gl/gl.h>
#include "gl/extensions.h"
#elif LINUX
#include <gl.h>
#else
	#include <GLUT/glut.h>
#endif

#include "level.hpp"
#include "SceneGraph.hpp"

void Curve::draw(){

	if(degree==1){
		
		#ifndef SOFTWARE_TRANSFORMS
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glTranslatef(pos.x,pos.y,pos.z);
					
			
			glRotatef(rot.z,0.0f,0.0f,1.0f);
			glRotatef(rot.y,0.0f,1.0f,0.0f);
			glRotatef(rot.x,1.0f,0.0f,0.0f);

			glScalef(scale.x,scale.y,scale.z);



		#endif

		glColor4f(0,float(0.4),0,1);

		glBegin(GL_LINES);

		for(int i=1; i<controlPoints.size(); i++){
			glVertex3f(controlPoints[i-1].x,controlPoints[i-1].y,controlPoints[i-1].z);
			glVertex3f(controlPoints[i].x,controlPoints[i].y,controlPoints[i].z);
		}

		if(closed){
			glVertex3f(controlPoints[controlPoints.size()-1].x,controlPoints[controlPoints.size()-1].y,controlPoints[controlPoints.size()-1].z);
			glVertex3f(controlPoints[0].x,controlPoints[0].y,controlPoints[0].z);
		}

		glEnd();

		#ifndef SOFTWARE_TRANSFORMS
			glPopMatrix();
		#endif
		
	}else if(degree==3){

	}

}

void Curve::drawRaw(){

	if(degree==1){
		glColor4f(0,0,float(0.4),1);

		glBegin(GL_LINES);

		for(int i=1; i<controlPoints.size(); i++){
			glVertex3f(controlPoints[i-1].x,controlPoints[i-1].y,controlPoints[i-1].z);
			glVertex3f(controlPoints[i].x,controlPoints[i].y,controlPoints[i].z);
		}

		glEnd();

		glColor4f(1,0,0,1);

		glBegin(GL_LINES);
		glVertex3f(0,100,0);
		glVertex3f(0,-100,0);

		glVertex3f(100,0,0);
		glVertex3f(-100,0,0);

		glVertex3f(0,0,100);
		glVertex3f(0,0,-100);
		glEnd();
	}

}

FloatVector3d Curve::getPos(float t){


	FloatVector3d final;

	if(degree==1){
		float totlen=0;
		float* seclen=new float[controlPoints.size()];

		for(int i=1; i<controlPoints.size(); i++){
			FloatVector3d tt=controlPoints[i]-controlPoints[i-1];
			totlen+=tt.magnitude();
			seclen[i-1]=totlen;
		}

		if(closed){
			FloatVector3d tt=controlPoints[controlPoints.size()-1]-controlPoints[0];
			totlen+=tt.magnitude();
			seclen[controlPoints.size()-1]=totlen;
		}

		while(t>1){t-=1;}
		if(t<0){t=0;}

		float ourpos=t*totlen;

		int cnt=controlPoints.size()-1;

		if(closed){cnt=controlPoints.size();}

		for(int i=0; i<cnt; i++){
			if(seclen[i]>ourpos){

				if(i==0){
					float scale=ourpos/seclen[i];
					final=controlPoints[0]+(controlPoints[1]-controlPoints[0])*scale;
				}else if(i==cnt-1 && closed){
					float scale=(ourpos-seclen[i-1])/(seclen[i]-seclen[i-1]);
					final=controlPoints[controlPoints.size()-1]+(controlPoints[0]-controlPoints[controlPoints.size()-1])*scale;
				}else{
					float scale=(ourpos-seclen[i-1])/(seclen[i]-seclen[i-1]);
					final=controlPoints[i]+(controlPoints[i+1]-controlPoints[i])*scale;
				}

				break;
			}
		}

		delete[] seclen;

	}else if(degree==3){

	}

	final=level->sceneGraph.globalPos(sceneGraphNode->object,final);

	return final;
}

Curve::Curve(){
	closed=false;
	degree=1;
	hasBasePose=false;
}


int luaGetCurvePos(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("Curve error: bad arguments, should be 2 arguments");
		return 0;
	}

	String object;

	float sx;

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				object=args[0].children[i].value.ss;
			}
		}

	}else{
		console().write("Curve error: bad arguments, should be accessed as a class");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){
		sx=args[1].value.ds;
	}else{
		console().write("Curve error: bad arguments");
		return 0;
	}


	if(level->curveKey.find(object)){
		Curve* o=level->curveKey[object];

		FloatVector3d p=o->getPos(sx);

		script().resultTable(L);
		script().result(L,"x",p.x);
		script().result(L,"y",p.y);
		script().result(L,"z",p.z);

		return 1;

	}else{
		console().write("Curve error: the curve '"+object+"' doesn't exist.");
		return 0;
	}	

	return 0;
}

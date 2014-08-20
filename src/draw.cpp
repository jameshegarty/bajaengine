#include "GlHeader.hpp"
										
#include "draw.hpp"
#include "DrawDynamic.h"
#include "level.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mouse.h"
#include "picking.h"
#include "hardware.hpp"
#include "random.h"

#include "Helperlib/Log.hpp"
#include "Helperlib/sort.hpp"
#include "Helperlib/HelperLibMath.hpp"

#include "lightmap.h"
#include "editor.h"
#include "timeing.hpp"

#ifdef _WIN32
	#include <math.h>
#endif

#include "picker_matrix.h"


GLint maxLights;
ShaderId displaceFragmentShader;
ShaderId displaceVertexShader;
ShaderId specularFragmentShader;
ShaderId specularVertexShader;
ShaderId displaceSpecularFragmentShader;
ShaderId displaceSpecularVertexShader;
ShaderId displaceSpecularBumpFragmentShader;
ShaderId displaceSpecularBumpVertexShader;
ShaderId envelopeVertexShader;

ShaderId mixer_1_v;
ShaderId mixer_1_f;

ShaderId mixer_2_v;
ShaderId mixer_2_f;

ShaderId mixer_3_f;
ShaderId mixer_3_v;

bool lightState=false;



bool drawInit(){

	if(!hardware.fragmentProgram){
		return true;
	}

	displaceFragmentShader=shader.addFragmentFile("shaders/displace.fp","a9aa8b78ca9a37900ed4196b955fd464");
	#ifdef SOFTWARE_TRANSFORMS
		displaceVertexShader=shader.addVertexFile("shaders/displace.vp");
	#else
		displaceVertexShader=shader.addVertexFile("shaders/displace_hardware.vp","4d96faadf67f5c2b1f9193b7dd7b853f");
	#endif

	specularFragmentShader=shader.addFragmentFile("shaders/spec.fp");
	
	#ifdef SOFTWARE_TRANSFORMS
		specularVertexShader=shader.addVertexFile("shaders/spec.vp");
	#else
		specularVertexShader=shader.addVertexFile("shaders/spec_hardware.vp");
	#endif


	displaceSpecularFragmentShader=shader.addFragmentFile("shaders/displace_spec.fp","5cfe103690aed5d05c0e030553938f77");

	#ifdef SOFTWARE_TRANSFORMS
		displaceSpecularVertexShader=shader.addVertexFile("shaders/displace_spec.vp");
	#else
		displaceSpecularVertexShader=shader.addVertexFile("shaders/displace_spec_hardware.vp");
	#endif

	displaceSpecularBumpFragmentShader=shader.addFragmentFile("shaders/displace_spec_bump.fp","1e4932cfb7e3deb4c892c669ee41d017");

	#ifdef SOFTWARE_TRANSFORMS
		displaceSpecularBumpVertexShader=shader.addVertexFile("shaders/displace_spec_bump.vp");
	#else
		displaceSpecularBumpVertexShader=shader.addVertexFile("shaders/displace_spec_bump_hardware.vp","1cc645f2d4b096f34c248747fbfad957");
	#endif

	mixer_1_f=shader.addFragmentFile(hardware.compatibility.mixer1_fp);

	#ifdef SOFTWARE_TRANSFORMS
		mixer_1_v=shader.addVertexFile("shaders/mixer1.vp");
	#else
		mixer_1_v=shader.addVertexFile(hardware.compatibility.mixer1_hardware_vp);
	#endif

	mixer_2_f=shader.addFragmentFile(hardware.compatibility.mixer2_fp);
	
	#ifdef SOFTWARE_TRANSFORMS
		mixer_2_v=shader.addVertexFile("shaders/mixer2.vp");
	#else
		mixer_2_v=shader.addVertexFile(hardware.compatibility.mixer2_hardware_vp);
	#endif

	mixer_3_f=shader.addFragmentFile(hardware.compatibility.mixer3_fp);

	#ifdef SOFTWARE_TRANSFORMS
		mixer_3_v=shader.addVertexFile("shaders/mixer3.vp");
	#else
		mixer_3_v=shader.addVertexFile(hardware.compatibility.mixer3_hardware_vp);
	#endif

	envelopeVertexShader=shader.addVertexFile("shaders/envelope.vp","3f81fad1faa89d1808993ddac0f75e85");

	#ifdef PIPELINE_DYNAMIC
		drawInitDynamic();
	#endif
	return true;
}




bool listBuild=false;

void buildStaticList(){
	glDeleteLists(level->glListName,1);
	level->glListName=glGenLists(1);

	
	glNewList(level->glListName,GL_COMPILE);	
	drawArray(&level->staticObjects,true);

	glEndList();	
}


void drawStaticList(){
	if(level->objects.size()>0){
		glCallList(level->glListName);
	}
}


Level* lastLevel;

void drawLevel(){

	#ifdef PIPELINE_DYNAMIC
		if(conf->renderer=="dynamic"){
			drawLevelDynamic();
			return;
		}
	#endif

	glFrontFace( GL_CCW );

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);

	glEnable(GL_CULL_FACE);

	#ifndef SOFTWARE_TRANSFORMS
		glEnable(GL_NORMALIZE);
	#endif


	level->fog.update();

	if(conf->optimize=="none"){
		drawArray(&level->staticObjects,true);
		drawArray(&level->nonstaticObjects,true);
	}else if(conf->optimize=="list"){

		if(!listBuild  || level!=lastLevel){
			buildStaticList();
			listBuild=true;
			lastLevel=level;
		}

		drawStaticList();

		buildNonstaticList();
		drawNonstaticList();
	}else if(conf->optimize=="modern"){
		drawModern();
	}else if(conf->optimize=="vbo"){
		buildVBOs();
		drawVBOs();
	}

	for(int i=0; i<level->hair.size(); i++){
		if(level->hair[i]->visible){
			level->hair[i]->draw();
		}
	}

	for(int i=0; i<level->points.size(); i++){
		if(level->points[i]->visible){
			level->points[i]->draw();
		}
	}


	for(int i=0; i<level->lines.size(); i++){
		if(level->lines[i]->visible){
			level->lines[i]->draw();
		}
	}
	
#ifdef DEVELOPER
	for(int i=0; i<editorLines.size(); i++){
		if(editorLines[i]->visible){
			editorLines[i]->draw();
		}
	}
#endif

	for(int i=0; i<level->disks.size(); i++){
		if(level->disks[i]->visible){
			level->disks[i]->draw();
		}
	}


	for(int i=0; i<level->spheres.size(); i++){
		if(level->spheres[i]->visible){
			level->spheres[i]->draw();
		}
	}

	for(int i=0; i<level->boxes.size(); i++){
		if(level->boxes[i]->visible){
			level->boxes[i]->draw();
		}
	}

	for(int i=0; i<level->particles.size(); i++){
		if(level->particles[i]->visible){
			level->particles[i]->draw();
		}
	}

	if(conf->normals){
		drawNormals();
	}

	if(conf->drawbb){
		drawBB();
	}		

	if(conf->drawImplicit){
		drawImplicit();
	}

	if(level->water==NULL){
		drawTransparent(0,false,true);
	}else{
		drawTransparent(level->water->pos.y,false,false);
	}
}

void buildPickerNonstaticList(){

	Array<Object*> a;
	a.pushBack();

	for(int i=0; i<level->nonstaticObjects.size(); i++){

		if(!level->nonstaticObjects[i]->listBuiltQuick || level->nonstaticObjects[i]->dirty){

	
			glDeleteLists(level->nonstaticObjects[i]->glListNameQuick,1);

			level->nonstaticObjects[i]->glListNameQuick=glGenLists(1);
			glNewList(level->nonstaticObjects[i]->glListNameQuick,GL_COMPILE);

			level->nonstaticObjects[i]->draw();
			glEndList();	

			level->nonstaticObjects[i]->listBuiltQuick=true;
		}

	}
}


void buildPickerTransparentList(){

	Array<Object*> a;
	a.pushBack();

	for(int i=0; i<level->transparentObjects.size(); i++){
		if(!(level->transparentObjects[i]->flags&OBJECT_STATIC)){
			if(!level->transparentObjects[i]->listBuiltQuick || level->transparentObjects[i]->dirty){

		
				glDeleteLists(level->transparentObjects[i]->glListNameQuick,1);

				level->transparentObjects[i]->glListNameQuick=glGenLists(1);
				glNewList(level->transparentObjects[i]->glListNameQuick,GL_COMPILE);

				level->transparentObjects[i]->draw();
				glEndList();	

				level->transparentObjects[i]->listBuiltQuick=true;
			}
		}
	}
}

void doPicking(){

	if(!conf->pickerOn){
		return;
	}

	if(conf->pickerDraw=="nonstatic"){

		if(!mouse.lDown && !mouse.lUp){return;}

		if(level->nonstaticObjects.size()<1){
			return;
		}

		GLdouble modelMatrix[16];
		GLdouble projMatrix[16];
		GLint viewport[4];

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		level->camera->projectionMatrix();

		glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		level->camera->transform();


		level->camera->viewport();

		glGetIntegerv(GL_VIEWPORT,viewport);

		FloatVector3d o,d,ta,tb,ca,cb;
		GLdouble x,y,z;

		Array<int> finList;

		glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

		float mx=conf->pickerPosX;
		float my=conf->sizeY+conf->pickerPosY;

		int r=gluUnProject(mx,my,0.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
		o.x=x;
		o.y=y;
		o.z=z;
		
		r=gluUnProject(mx,my,1.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
		d.x=x;
		d.y=y;
		d.z=z;

		d=d-o;
		d.normalize();

		Array<Object*>* stack=&level->nonstaticObjects;

		int intadg=0;

		for(int s=0; s<2; s++){

			if(s==0){
				stack=&level->nonstaticObjects;
				intadg=0;
			}else{	
				stack=&level->transparentObjects;
				intadg=level->nonstaticObjects.size();
			}

			for(int i=0; i<(*stack).size(); i++){

				if((*stack)[i]->visible){

					if((*stack)[i]->box.intersect(o,d)){
						finList.pushBack(i+intadg);
					}
				}
			}
		}

		int eid=-1;
		float edist=-1;

		Matrix4d4d m;


		for(int i=0; i<finList.size(); i++){
			int ind=finList[i];

			if(ind<level->nonstaticObjects.size()){
				stack=&level->nonstaticObjects;
				intadg=0;
			}else{
				stack=&level->transparentObjects;
				ind-=level->nonstaticObjects.size();
				intadg=level->nonstaticObjects.size();
			}

			for(int v=0; v<(*stack)[ind]->getVertexCount(); v+=3){

				m.loadIdentity();
				m.translate((*stack)[ind]->pos.x,(*stack)[ind]->pos.y,(*stack)[ind]->pos.z);
				m.rotateZ((*stack)[ind]->rot.z);
				m.rotateY((*stack)[ind]->rot.y);
				m.rotateX((*stack)[ind]->rot.x);
				m.scale((*stack)[ind]->scale.x,(*stack)[ind]->scale.y,(*stack)[ind]->scale.z);

				#ifndef SOFTWARE_TRANSFORMS
					glMatrixMode(GL_MODELVIEW);
					glPushMatrix();

					glTranslatef((*stack)[ind]->pos.x,(*stack)[ind]->pos.y,(*stack)[ind]->pos.z);
					
					
					glRotatef((*stack)[ind]->rot.z,0.0f,0.0f,1.0f);
					glRotatef((*stack)[ind]->rot.y,0.0f,1.0f,0.0f);
					glRotatef((*stack)[ind]->rot.x,1.0f,0.0f,0.0f);

					glScalef((*stack)[ind]->scale.x,(*stack)[ind]->scale.y,(*stack)[ind]->scale.z);
				#endif

				glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

				#ifndef SOFTWARE_TRANSFORMS
					glMatrixMode(GL_MODELVIEW);
					glPopMatrix();
				#endif

				float mx=conf->pickerPosX;
				float my=conf->sizeY+conf->pickerPosY;

				int r=gluUnProject(mx,my,0.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
				o.x=x;o.y=y;o.z=z;
				
				r=gluUnProject(mx,my,1.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
				d.x=x;d.y=y;d.z=z;

				d=d-o;
				d.normalize();

				float dist=0;
				float b1=0;
				float b2=0;

				if(math.intersectRayTriangle(&dist,&b1,&b2, &o,&d,&(*stack)[ind]->getVertexRaw(v).pos,&(*stack)[ind]->getVertexRaw(v+1).pos,&(*stack)[ind]->getVertexRaw(v+2).pos)){
	
					FloatVector4d vp=FloatVector4d((*stack)[ind]->getVertexRaw(v).pos.x,(*stack)[ind]->getVertexRaw(v).pos.y,(*stack)[ind]->getVertexRaw(v).pos.z,1);
					FloatVector4d rdist=(m*vp)*b1;

					vp=FloatVector4d((*stack)[ind]->getVertexRaw(v+1).pos.x,(*stack)[ind]->getVertexRaw(v+1).pos.y,(*stack)[ind]->getVertexRaw(v+1).pos.z,1);

					rdist+=(m*vp)*b2;

					vp=FloatVector4d((*stack)[ind]->getVertexRaw(v+2).pos.x,(*stack)[ind]->getVertexRaw(v+2).pos.y,(*stack)[ind]->getVertexRaw(v+2).pos.z,1);

					rdist+=(m*vp)*(1-b1-b2);
					rdist-=level->camera->pos;
					dist=rdist.magnitude();

					if( (dist>0 && dist<edist) || (dist>0 && edist<0)){

						if(dist>conf->pickerNear && dist<conf->pickerFar){
							if(math.dotProduct(d,(*stack)[ind]->getVertexRaw(v).normal)<0){
								eid=ind+intadg;
								edist=dist;
							}else{
								
							}

						}else{
							
						}
					}
				}
			}
		}

		if(eid!=-1){
			if(mouse.lDown){

				if(eid<level->nonstaticObjects.size()){
					if(level->nonstaticObjects[eid]->action!=""){

						script().run(level->nonstaticObjects[eid]->action+"(\"mouseLDown\")");
					}

				}else{

					if(level->transparentObjects[eid-level->nonstaticObjects.size()]->action!=""){

						script().run(level->transparentObjects[eid-level->nonstaticObjects.size()]->action+"(\"mouseLDown\")");
					}

				}
			}	
		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}

}

void doPickingMatrix(){
	pickerMatrix.clear();
	pickerMatrix.posX=conf->pickerPosX;
	pickerMatrix.posY=conf->pickerPosY;

	for(int i=0; i<level->nonstaticObjects.size(); i++){
		pickerMatrix.add(i,level->nonstaticObjects[i]);
	}

	for(int i=0; i<level->transparentObjects.size(); i++){
		pickerMatrix.add(i+level->nonstaticObjects.size(),level->transparentObjects[i]);
	}

	Array<int> result=pickerMatrix.pick();

	if(result.size()>0){
		if(mouse.lUp){

		}else if(mouse.lDown){

			if(result[0]<level->nonstaticObjects.size()){
				if(level->nonstaticObjects[result[0]]->action!=""){
					script().run(level->nonstaticObjects[result[0]]->action+"(\"mouseLDown\")");
				}
			}else{
				if(level->transparentObjects[result[0]-level->nonstaticObjects.size()]->action!=""){
					script().run(level->transparentObjects[result[0]-level->nonstaticObjects.size()]->action+"(\"mouseLDown\")");
				}
			}
		}
	}
}

void drawRefreshAll(){
	buildStaticList();
	listBuild=true;

	for(int i=0; i<level->nonstaticObjects.size(); i++){
		level->nonstaticObjects[i]->dirty=true;
	}
}

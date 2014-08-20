#ifdef DEVELOPER

#include "editor.h"
#include "level.hpp"
#include "Helperlib/HelperLibMath.hpp"
#include "Helperlib/Log.hpp"

#include "keyboard.h"
#ifdef DEVELOPER

#include "mouse.h"
#include "texturelibrary.h"
#include "conf.h"
#include "lightmap.h"
#include "Audio/Audio.h"
#include "texture.hpp"
#include "draw.hpp"
#include "timeing.hpp"


void Editor::drawArrowCamera(){

	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-0.75f,-0.85,0);
	level->camera->projectionMatrix();
	glTranslatef(0,0,0);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	FloatVector3d dist=cameraCenter-level->camera->pos;

	float camdist=math.sqrt(dist.x*dist.x+dist.y*dist.y+dist.z*dist.z)*0.5;

	glDisable(GL_DEPTH_TEST);

	glTranslatef(cameraCenter.x,cameraCenter.y,cameraCenter.z);
	glRotatef(90,0,1,0);

	glColor3f(1,0.0,0.0);

	gluQuadricDrawStyle(quad,GLU_FILL);
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.2)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);
	gluCylinder(quad,float(0.04)*camdist,0,float(0.1)*camdist,11,11);
	glTranslatef(0,0,-float(0.2)*camdist);

	glRotatef(-90,0,1,0);

	glColor3f(0.0,0.0,1);

	gluQuadricDrawStyle(quad,GLU_FILL);
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.2)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);
	gluCylinder(quad,float(0.04)*camdist,0,float(0.1)*camdist,11,11);
	glTranslatef(0,0,-float(0.2)*camdist);

	glRotatef(270,1,0,0);

	glColor3f(0.0,1,0.0);

	gluQuadricDrawStyle(quad,GLU_FILL);
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.2)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);
	gluCylinder(quad,float(0.04)*camdist,0,float(0.1)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);

	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glEnable(GL_CULL_FACE);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

}


void Editor::drawArrowTranslate(FloatVector3d pos){

	glDisable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	FloatVector3d dist=pos-level->camera->pos;

	float camdist=math.sqrt(dist.x*dist.x+dist.y*dist.y+dist.z*dist.z);

	if(cameraMode=="top"){
		camdist=10;
	}

	glDisable(GL_DEPTH_TEST);

	glTranslatef(pos.x,pos.y,pos.z);
	glRotatef(90,0,1,0);

	if(mouse.l){
		glColor3f(1.0,0.0,0.0);
	}else{
		glColor3f(0.8,0.0,0.0);
	}

	gluQuadricDrawStyle(quad,GLU_FILL);
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.2)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);
	gluCylinder(quad,float(0.04)*camdist,0,float(0.1)*camdist,11,11);
	glTranslatef(0,0,-float(0.2)*camdist);

	glRotatef(-90,0,1,0);

	if(mouse.r){
		glColor3f(0.0,0.0,1.0);
	}else{
		glColor3f(0.0,0.0,0.8);
	}
	gluQuadricDrawStyle(quad,GLU_FILL);
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.2)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);
	gluCylinder(quad,float(0.04)*camdist,0,float(0.1)*camdist,11,11);
	glTranslatef(0,0,-float(0.2)*camdist);

	glRotatef(270,1,0,0);

	if(mouse.m){
		glColor3f(0.0,1.0,0.0);
	}else{
		glColor3f(0.0,0.8,0.0);
	}
	gluQuadricDrawStyle(quad,GLU_FILL);
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.2)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);
	gluCylinder(quad,float(0.04)*camdist,0,float(0.1)*camdist,11,11);
	glTranslatef(0,0,float(0.2)*camdist);

	glTranslatef(-pos.x,-pos.y,-pos.z);

	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glEnable(GL_CULL_FACE);
}

int list=-1;

void genCubeList(){
	list=glGenLists(1);

	glNewList(list,GL_COMPILE);

	glBegin(GL_QUADS);									// Draw A Quad
		glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Top)
		glVertex3f(-1.0f, 1.0f, 1.0f);					// Bottom Left Of The Quad (Top)
		glVertex3f( 1.0f, 1.0f, 1.0f);					// Bottom Right Of The Quad (Top)
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Top Right Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Top Left Of The Quad (Bottom)
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Bottom)
		glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Bottom)
		glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Front)
		glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Front)
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Front)
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Front)
		glVertex3f( 1.0f,-1.0f,-1.0f);					// Top Right Of The Quad (Back)
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Top Left Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f,-1.0f);					// Bottom Left Of The Quad (Back)
		glVertex3f( 1.0f, 1.0f,-1.0f);					// Bottom Right Of The Quad (Back)
		glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Left)
		glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Left)
		glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Left)
		glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Right)
		glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Right)
		glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Right)
	glEnd();	

	glEndList();
}

void Editor::drawArrowScale(FloatVector3d pos){

	glDisable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	if(list==-1){genCubeList();}

	FloatVector3d dist=pos-level->camera->pos;

	float camdist=math.sqrt(dist.x*dist.x+dist.y*dist.y+dist.z*dist.z);

	glDisable(GL_DEPTH_TEST);

	glTranslatef(pos.x,pos.y,pos.z);
	glRotatef(90,0,1,0);

	if(mouse.l){
		glColor3f(1.0,0.0,0.0);
	}else{
		glColor3f(0.8,0.0,0.0);
	}

	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.25)*camdist,11,11);
	glTranslatef(0,0,float(0.25)*camdist);
	glScalef(float(0.03)*camdist,float(0.03)*camdist,float(0.03)*camdist);
	glCallList(list);
	glScalef(float(1)/(float(0.03)*camdist),float(1)/(float(0.03)*camdist),float(1)/(float(0.03)*camdist));
	glTranslatef(0,0,-float(0.25)*camdist);

	glRotatef(-90,0,1,0);

	if(mouse.r){
		glColor3f(0.0,0.0,1.0);
	}else{
		glColor3f(0.0,0.0,0.8);
	}
	
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.25)*camdist,11,11);
	glTranslatef(0,0,float(0.25)*camdist);
	glScalef(float(0.03)*camdist,float(0.03)*camdist,float(0.03)*camdist);
	glCallList(list);
	glScalef(float(1)/(float(0.03)*camdist),float(1)/(float(0.03)*camdist),float(1)/(float(0.03)*camdist));
	glTranslatef(0,0,-float(0.25)*camdist);

	glRotatef(270,1,0,0);

	if(mouse.m){
		glColor3f(0.0,1.0,0.0);
	}else{
		glColor3f(0.0,0.8,0.0);
	}
	
	gluCylinder(quad,float(0.005)*camdist,float(0.005)*camdist,float(0.25)*camdist,11,11);
	glTranslatef(0,0,float(0.25)*camdist);
	glScalef(float(0.03)*camdist,float(0.03)*camdist,float(0.03)*camdist);
	glCallList(list);
	glScalef(float(1)/(float(0.03)*camdist),float(1)/(float(0.03)*camdist),float(1)/(float(0.03)*camdist));
	glTranslatef(0,0,-float(0.25)*camdist);

	glTranslatef(-pos.x,-pos.y,-pos.z);

	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glEnable(GL_CULL_FACE);
}


int rlist=-1;

void genRotList(){
	rlist=glGenLists(1);

	glNewList(rlist,GL_COMPILE);

	glBegin(GL_QUADS);									// Draw A Quad
		
	float r=1;
	float zd=0.05;

	for(float rpos=10; rpos<370; rpos+=10){
		glVertex3f(math.cosDegrees(rpos-10)*r,math.sinDegrees(rpos-10)*r,-zd);
		glVertex3f(math.cosDegrees(rpos)*r,math.sinDegrees(rpos)*r,-zd);
		glVertex3f(math.cosDegrees(rpos)*r,math.sinDegrees(rpos)*r,zd);
		glVertex3f(math.cosDegrees(rpos-10)*r,math.sinDegrees(rpos-10)*r,zd);
	}

	glEnd();	

	glEndList();
}
void Editor::drawArrowRotate(FloatVector3d pos){

	if(rlist==-1){genRotList();}

	glDisable(GL_CULL_FACE);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glEnable(GL_CULL_FACE);

	FloatVector3d dist=pos-level->camera->pos;

	float camdist=math.sqrt(dist.x*dist.x+dist.y*dist.y+dist.z*dist.z);

	glDisable(GL_DEPTH_TEST);

	glTranslatef(pos.x,pos.y,pos.z);
	glRotatef(90,0,1,0);

	if(mouse.l){
		glColor3f(1.0,0.0,0.0);
	}else{
		glColor3f(0.8,0.0,0.0);
	}

	glScalef(float(0.27)*camdist,float(0.27)*camdist,float(0.27)*camdist);
	glCallList(rlist);
	glScalef(float(1)/(float(0.27)*camdist),float(1)/(float(0.27)*camdist),float(1)/(float(0.27)*camdist));

	glRotatef(-90,0,1,0);

	if(mouse.r){
		glColor3f(0.0,0.0,1.0);
	}else{
		glColor3f(0.0,0.0,0.8);
	}

	glScalef(float(0.27)*camdist,float(0.27)*camdist,float(0.27)*camdist);
	glCallList(rlist);
	glScalef(float(1)/(float(0.27)*camdist),float(1)/(float(0.27)*camdist),float(1)/(float(0.27)*camdist));

	glRotatef(270,1,0,0);

	if(mouse.m){
		glColor3f(0.0,1.0,0.0);
	}else{
		glColor3f(0.0,0.8,0.0);
	}

	glScalef(float(0.27)*camdist,float(0.27)*camdist,float(0.27)*camdist);
	glCallList(rlist);
	glScalef(float(1)/(float(0.27)*camdist),float(1)/(float(0.27)*camdist),float(1)/(float(0.27)*camdist));

	glTranslatef(-pos.x,-pos.y,-pos.z);

	glEnable(GL_DEPTH_TEST);

	glPopMatrix();
	glEnable(GL_CULL_FACE);
}

void Editor::drawStatus(){

	if(visible || mouseVisibleHack){
		mouse.color=oldEditorMouseColor;
		mouse.visible=oldEditorMouseVisible;
		
		mouseVisibleHack=false;
	}

	

	if(!renderLMMode){
		return;
	}

	if(statusTexId==-1){

		statusTexId=textureLibrary.load(String("rendering.tga"));
	}

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();

	glOrtho(0,conf->sizeX,-conf->sizeY,0,-100,100);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix

	glLoadIdentity ();	
	

	glEnable(GL_BLEND);
	glDisable(GL_LIGHTING);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);

	glDisable(GL_CULL_FACE);

	

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	shader.reset();

#ifdef LOOP_DEBUG
	logs().main.write("drawStatus texture DISABLE");
#endif

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE5_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	float amb[4];

	amb[0]=1;
	amb[1]=1;
	amb[2]=1;
	amb[3]=1;

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);

	glDisable (GL_FOG);


	glColor4f(1,1,1,1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,statusTexId);


	float sx=190;
	float sy=50;
	
	float px=float(conf->sizeX)*float(0.5)-95;
	float py=-float(conf->sizeY)+100;
	float pz=0;

	glBegin(GL_QUADS);
		glTexCoord2d(0,0); glVertex3f(px		,py,pz);
		glTexCoord2d(1,0); glVertex3f(px+sx	,py,pz);
		glTexCoord2d(1,-1); glVertex3f(px+sx	,py-sy,pz);
		glTexCoord2d(0,-1); glVertex3f(px		,py-sy,pz);
	glEnd();


	//mouse visibility
	
}

void Editor::centerAll(){
	
	if(level->nonstaticObjects.size()==0  && level->lights.size()==0){
		return;
	}

	Box3d extent;

	FloatVector3d center;

	float cnt=0;

	for(int i=0; i<level->nonstaticObjects.size(); i++){

		if(level->nonstaticObjects[i]->visible){
			center+=level->nonstaticObjects[i]->pos;

			extent+=level->nonstaticObjects[i]->box;
			cnt++;
		}
	}

	for(int i=0; i<level->lights.size(); i++){

		center+=level->lights[i]->pos;

		Box3d t;
		t.nx=level->lights[i]->pos.x;
		t.px=level->lights[i]->pos.x;
		t.ny=level->lights[i]->pos.y;
		t.py=level->lights[i]->pos.y;
		t.nz=level->lights[i]->pos.z;
		t.pz=level->lights[i]->pos.z;
		extent+=t;
		cnt++;
	}

	center/=cnt;

	FloatVector3d crot=-math.degreesToNormal(level->camera->rot);

	float dist=extent.magnitude();

	FloatVector3d cpos=center+crot*dist;
	level->camera->pos=cpos;
	
	cameraCenter=center;
	cameraDistance=dist;
}

void Editor::centerSelected(){
	if(selectedObjects.size()==0 && selectedLights.size()==0){
		return;
	}

	Box3d extent;
	FloatVector3d center;

	float cnt=0;

	for(int i=0; i<selectedObjects.size(); i++){

		if(selectedPolygons[i].size()>0){
			for(int v=0; v<selectedPolygons[i].size(); v++){

				for(int a=0; a<3; a++){
					FloatVector4d rpos;
					rpos.x=selectedObjects[i]->getVertexRaw(selectedPolygons[i][v]*3+a).pos.x;
					rpos.y=selectedObjects[i]->getVertexRaw(selectedPolygons[i][v]*3+a).pos.y;
					rpos.z=selectedObjects[i]->getVertexRaw(selectedPolygons[i][v]*3+a).pos.z;
					rpos.w=1;

					Matrix4d4d m;
					m.loadIdentity();
					m.translate(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);			
					m.rotateZ(selectedObjects[i]->rot.z);
					m.rotateY(selectedObjects[i]->rot.y);
					m.rotateX(selectedObjects[i]->rot.x);
					m.scale(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);

					rpos=m*rpos;

					center.x+=rpos.x;
					center.y+=rpos.y;
					center.z+=rpos.z;

					extent+=rpos;

					cnt++;
				}

			}

		}else if(selectedEdges[i].size()>0){
			for(int v=0; v<selectedEdges[i].size(); v++){

				for(int a=0; a<2; a++){

					int inc=0;

					if(selectedEdges[i][v] % 3 == 0){
						inc=a;
					}else if(selectedEdges[i][v] % 3 == 1){
						inc=a;
					}else if(selectedEdges[i][v] % 3 == 2){
						if(a==0){inc=0;}else{inc=-2;}
					}

					FloatVector4d rpos;
					rpos.x=selectedObjects[i]->getVertexRaw(selectedEdges[i][v]+inc).pos.x;
					rpos.y=selectedObjects[i]->getVertexRaw(selectedEdges[i][v]+inc).pos.y;
					rpos.z=selectedObjects[i]->getVertexRaw(selectedEdges[i][v]+inc).pos.z;
					rpos.w=1;

					Matrix4d4d m;
					m.loadIdentity();
					m.translate(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);			
					m.rotateZ(selectedObjects[i]->rot.z);
					m.rotateY(selectedObjects[i]->rot.y);
					m.rotateX(selectedObjects[i]->rot.x);
					m.scale(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);

					rpos=m*rpos;

					center.x+=rpos.x;
					center.y+=rpos.y;
					center.z+=rpos.z;

					extent+=rpos;

					cnt++;
				}

			}
		}else if(selectedVertices[i].size()>0){
			for(int v=0; v<selectedVertices[i].size(); v++){
				FloatVector4d rpos;
				rpos.x=selectedObjects[i]->getVertexRaw(selectedVertices[i][v]).pos.x;
				rpos.y=selectedObjects[i]->getVertexRaw(selectedVertices[i][v]).pos.y;
				rpos.z=selectedObjects[i]->getVertexRaw(selectedVertices[i][v]).pos.z;
				rpos.w=1;

				Matrix4d4d m;
				m.loadIdentity();
				m.translate(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);			
				m.rotateZ(selectedObjects[i]->rot.z);
				m.rotateY(selectedObjects[i]->rot.y);
				m.rotateX(selectedObjects[i]->rot.x);
				m.scale(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);

				rpos=m*rpos;

				center.x+=rpos.x;
				center.y+=rpos.y;
				center.z+=rpos.z;

				extent+=rpos;

				cnt++;
			}
		}else{
			center+=selectedObjects[i]->pos;

			if(!selectedObjects[i]->boxBuilt){
				selectedObjects[i]->update();
			}

			extent+=selectedObjects[i]->box;

			cnt++;
		}
	}

	for(int i=0; i<selectedLights.size(); i++){
		center+=selectedLights[i]->pos;

		Box3d t;
		t.nx=selectedLights[i]->pos.x;
		t.px=selectedLights[i]->pos.x;
		t.ny=selectedLights[i]->pos.y;
		t.py=selectedLights[i]->pos.y;
		t.nz=selectedLights[i]->pos.z;
		t.pz=selectedLights[i]->pos.z;
		extent+=t;
		cnt++;
	}

	center/=cnt;

	FloatVector3d crot=-math.degreesToNormal(level->camera->rot);

	float dist=extent.magnitude();

	FloatVector3d cpos=center+crot*dist;
	level->camera->pos=cpos;

	cameraCenter=center;
	cameraDistance=dist;
}

void Editor::moveEdges(int oi,float x,float y, float z){

	Array<int> vlist;

	for(int i=0; i<selectedEdges[oi].size(); i++){

		if(selectedEdges[oi][i]%3 == 2){
			if(vlist.find(selectedEdges[oi][i])==-1){
				vlist.pushBack(selectedEdges[oi][i]);
			}

			if(vlist.find(selectedEdges[oi][i]-2)==-1){
				vlist.pushBack(selectedEdges[oi][i]-2);
			}

			for(int m=0; m<selectedObjects[oi]->getVertexCount(); m++){
				if(selectedObjects[oi]->getVertexRaw(m).pos==selectedObjects[oi]->getVertexRaw(selectedEdges[oi][i]).pos){
					if(vlist.find(m)==-1){
						vlist.pushBack(m);
					}
				}
			}

			for(int m=0; m<selectedObjects[oi]->getVertexCount(); m++){
				if(selectedObjects[oi]->getVertexRaw(m).pos==selectedObjects[oi]->getVertexRaw(selectedEdges[oi][i]-2).pos){
					if(vlist.find(m)==-1){
						vlist.pushBack(m);
					}
				}
			}
		
		}else{
			if(vlist.find(selectedEdges[oi][i])==-1){
				vlist.pushBack(selectedEdges[oi][i]);
			}

			if(vlist.find(selectedEdges[oi][i]+1)==-1){
				vlist.pushBack(selectedEdges[oi][i]+1);
			}

			for(int m=0; m<selectedObjects[oi]->getVertexCount(); m++){
				if(selectedObjects[oi]->getVertexRaw(m).pos==selectedObjects[oi]->getVertexRaw(selectedEdges[oi][i]).pos){
					if(vlist.find(m)==-1){
						vlist.pushBack(m);
					}
				}
			}

			for(int m=0; m<selectedObjects[oi]->getVertexCount(); m++){
				if(selectedObjects[oi]->getVertexRaw(m).pos==selectedObjects[oi]->getVertexRaw(selectedEdges[oi][i]+1).pos){
					if(vlist.find(m)==-1){
						vlist.pushBack(m);
					}
				}
			}
		}
	}

	for(int p=0; p<vlist.size(); p++){	
		Vertex* v1=&selectedObjects[oi]->setVertexRaw(vlist[p]);

		v1->pos.x+=x;
		v1->pos.y+=y;
		v1->pos.z+=z;
	}

	selectedObjects[oi]->dirty=true;
	selectedObjects[oi]->listBuilt=false;
	selectedObjects[oi]->listBuiltQuick=false;
	selectedObjects[oi]->listBuiltQuickPolygon=false;
	selectedObjects[oi]->listBuiltQuickVertex=false;
	selectedObjects[oi]->listBuiltQuickEdge=false;
}


void Editor::moveVertices(int oi,float x,float y, float z){

	for(int p=0; p<selectedVertices[oi].size(); p++){	
		Vertex* v1=&selectedObjects[oi]->setVertexRaw(selectedVertices[oi][p]);

		v1->pos.x+=x;
		v1->pos.y+=y;
		v1->pos.z+=z;
	}

	selectedObjects[oi]->dirty=true;
	selectedObjects[oi]->listBuilt=false;
	selectedObjects[oi]->listBuiltQuick=false;
	selectedObjects[oi]->listBuiltQuickPolygon=false;
	selectedObjects[oi]->listBuiltQuickVertex=false;
	selectedObjects[oi]->listBuiltQuickEdge=false;
}


void Editor::scalePolygons(int i,float x,float y, float z){
	
	Array<int> vlist;

	for(int p=0; p<selectedPolygons[i].size(); p++){
		if(vlist.find(selectedPolygons[i][p]*3)==-1){vlist.pushBack(selectedPolygons[i][p]*3);}
		if(vlist.find(selectedPolygons[i][p]*3+1)==-1){vlist.pushBack(selectedPolygons[i][p]*3+1);}
		if(vlist.find(selectedPolygons[i][p]*3+2)==-1){vlist.pushBack(selectedPolygons[i][p]*3+2);}

		for(int m=0; m<selectedObjects[i]->getVertexCount(); m++){
			if(selectedObjects[i]->getVertexRaw(m).pos==selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).pos ||
				selectedObjects[i]->getVertexRaw(m).pos==selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).pos ||
				selectedObjects[i]->getVertexRaw(m).pos==selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).pos){

				if(vlist.find(m)==-1){
					vlist.pushBack(m);
				}
			}
		}
	}
	
	for(int p=0; p<vlist.size(); p++){
		Vertex* v1=&selectedObjects[i]->setVertexRaw(vlist[p]);
		
		v1->pos.x*=x;
		v1->pos.y*=y;
		v1->pos.z*=z;
	}
	
	selectedObjects[i]->dirty=true;
	selectedObjects[i]->listBuilt=false;
	selectedObjects[i]->listBuiltQuick=false;
	selectedObjects[i]->listBuiltQuickPolygon=false;
	selectedObjects[i]->listBuiltQuickVertex=false;
	selectedObjects[i]->listBuiltQuickEdge=false;
	
}

void Editor::movePolygons(int i,float x,float y, float z){
	//for(int i=0; i<selectedPolygons.size(); i++){

	Array<int> vlist;

	for(int p=0; p<selectedPolygons[i].size(); p++){
		if(vlist.find(selectedPolygons[i][p]*3)==-1){vlist.pushBack(selectedPolygons[i][p]*3);}
		if(vlist.find(selectedPolygons[i][p]*3+1)==-1){vlist.pushBack(selectedPolygons[i][p]*3+1);}
		if(vlist.find(selectedPolygons[i][p]*3+2)==-1){vlist.pushBack(selectedPolygons[i][p]*3+2);}

		for(int m=0; m<selectedObjects[i]->getVertexCount(); m++){
			if(selectedObjects[i]->getVertexRaw(m).pos==selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).pos ||
				selectedObjects[i]->getVertexRaw(m).pos==selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).pos ||
				selectedObjects[i]->getVertexRaw(m).pos==selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).pos){

				if(vlist.find(m)==-1){
					vlist.pushBack(m);
				}
			}
		}
	}
	
	for(int p=0; p<vlist.size(); p++){
		Vertex* v1=&selectedObjects[i]->setVertexRaw(vlist[p]);
		
		v1->pos.x+=x;
		v1->pos.y+=y;
		v1->pos.z+=z;
	}
	
	selectedObjects[i]->dirty=true;
	selectedObjects[i]->listBuilt=false;
	selectedObjects[i]->listBuiltQuick=false;
	selectedObjects[i]->listBuiltQuickPolygon=false;
	selectedObjects[i]->listBuiltQuickVertex=false;
	selectedObjects[i]->listBuiltQuickEdge=false;
}

void Editor::rebuildSelection(){
	for(int i=0; i<selectedObjects.size(); i++){
		selectedObjects[i]->dirty=true;
		selectedObjects[i]->listBuilt=false;
		selectedObjects[i]->listBuiltQuick=false;
		selectedObjects[i]->listBuiltQuickPolygon=false;
		selectedObjects[i]->listBuiltQuickVertex=false;
		selectedObjects[i]->listBuiltQuickEdge=false;
	}

}

const GLubyte mask[128]={0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55};

void Editor::draw(){

	if(toolCommands.textpanel->mouseover){
		toolCommands.textpanel->color.x=0;
		toolCommands.textpanel->color.y=0;
		toolCommands.textpanel->color.z=0;

	}else{
		toolCommands.textpanel->color.x=1;
		toolCommands.textpanel->color.y=1;
		toolCommands.textpanel->color.z=1;
	}

	if(renderLMMode){
		for(int i=0; i<selectedObjects.size(); i++){

			if(!lmSizes.find(selectedObjects[i]->name)){
				IntVector2d v(256,256);
				lmSizes[selectedObjects[i]->name]=v;
			}

			lightmap.build(selectedObjects[i],lmSizes[selectedObjects[i]->name].x,lmSizes[selectedObjects[i]->name].y);
		}

		renderLMMode=false;
	}

	if(keyboard.primeUp){
		visible=!visible;

		if(!visible){
			hideGUIs=true;
			forceGui("main",true);
		}
	}else{
		hideGUIs=false;
	}

	if(visible){
		for(int i=0; i<grid.size(); i++){
			grid[i].line->visible=true;
		}
	}else{
		for(int i=0; i<grid.size(); i++){
			grid[i].line->visible=false;
		}
	}

	if(visible && (expertMode==0 || expertMode==1)){

		toolCommands.textpanel->visible=true;
		viewTitle.textpanel->visible=true;

		viewTitle.textpanel->pos.x=conf->sizeX/float(2)-viewTitle.textpanel->scale.x/float(2);

		if(expertMode==1){
			toolCommands.textpanel->pos.x=conf->sizeX-10;
			stickyToolCommands.textpanel->pos.x=conf->sizeX-10;
		}else{
			toolCommands.textpanel->pos.x=conf->sizeX-60;
			stickyToolCommands.textpanel->pos.x=conf->sizeX-60;
		}
	}else{
		toolCommands.textpanel->visible=false;
		stickyToolCommands.textpanel->visible=false;
		viewTitle.textpanel->visible=false;
	}

	if(visible && expertMode==0){
		editorLeftBkg.panel->visible=true;
		editorRightBkg.panel->visible=true;
		editorTopBkg.panel->visible=true;
		editorBottomBkg.panel->visible=true;

		for(int i=0; i<buttonsL.size(); i++){
			if(buttonsLB[i].panel!=NULL){
				buttonsLB[i].panel->visible=true;
				buttonsLI[i].panel->visible=true;
				buttonsL[i].panel->visible=true;
			}
		}

		for(int i=0; i<buttonsRB.size(); i++){
			buttonsRB[i].panel->visible=true;
			buttonsRI[i].panel->visible=true;
			buttonsR[i].panel->visible=true;
		}

		for(int i=0; i<buttonsT.size(); i++){
			buttonsTB[i].panel->visible=true;
			buttonsTI[i].panel->visible=true;
			buttonsT[i].panel->visible=true;
		}
		
		for(int i=0; i<buttonsB.size(); i++){
			buttonsBB[i].panel->visible=true;
			buttonsBI[i].panel->visible=true;
			buttonsB[i].panel->visible=true;
		}
	}else{
		editorLeftBkg.panel->visible=false;
		editorRightBkg.panel->visible=false;
		editorTopBkg.panel->visible=false;
		editorBottomBkg.panel->visible=false;
		
		tooltipBkg.panel->visible=false;
		tooltipText.textpanel->visible=false;

		for(int i=0; i<buttonsL.size(); i++){
			if(buttonsL[i].panel!=NULL){
				buttonsLB[i].panel->visible=false;
				buttonsLI[i].panel->visible=false;
				buttonsL[i].panel->visible=false;
			}
		}

		for(int i=0; i<buttonsR.size(); i++){
			buttonsRB[i].panel->visible=false;
			buttonsRI[i].panel->visible=false;
			buttonsR[i].panel->visible=false;
		}
		
		for(int i=0; i<buttonsT.size(); i++){
			buttonsTB[i].panel->visible=false;
			buttonsTI[i].panel->visible=false;
			buttonsT[i].panel->visible=false;
		}
		
		for(int i=0; i<buttonsB.size(); i++){
			buttonsBB[i].panel->visible=false;
			buttonsBI[i].panel->visible=false;
			buttonsB[i].panel->visible=false;
		}

		
	}

	

	if(visible && scriptEditor){
		scriptBkg.panel->visible=true;
		scriptText.textpanel->visible=true;
	}else{
		scriptBkg.panel->visible=false;
		scriptText.textpanel->visible=false;
	}



	if(!visible){
		if(oldEditorMouseTextureId!=-1){
			mouse.textureId=oldEditorMouseTextureId;
			oldEditorMouseTextureId=-1;
			mouse.textureScale.x=oldEditorMouseScale.x;
			mouse.textureScale.y=oldEditorMouseScale.y;
		}
	}


	if(visible || hideGUIs){
		tools();

		drawProjects();
		drawExplorer();
		drawAlerts();
		drawUv();
		drawScript();
		drawProperties();
		drawMaterial();
		drawNew();
	}
	
	if(!visible){return;}

	oldEditorMouseVisible=mouse.visible;
	oldEditorMouseColor=mouse.color;

	if(oldEditorMouseTextureId==-1){
		oldEditorMouseTextureId=mouse.textureId;
		oldEditorMouseScale.x=mouse.textureScale.x;
		oldEditorMouseScale.y=mouse.textureScale.y;
	}

	mouseVisibleHack=true;
	mouse.visible=true;
	mouse.color.x=1;
	mouse.color.y=1;
	mouse.color.z=1;
	mouse.color.w=1;
	
	if(mousetexid==-1){
		mousetexid=textureLibrary.load(String("mouse.tga"));
	}

	if(mouse.textureId!=mousetexid){
		mouse.textureId=mousetexid;
		ImageData id=texture.info(mousetexid);

		mouse.textureScale.x=id.width;
		mouse.textureScale.y=id.height;

	}

	tooltipBkg.panel->visible=false;
	tooltipText.textpanel->visible=false;

	FloatVector4d selected=FloatVector4d(0.2,0.2,0.9,1.0);
	FloatVector4d selectedI=FloatVector4d(0.8,0.8,1,1.0);

	bool lreverse=false;
	bool lreverseT=false;

	for(int i=0; i<buttonsL.size(); i++){

		if(lreverse){
			buttonsL[i].panel->pos.y=buttonsLB[i-1].panel->pos.y+52;
			buttonsLB[i].panel->pos.y=buttonsLB[i-1].panel->pos.y+50;
			buttonsLI[i].panel->pos.y=buttonsLB[i-1].panel->pos.y+52;
		}

		if(lreverseT && !lreverse){
			buttonsL[i].panel->pos.y=-conf->sizeY+50;
			buttonsLB[i].panel->pos.y=-conf->sizeY+48;
			buttonsLI[i].panel->pos.y=-conf->sizeY+50;
			lreverse=true;
		}


		if(buttonsLB[i].panel==NULL){
			lreverseT=true;

			

		}else if(buttonsLB[i].panel->mouseover){
			buttonsLB[i].panel->color=FloatVector4d(0.6,0.6,0.6,1.0);

			tooltipBkg.panel->visible=true;
			tooltipText.textpanel->visible=true;

			if(buttonsL[i].panel->name=="editorButtonLSelect Objects"){
				tooltipText.textpanel->setText("Select Objects [space]");
			}else if(buttonsL[i].panel->name=="editorButtonLSelect Vertices"){
				tooltipText.textpanel->setText("Select Vertices [t]");
			}else if(buttonsL[i].panel->name=="editorButtonLSelect Polygons"){
				tooltipText.textpanel->setText("Select Polygons [u]");
			}else if(buttonsL[i].panel->name=="editorButtonLSelect Edges"){
				tooltipText.textpanel->setText("Select Edges [e]");
			}else if(buttonsL[i].panel->name=="editorButtonLtranslate"){
				tooltipText.textpanel->setText("Translate [v]");
			}else if(buttonsL[i].panel->name=="editorButtonLrotate"){
				tooltipText.textpanel->setText("Rotate [c]");
			}else if(buttonsL[i].panel->name=="editorButtonLscale"){
				tooltipText.textpanel->setText("Scale [x]");
			}else if(buttonsL[i].panel->name=="editorButtonLnavigate"){
				tooltipText.textpanel->setText("Navigate [s]");
			}else if(buttonsL[i].panel->name=="editorButtonLScript Editor"){
				tooltipText.textpanel->setText("Script Editor [ctrl-r]");
			}else if(buttonsL[i].panel->name=="editorButtonLProperties"){
				tooltipText.textpanel->setText("Properties [ctrl-d]");
			}else if(buttonsL[i].panel->name=="editorButtonLUV Editor"){
				tooltipText.textpanel->setText("UV Editor [ctrl-u]");
			}else if(buttonsL[i].panel->name=="editorButtonLExplorer"){
				tooltipText.textpanel->setText("Explorer [ctrl-e]");
			}else if(buttonsL[i].panel->name=="editorButtonLMaterial Editor"){
				tooltipText.textpanel->setText("Material Editor [ctrl-m]");
			}

		}else{
			if(buttonsL[i].panel->name=="editorButtonLtranslate" && toolActive("translate","main") ||
				buttonsL[i].panel->name=="editorButtonLrotate" && toolActive("rotate","main") ||
				buttonsL[i].panel->name=="editorButtonLscale" && toolActive("scale","main") ||
				buttonsL[i].panel->name=="editorButtonLnavigate" && toolActive("navigate","main") ||
				buttonsL[i].panel->name=="editorButtonLSelect Objects" && toolActive("selectobjects","main") ||
				buttonsL[i].panel->name=="editorButtonLSelect Polygons" && toolActive("selectpolygons","main") ||
				buttonsL[i].panel->name=="editorButtonLSelect Edges" && toolActive("selectedges","main") ||
				buttonsL[i].panel->name=="editorButtonLSelect Vertices" && toolActive("selectvertices","main")){

				buttonsLB[i].panel->color=selected;
				buttonsLI[i].panel->color=selectedI;
			}else{
				buttonsLB[i].panel->color=FloatVector4d(0.4,0.4,0.4,1.0);
				buttonsLI[i].panel->color=FloatVector4d(1,1,1,1.0);
			}
		}
	}

	

	editorRightBkg.panel->pos.x=conf->sizeX-50;
	editorTopBkg.panel->scale.x=conf->sizeX-100;
	editorBottomBkg.panel->scale.x=conf->sizeX-100;
	editorBottomBkg.panel->pos.y=-conf->sizeY+50;

	

	for(int i=0; i<buttonsR.size(); i++){
		buttonsRB[i].panel->pos.x=conf->sizeX-48;
		buttonsRI[i].panel->pos.x=conf->sizeX-48;
		buttonsR[i].panel->pos.x=conf->sizeX-50;

		if(buttonsRB[i].panel->mouseover){
			buttonsRB[i].panel->color=FloatVector4d(0.6,0.6,0.6,1.0);

			tooltipBkg.panel->visible=true;
			tooltipText.textpanel->visible=true;

			if(buttonsR[i].panel->name=="editorButtonRAdd Cube"){
				tooltipText.textpanel->setText("Add Cube [ctrl-n c]");
			}else if(buttonsR[i].panel->name=="editorButtonRAdd Point Light"){
				tooltipText.textpanel->setText("Add Point Light [ctrl-n l]");
			}else if(buttonsR[i].panel->name=="editorButtonRAdd Spotlight"){
				tooltipText.textpanel->setText("Add Spotlight [ctrl-n s]");
			}else if(buttonsR[i].panel->name=="editorButtonRRebuild Lightmap"){
				tooltipText.textpanel->setText("Build Lightmap [b]");
			}else if(buttonsR[i].panel->name=="editorButtonRDelete Selected"){
				tooltipText.textpanel->setText("Delete Selected [delete]");
			}else if(buttonsR[i].panel->name=="editorButtonRAdd Grid"){
				tooltipText.textpanel->setText("Add Grid [ctrl-n g]");
			}

		}else{
			buttonsRB[i].panel->color=FloatVector4d(0.4,0.4,0.4,1.0);
		}
	}

	for(int i=0; i<buttonsB.size(); i++){
		buttonsBB[i].panel->pos.y=-conf->sizeY+48;
		buttonsBI[i].panel->pos.y=-conf->sizeY+48;
		buttonsB[i].panel->pos.y=-conf->sizeY+50;
		
		if(buttonsBB[i].panel->mouseover){
			buttonsBB[i].panel->color=FloatVector4d(0.6,0.6,0.6,1.0);

			tooltipBkg.panel->visible=true;
			tooltipText.textpanel->visible=true;

			if(buttonsB[i].panel->name=="editorButtonBScript Editor"){
				tooltipText.textpanel->setText("Script Editor [ctrl-r]");
			}else if(buttonsB[i].panel->name=="editorButtonBProperties"){
				tooltipText.textpanel->setText("Properties [ctrl-d]");
			}else if(buttonsB[i].panel->name=="editorButtonBUV Editor"){
				tooltipText.textpanel->setText("UV Editor [ctrl-d]");
			}else if(buttonsB[i].panel->name=="editorButtonBExplorer"){
				tooltipText.textpanel->setText("Explorer [ctrl-e]");
			}else if(buttonsB[i].panel->name=="editorButtonBFrame All"){
				tooltipText.textpanel->setText("Frame All [a]");
			}else if(buttonsB[i].panel->name=="editorButtonBFrame Selected"){
				tooltipText.textpanel->setText("Frame Selected [f]");
			}else if(buttonsB[i].panel->name=="editorButtonBSelect All"){
				tooltipText.textpanel->setText("Select All [ctrl-a]");
			}else if(buttonsB[i].panel->name=="editorButtonBSnap"){
				tooltipText.textpanel->setText("Snap [ctrl]");
			}else if(buttonsB[i].panel->name=="editorButtonBWireframe"){
				tooltipText.textpanel->setText("Wireframe [w]");
			}else if(buttonsB[i].panel->name=="editorButtonBSolo"){
				tooltipText.textpanel->setText("Solo Selected Object [o]");
			}else if(buttonsB[i].panel->name=="editorButtonBLocal Mode"){
				tooltipText.textpanel->setText("Local Mode [l]");
			}else if(buttonsB[i].panel->name=="editorButtonBGlobal Mode"){
				tooltipText.textpanel->setText("Global Mode [g]");
			}else if(buttonsB[i].panel->name=="editorButtonBShow Normals"){
				tooltipText.textpanel->setText("Show Normals [n]");
			}else if(buttonsB[i].panel->name=="editorButtonBHide Selected"){
				tooltipText.textpanel->setText("Hide Selected [h]");
			}else if(buttonsB[i].panel->name=="editorButtonBFree Camera"){
				tooltipText.textpanel->setText("Free Camera [1]");
			}else if(buttonsB[i].panel->name=="editorButtonBFree Orthographic Camera"){
				tooltipText.textpanel->setText("Free Orthographic Camera [2]");
			}else if(buttonsB[i].panel->name=="editorButtonBTop Camera"){
				tooltipText.textpanel->setText("Top Camera [3]");
			}else if(buttonsB[i].panel->name=="editorButtonBRight Camera"){
				tooltipText.textpanel->setText("Right Camera [5]");
			}else if(buttonsB[i].panel->name=="editorButtonBFront Camera"){
				tooltipText.textpanel->setText("Front Camera [4]");
			}else if(buttonsB[i].panel->name=="editorButtonBExpert Mode"){
				tooltipText.textpanel->setText("Expert Mode [ctrl-i]");
			}
		}else{
			buttonsBB[i].panel->color=FloatVector4d(0.4,0.4,0.4,1.0);
		}
	}
	
	for(int i=0; i<buttonsT.size(); i++){
	
		if(buttonsTB[i].panel->mouseover){
			buttonsTB[i].panel->color=FloatVector4d(0.6,0.6,0.6,1.0);

			tooltipBkg.panel->visible=true;
			tooltipText.textpanel->visible=true;

			if(buttonsT[i].panel->name=="editorButtonTSave"){
				tooltipText.textpanel->setText("Save [ctrl-s]");
			}else if(buttonsT[i].panel->name=="editorButtonTOpen"){
				tooltipText.textpanel->setText("Open [ctrl-o]");
			}else if(buttonsT[i].panel->name=="editorButtonTUndo"){
				tooltipText.textpanel->setText("Undo [ctrl-z]");
			}else if(buttonsT[i].panel->name=="editorButtonTRedo"){
				tooltipText.textpanel->setText("Redo [ctrl-y]");
			}else if(buttonsT[i].panel->name=="editorButtonTCopy"){
				tooltipText.textpanel->setText("Copy [ctrl-c]");
			}else if(buttonsT[i].panel->name=="editorButtonTCut"){
				tooltipText.textpanel->setText("Cut [ctrl-x]");
			}else if(buttonsT[i].panel->name=="editorButtonTPaste"){
				tooltipText.textpanel->setText("Paste [ctrl-p]");
			}else if(buttonsT[i].panel->name=="editorButtonTBuild Project"){
				tooltipText.textpanel->setText("Build Project [ctrl-b]");
			}else if(buttonsT[i].panel->name=="editorButtonTScript Editor"){
				tooltipText.textpanel->setText("Script Editor [ctrl-r]");
			}else if(buttonsT[i].panel->name=="editorButtonTProperties"){
				tooltipText.textpanel->setText("Properties [ctrl-d]");
			}else if(buttonsT[i].panel->name=="editorButtonTUV Editor"){
				tooltipText.textpanel->setText("UV Editor [ctrl-d]");
			}else if(buttonsT[i].panel->name=="editorButtonTExplorer"){
				tooltipText.textpanel->setText("Explorer [ctrl-e]");
			}else if(buttonsT[i].panel->name=="editorButtonTQuit"){
				tooltipText.textpanel->setText("Quit [escape]");
			}

		}else{
			buttonsTB[i].panel->color=FloatVector4d(0.4,0.4,0.4,1.0);
		}
	}
	
	if(tooltipBkg.panel->visible){
		

		tooltipBkg.panel->scale.x=tooltipText.textpanel->scale.x+(float(10));
		tooltipBkg.panel->scale.y=tooltipText.textpanel->scale.y+(float(10));

		if(mouse.x>float(conf->sizeX)*float(0.5)){
			tooltipBkg.panel->pos.x=mouse.x-(float(30))-tooltipBkg.panel->scale.x;
			tooltipText.textpanel->pos.x=mouse.x-(float(30))-tooltipText.textpanel->scale.x-(float(10));
		}else{
			tooltipBkg.panel->pos.x=mouse.x+(float(30));
			tooltipText.textpanel->pos.x=mouse.x+(float(35));
			
		}

		
		
		tooltipBkg.panel->pos.y=mouse.y-(float(30));
		tooltipText.textpanel->pos.y=mouse.y-(float(32));
		
		if(mouse.y<-conf->sizeY+75){
			tooltipBkg.panel->pos.y+=60;
			tooltipText.textpanel->pos.y+=60;
		}
	}
	
	if(mouse.lUp){
		editorButtonDownHack=false;
	}

	if(mouse.lDown && editorLeftBkg.panel->mouseover){
		editorButtonDownHack=true;
	}

	for(int i=0; i<buttonsL.size(); i++){
		if(buttonsLB[i].panel!=NULL){
			if(mouse.lDown && buttonsLB[i].panel->mouseover){
				editorButtonDownHack=true;
			}
		}
	}	

	for(int i=0; i<buttonsB.size(); i++){
		if(mouse.lDown && buttonsBB[i].panel->mouseover){
			editorButtonDownHack=true;
		}
	}	
	
	for(int i=0; i<buttonsT.size(); i++){
		if(mouse.lDown && buttonsTB[i].panel->mouseover){
			editorButtonDownHack=true;
		}
	}	
	
	for(int i=0; i<buttonsR.size(); i++){
		if(mouse.lDown && buttonsRB[i].panel->mouseover){
			editorButtonDownHack=true;
		}
	}	

	if(!editorButtonDownHack && toolActive("selectobjects","main")){
		doEditorPickingObjects();
	}

	if(!editorButtonDownHack && toolActive("selectpolygons","main")){
		doEditorPickingPolygons();
	}
	
	if(!editorButtonDownHack && toolActive("selectvertices","main")){
		doEditorPickingVertices();
	}
	
	if(!editorButtonDownHack && toolActive("selectedges","main")){
		doEditorPickingEdges();
	}

#ifdef LOOP_DEBUG
	logs().main.write("draw texture DISABLE");
#endif

	glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE5_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE7_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE8_ARB);
	glDisable(GL_TEXTURE_2D);

	glDisable(GL_POLYGON_OFFSET_FILL);

	glMatrixMode(GL_MODELVIEW);

	

	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(1,1);
	
	for(int s=0; s<selectedObjects.size(); s++){
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		shader.reset();

		glDisable(GL_FOG);

		glLineWidth(3);
		glColor4f(1,1,1,1);
		glDisable(GL_LIGHTING);

#ifndef SOFTWARE_TRANSFORMS
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glTranslatef(selectedObjects[s]->pos.x,selectedObjects[s]->pos.y,selectedObjects[s]->pos.z);
			
			
			glRotatef(selectedObjects[s]->rot.z,0.0f,0.0f,1.0f);
			glRotatef(selectedObjects[s]->rot.y,0.0f,1.0f,0.0f);
			glRotatef(selectedObjects[s]->rot.x,1.0f,0.0f,0.0f);

			glScalef(selectedObjects[s]->scale.x,selectedObjects[s]->scale.y,selectedObjects[s]->scale.z);
#endif
		
		glBegin(GL_LINES);

		for(int v=0; v<selectedObjects[s]->getVertexCount(); v+=3){
			if(toolActive("selectedges","main") && selectedEdges[s].find(v)==-1){
				glColor4f(1,0.8,0,1);
			}else if(selectedEdges[s].find(v)!=-1){
				glColor4f(1,0,0,1);
			}else if(toolActive("selectpolygons","main")){
				glColor4f(0,0.8,1,1);
			}else if(toolActive("selectvertices","main")){
				glColor4f(0.2,1,0.2,1);
			}else if(selectedEdges[s].size()>0){
				glColor4f(1,0.8,0,1);
			}else if(selectedPolygons[s].size()>0){
				glColor4f(0,0.8,1,1);
			}else if(selectedVertices[s].size()>0){
				glColor4f(0.2,1,0.2,1);
			}else{
				glColor4f(1,1,1,1);
			}

			glVertex3f(selectedObjects[s]->getVertexRaw(v).pos.x,selectedObjects[s]->getVertexRaw(v).pos.y,selectedObjects[s]->getVertexRaw(v).pos.z);
			glVertex3f(selectedObjects[s]->getVertexRaw(v+1).pos.x,selectedObjects[s]->getVertexRaw(v+1).pos.y,selectedObjects[s]->getVertexRaw(v+1).pos.z);

			if(toolActive("selectedges","main") && selectedEdges[s].find(v+1)==-1){
				glColor4f(1,0.8,0,1);
			}else if(selectedEdges[s].find(v+1)!=-1){
				glColor4f(1,0,0,1);
			}else if(toolActive("selectpolygons","main")){
				glColor4f(0,0.8,1,1);
			}else if(toolActive("selectvertices","main")){
				glColor4f(0.2,1,0.2,1);
			}else if(selectedEdges[s].size()>0){
				glColor4f(1,0.8,0,1);
			}else if(selectedPolygons[s].size()>0){
				glColor4f(0,0.8,1,1);
			}else if(selectedVertices[s].size()>0){
				glColor4f(0.2,1,0.2,1);
			}else{
				glColor4f(1,1,1,1);
			}

			glVertex3f(selectedObjects[s]->getVertexRaw(v+1).pos.x,selectedObjects[s]->getVertexRaw(v+1).pos.y,selectedObjects[s]->getVertexRaw(v+1).pos.z);
			glVertex3f(selectedObjects[s]->getVertexRaw(v+2).pos.x,selectedObjects[s]->getVertexRaw(v+2).pos.y,selectedObjects[s]->getVertexRaw(v+2).pos.z);

			if(toolActive("selectedges","main") && selectedEdges[s].find(v+2)==-1){
				glColor4f(1,0.8,0,1);
			}else if(selectedEdges[s].find(v+2)!=-1){
				glColor4f(1,0,0,1);
			}else if(toolActive("selectpolygons","main")){
				glColor4f(0,0.8,1,1);
			}else if(toolActive("selectvertices","main")){
				glColor4f(0.2,1,0.2,1);
			}else if(selectedEdges[s].size()>0){
				glColor4f(1,0.8,0,1);
			}else if(selectedPolygons[s].size()>0){
				glColor4f(0,0.8,1,1);
			}else if(selectedVertices[s].size()>0){
				glColor4f(0.2,1,0.2,1);
			}else{
				glColor4f(1,1,1,1);
			}

			glVertex3f(selectedObjects[s]->getVertexRaw(v+2).pos.x,selectedObjects[s]->getVertexRaw(v+2).pos.y,selectedObjects[s]->getVertexRaw(v+2).pos.z);
			glVertex3f(selectedObjects[s]->getVertexRaw(v).pos.x,selectedObjects[s]->getVertexRaw(v).pos.y,selectedObjects[s]->getVertexRaw(v).pos.z);
		}

		glEnd();

		if(toolActive("selectvertices","main") || selectedVertices[s].size()>0){
			glPointSize(10);
			glColor4f(0,0,1,1);
			glBegin(GL_POINTS);

			for(int v=0; v<selectedObjects[s]->getVertexCount(); v++){

				if(selectedVertices[s].find(v)!=-1){
					glColor4f(1,0,0,1);
				}else{
					glColor4f(0,0,1,1);
				}

				glVertex3f(selectedObjects[s]->getVertexRaw(v).pos.x,selectedObjects[s]->getVertexRaw(v).pos.y,selectedObjects[s]->getVertexRaw(v).pos.z);
			}

			glEnd();
			glPointSize(1);
		}

		#ifndef SOFTWARE_TRANSFORMS
			glPopMatrix();
		#endif

		if(toolActive("translate","main")){

			FloatVector4d arrowPos=FloatVector4d(selectedObjects[s]->pos.x,selectedObjects[s]->pos.y,selectedObjects[s]->pos.z,1);
			float fnd=0;
			float efnd=0;
			float vfnd=0;

			for(int i=0; i<selectedObjects.size(); i++){
				for(int p=0; p<selectedPolygons[i].size(); p++){

					Vertex* v1=&selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3);
					Vertex* v2=&selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1);
					Vertex* v3=&selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2);

					if(fnd==0){
						arrowPos.x=(v1->pos.x+v2->pos.x+v3->pos.x);
						arrowPos.y=(v1->pos.y+v2->pos.y+v3->pos.y);
						arrowPos.z=(v1->pos.z+v2->pos.z+v3->pos.z);
						fnd+=3;
					}else{
						arrowPos.x+=(v1->pos.x+v2->pos.x+v3->pos.x);
						arrowPos.y+=(v1->pos.y+v2->pos.y+v3->pos.y);
						arrowPos.z+=(v1->pos.z+v2->pos.z+v3->pos.z);
						fnd+=3;
					}
				}

				for(int p=0; p<selectedEdges[i].size(); p++){

					Vertex* v1;
					Vertex* v2;
					
					int e=selectedEdges[i][p];

					if(e%3 == 0){
						v1=&selectedObjects[i]->getVertexRaw(e);
						v2=&selectedObjects[i]->getVertexRaw(e+1);
					}else if(e%3 == 1){
						v1=&selectedObjects[i]->getVertexRaw(e);
						v2=&selectedObjects[i]->getVertexRaw(e+1);
					}else if(e%3 == 2){
						v1=&selectedObjects[i]->getVertexRaw(e);
						v2=&selectedObjects[i]->getVertexRaw(e-2);
					}

					if(efnd==0){
						arrowPos.x=(v1->pos.x+v2->pos.x);
						arrowPos.y=(v1->pos.y+v2->pos.y);
						arrowPos.z=(v1->pos.z+v2->pos.z);
						efnd+=2;
					}else{
						arrowPos.x+=(v1->pos.x+v2->pos.x);
						arrowPos.y+=(v1->pos.y+v2->pos.y);
						arrowPos.z+=(v1->pos.z+v2->pos.z);
						efnd+=2;
					}
				}

				for(int p=0; p<selectedVertices[i].size(); p++){
					
					int e=selectedVertices[i][p];

					if(vfnd==0){
						arrowPos.x=selectedObjects[i]->getVertexRaw(e).pos.x;
						arrowPos.y=selectedObjects[i]->getVertexRaw(e).pos.y;
						arrowPos.z=selectedObjects[i]->getVertexRaw(e).pos.z;
						vfnd+=1;
					}else{
						arrowPos.x+=selectedObjects[i]->getVertexRaw(e).pos.x;
						arrowPos.y+=selectedObjects[i]->getVertexRaw(e).pos.y;
						arrowPos.z+=selectedObjects[i]->getVertexRaw(e).pos.z;
						vfnd+=1;
					}
				}

				if(fnd>0){arrowPos/=fnd;}
				if(efnd>0){arrowPos/=efnd;}
				if(vfnd>0){arrowPos/=vfnd;}

				if(fnd>0 || efnd>0 || vfnd>0){
					arrowPos.w=1;

					Matrix4d4d m;

					m.loadIdentity();
					m.translate(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);
					m.rotateZ(selectedObjects[i]->rot.z);
					m.rotateY(selectedObjects[i]->rot.y);
					m.rotateX(selectedObjects[i]->rot.x);
					m.scale(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);

					arrowPos=m*arrowPos;
				}

				drawArrowTranslate(FloatVector3d(arrowPos.x,arrowPos.y,arrowPos.z));
			}

			
		}else if(toolActive("scale","main")){
			drawArrowScale(selectedObjects[s]->pos);
		}else if(toolActive("rotate","main")){
			drawArrowRotate(selectedObjects[s]->pos);
		}

		
	}

	if(toolActive("translate","main")){
		for(int s=0; s<selectedLights.size(); s++){
			drawArrowTranslate(selectedLights[s]->pos);
		}
	}

	if(toolActive("rotate","main")){
		for(int s=0; s<selectedLights.size(); s++){
			if(selectedLights[s]->type==LIGHT_SPOT){
				drawArrowRotate(selectedLights[s]->pos);
			}
		}
	}

	drawArrowCamera();

	if(selectedPolygons.size()>0){
		glPolygonOffset(0,-1);
		glEnable(GL_POLYGON_STIPPLE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonStipple(mask);

		glColor4f(1,0,0,1);

		for(int s=0; s<selectedObjects.size(); s++){
			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				glTranslatef(selectedObjects[s]->pos.x,selectedObjects[s]->pos.y,selectedObjects[s]->pos.z);
				
				
				glRotatef(selectedObjects[s]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(selectedObjects[s]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(selectedObjects[s]->rot.x,1.0f,0.0f,0.0f);

				glScalef(selectedObjects[s]->scale.x,selectedObjects[s]->scale.y,selectedObjects[s]->scale.z);
			#endif

			glBegin(GL_TRIANGLES);

			for(int i=0; i<selectedPolygons[s].size(); i++){
				Vertex* v1=&selectedObjects[s]->getVertexRaw(selectedPolygons[s][i]*3);
				Vertex* v2=&selectedObjects[s]->getVertexRaw(selectedPolygons[s][i]*3+1);
				Vertex* v3=&selectedObjects[s]->getVertexRaw(selectedPolygons[s][i]*3+2);

				glVertex3f(v1->pos.x,v1->pos.y,v1->pos.z);
				glVertex3f(v2->pos.x,v2->pos.y,v2->pos.z);
				glVertex3f(v3->pos.x,v3->pos.y,v3->pos.z);
			}

			glEnd();

			#ifndef SOFTWARE_TRANSFORMS
				glPopMatrix();
			#endif
		}

		glDisable(GL_POLYGON_STIPPLE);
	}
	glPolygonOffset(0,0);
	glDisable(GL_POLYGON_OFFSET_LINE);

	for(int l=0; l<level->lights.size(); l++){

		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		shader.reset();

		glDisable(GL_FOG);
		
	
		glLineWidth(3);
		glColor4f(level->lights[l]->color.x,level->lights[l]->color.y,level->lights[l]->color.z,1);
		glDisable(GL_LIGHTING);


		if(level->lights[l]->type==LIGHT_SPOT){

			
			gluQuadricDrawStyle(quad,GLU_LINE);


			float ang=level->lights[l]->coneAngle;
			if(ang>179.5){ang=179.5;}

			float r=math.tanDegrees(ang/2)*level->lights[l]->falloffEnd;

			glTranslatef(level->lights[l]->pos.x,level->lights[l]->pos.y,level->lights[l]->pos.z);

			gluQuadricDrawStyle(quad,GLU_FILL);
     		gluSphere(quad,1,20,20);	//r,slices,stacks

			glTranslatef(-level->lights[l]->pos.x,-level->lights[l]->pos.y,-level->lights[l]->pos.z);

			if(selectedLights.find(level->lights[l])!=-1){

				glTranslatef(level->lights[l]->pos.x,level->lights[l]->pos.y,level->lights[l]->pos.z);

				glRotatef((level->lights[l]->rot.y),0.0f,1.0f,0.0f);
				glRotatef((level->lights[l]->rot.x-180),1.0f,0.0f,0.0f);
				gluQuadricDrawStyle(quad,GLU_LINE);
				gluCylinder(quad,0,r,level->lights[l]->falloffEnd,11,11);
				glRotatef(-(level->lights[l]->rot.x-180),1.0f,0.0f,0.0f);
				glRotatef(-(level->lights[l]->rot.y),0.0f,1.0f,0.0f);

				glTranslatef(-level->lights[l]->pos.x,-level->lights[l]->pos.y,-level->lights[l]->pos.z);

				FloatVector3d ds=math.degreesToNormal(level->lights[l]->rot)*float(5);

				glColor4f(1,0,0,1);

				glTranslatef(level->lights[l]->pos.x+ds.x,level->lights[l]->pos.y+ds.y,level->lights[l]->pos.z+ds.z);
				gluQuadricDrawStyle(quad,GLU_FILL);
				gluSphere(quad,1,11,11);
				glTranslatef(-(level->lights[l]->pos.x+ds.x),-(level->lights[l]->pos.y+ds.y),-(level->lights[l]->pos.z+ds.z));
			}

		}else{

			glTranslatef(level->lights[l]->pos.x,level->lights[l]->pos.y,level->lights[l]->pos.z);

			gluQuadricDrawStyle(quad,GLU_FILL);
     		gluSphere(quad,1,20,20);	//r,slices,stacks

			if(selectedLights.find(level->lights[l])!=-1){
     			gluQuadricDrawStyle(quad,GLU_LINE);
				gluSphere(quad,level->lights[l]->falloffEnd,11,11);
     		}

			glTranslatef(-level->lights[l]->pos.x,-level->lights[l]->pos.y,-level->lights[l]->pos.z);
		}

		
		

		
	}

	for(int l=0; l<audio.sources.size(); l++){
		glDisable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		shader.reset();
		glDisable(GL_FOG);
		glLineWidth(3);
		glColor4f(1,0,0,1);
		glDisable(GL_LIGHTING);

		glTranslatef(audio.sources[l]->pos.x,audio.sources[l]->pos.y,audio.sources[l]->pos.z);

		gluQuadricDrawStyle(quad,GLU_FILL);
 		gluSphere(quad,1,20,20);	//r,slices,stacks
		
		gluQuadricDrawStyle(quad,GLU_LINE);
		gluSphere(quad,audio.sources[l]->range,11,11);
		glTranslatef(-audio.sources[l]->pos.x,-audio.sources[l]->pos.y,-audio.sources[l]->pos.z);
	}
}

#endif

#endif
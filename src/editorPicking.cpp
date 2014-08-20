#ifdef DEVELOPER

#include "editor.h"
#include "level.hpp"
#include "HelperLibMath.hpp"
#include "keyboard.h"
#include "mouse.h"
#include "texturelibrary.h"
#include "conf.h"
#include "lightmap.h"
#include "Audio.h"
#include "texture.hpp"
#include "draw.hpp"
#include "timeing.hpp"
#include "picking.h"


void Editor::buildEditorList(){

	Array<Object*> a;
	a.pushBack();

	for(int i=0; i<level->objects.size(); i++){

		if(!level->objects[i]->listBuiltQuick || level->objects[i]->dirty){

	
			glDeleteLists(level->objects[i]->glListNameQuick,1);

			level->objects[i]->glListNameQuick=glGenLists(1);
			glNewList(level->objects[i]->glListNameQuick,GL_COMPILE);

			level->objects[i]->draw();
			glEndList();	

			level->objects[i]->listBuiltQuick=true;
		}

		if(!level->objects[i]->listBuiltQuickPolygon || level->objects[i]->dirty){
			glDeleteLists(level->objects[i]->glListNameQuickPolygon,1);

			level->objects[i]->glListNameQuickPolygon=glGenLists(1);
			glNewList(level->objects[i]->glListNameQuickPolygon,GL_COMPILE);

			level->objects[i]->drawPolygon();
			glEndList();	

			level->objects[i]->listBuiltQuickPolygon=true;
		}

		if(!level->objects[i]->listBuiltQuickEdge || level->objects[i]->dirty){
			glDeleteLists(level->objects[i]->glListNameQuickEdge,1);

			level->objects[i]->glListNameQuickEdge=glGenLists(1);
			glNewList(level->objects[i]->glListNameQuickEdge,GL_COMPILE);

			level->objects[i]->drawEdge();
			glEndList();	

			level->objects[i]->listBuiltQuickEdge=true;
		}

		if(!level->objects[i]->listBuiltQuickVertex || level->objects[i]->dirty){
			glDeleteLists(level->objects[i]->glListNameQuickVertex,1);

			level->objects[i]->glListNameQuickVertex=glGenLists(1);
			glNewList(level->objects[i]->glListNameQuickVertex,GL_COMPILE);

			level->objects[i]->drawVertex();
			glEndList();	

			level->objects[i]->listBuiltQuickVertex=true;
		}
	}
}


bool intersectRayTriangle(FloatVector3d* o, FloatVector3d* d, FloatVector3d* v1, FloatVector3d* v2, FloatVector3d* v3){
	FloatVector3d p;
	return false;
}

void Editor::doEditorPickingObjects(){

	if(!mouse.lDown){return;}

	if(level->objects.size()<1 && level->lights.size()<1){
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

	float mx=mouse.x;
	float my=conf->sizeY+mouse.y;

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

	Array<Object*>* stack=&level->objects;

	for(int i=0; i<(*stack).size(); i++){
		if((*stack)[i]->visible){

			if((*stack)[i]->box.intersect(o,d)){

				finList.pushBack(i);
			}
			
		}
		
	}

	int eid=-1;
	float edist=-1;

	Matrix4d4d m;

	for(int i=0; i<finList.size(); i++){
		int ind=finList[i];

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

			float mx=mouse.x;
			float my=conf->sizeY+mouse.y;

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
					if(math.dotProduct(d,(*stack)[ind]->getVertexRaw(v).normal)<0){
						eid=ind;
						edist=dist;
					}
				}
			}
		}
	}

	float a,b,c;
	FloatVector3d t;

	for(int i=0; i<level->lights.size(); i++){
		if(level->lights[i]->visible){
			glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

			float mx=mouse.x;
			float my=conf->sizeY+mouse.y;

			int r=gluUnProject(mx,my,0.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			o.x=x;o.y=y;o.z=z;
			
			r=gluUnProject(mx,my,1.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			d.x=x;d.y=y;d.z=z;
			
			d=d-o;

			a=math.dotProduct(d,d);

			t=o-level->lights[i]->pos;
			
			b=math.dotProduct(float(2)*(t) , d);
			c=math.dotProduct(t,t)-2;

			if(4*a*c<b*b){
				eid=i+level->objects.size();
			}


			
		}
	
		
	}

	if(eid!=-1){
		int objsel=eid;

		if(objsel>=(*stack).size()){

			int lightid=objsel-(*stack).size();

			if(keyboard.shift){
				if(selectedLights.find(level->lights[lightid])==-1){
					selectedLights.pushBack(level->lights[lightid]);
				}
			}else if(keyboard.ctrl){
				int oi=0;
				oi=selectedLights.find(level->lights[lightid]);

				if(oi==-1){
					selectedLights.pushBack(level->lights[lightid]);
				}else{
					selectedLights.erase(oi);
				}

			}else{
				selectedObjects.clear();
				selectedLights.clear();
				
				selectedLights.pushBack(level->lights[lightid]);
			}
		}else if(objsel<(*stack).size()){
			
			while(selectedPolygons.size()<=selectedObjects.size()+1){
				selectedPolygons.pushBack();
			}

			while(selectedEdges.size()<=selectedObjects.size()+1){
				selectedEdges.pushBack();
			}

			while(selectedVertices.size()<=selectedObjects.size()+1){
				selectedVertices.pushBack();
			}

			if(keyboard.shift){
				if(selectedObjects.find((*stack)[objsel])==-1){
					selectedObjects.pushBack((*stack)[objsel]);
				}
			}else if(keyboard.ctrl){
				int oi=0;
				oi=selectedObjects.find((*stack)[objsel]);

				if(oi==-1){
					selectedObjects.pushBack((*stack)[objsel]);
				}else{
					selectedObjects.erase(oi);
				}

			}else{
				selectedObjects.clear();
				selectedLights.clear();
				
				selectedObjects.pushBack((*stack)[objsel]);
			}
		

		}
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}


void Editor::doEditorPickingPolygons(){

	if(!mouse.l){return;}

	if(selectedObjects.size()==0){
		return;
	}

	int oid=-1;
	int eid=-1;
	float edist=-1;
	Matrix4d4d m;

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

	for(int i=0; i<selectedObjects.size(); i++){

		for(int v=0; v<selectedObjects[i]->getVertexCount(); v+=3){

			m.loadIdentity();
			m.translate(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);
			m.rotateZ(selectedObjects[i]->rot.z);
			m.rotateY(selectedObjects[i]->rot.y);
			m.rotateX(selectedObjects[i]->rot.x);
			m.scale(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				glTranslatef(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);
				
				
				glRotatef(selectedObjects[i]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(selectedObjects[i]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(selectedObjects[i]->rot.x,1.0f,0.0f,0.0f);

				glScalef(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);
			#endif

			glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			#endif

			float mx=mouse.x;
			float my=conf->sizeY+mouse.y;

			int r=gluUnProject(mx,my,0.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			o.x=x;o.y=y;o.z=z;
			
			r=gluUnProject(mx,my,1.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			d.x=x;d.y=y;d.z=z;

			d=d-o;
			d.normalize();

			float dist=0;
			float b1=0;
			float b2=0;

			if(math.intersectRayTriangle(&dist,&b1,&b2, &o,&d,&selectedObjects[i]->getVertexRaw(v).pos,&selectedObjects[i]->getVertexRaw(v+1).pos,&selectedObjects[i]->getVertexRaw(v+2).pos)){

				FloatVector4d vp=FloatVector4d(selectedObjects[i]->getVertexRaw(v).pos.x,selectedObjects[i]->getVertexRaw(v).pos.y,selectedObjects[i]->getVertexRaw(v).pos.z,1);
				FloatVector4d rdist=(m*vp)*b1;

				vp=FloatVector4d(selectedObjects[i]->getVertexRaw(v+1).pos.x,selectedObjects[i]->getVertexRaw(v+1).pos.y,selectedObjects[i]->getVertexRaw(v+1).pos.z,1);

				rdist+=(m*vp)*b2;

				vp=FloatVector4d(selectedObjects[i]->getVertexRaw(v+2).pos.x,selectedObjects[i]->getVertexRaw(v+2).pos.y,selectedObjects[i]->getVertexRaw(v+2).pos.z,1);

				rdist+=(m*vp)*(1-b1-b2);
				rdist-=level->camera->pos;
				dist=rdist.magnitude();

				if( (dist>0 && dist<edist) || (dist>0 && edist<0)){

					if(math.dotProduct(d,selectedObjects[i]->getVertexRaw(v).normal)<0){
						oid=i;
						eid=v/3;
						edist=dist;
					}
				}
			}
		}
	}

	if(eid!=-1){

		int objsel=oid;


		if(objsel!=-1){

			if(keyboard.shift){

				while(selectedPolygons.size()<=objsel){
					selectedPolygons.pushBack();
				}

				while(selectedEdges.size()<=objsel){
					selectedEdges.pushBack();
				}

				while(selectedVertices.size()<=objsel){
					selectedVertices.pushBack();
				}

				if(selectedPolygons[objsel].find(eid)==-1){
					selectedPolygons[objsel].pushBack(eid);
				}
			}else if(keyboard.ctrl){

				while(selectedPolygons.size()<=objsel){
					selectedPolygons.pushBack();
				}

				while(selectedEdges.size()<=objsel){
					selectedEdges.pushBack();
				}

				while(selectedVertices.size()<=objsel){
					selectedVertices.pushBack();
				}

			}else{

				while(selectedPolygons.size()<=objsel){
					selectedPolygons.pushBack();
				}

				while(selectedEdges.size()<=objsel){
					selectedEdges.pushBack();
				}

				while(selectedVertices.size()<=objsel){
					selectedVertices.pushBack();
				}

				if(mouse.lDown){
					for(int b=0; b<selectedPolygons.size(); b++){
						selectedPolygons[b].clear();
					}
				}

				if(selectedPolygons[objsel].find(eid)==-1){
					selectedPolygons[objsel].pushBack(eid);
				}
			}
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void Editor::addAllEdges(int oid, int eid){

	FloatVector3d p1,p2;

	if( (eid-2) % 3 == 0){
		p1=selectedObjects[oid]->getVertexRaw(eid).pos;
		p2=selectedObjects[oid]->getVertexRaw(eid-2).pos;
	}else{
		p1=selectedObjects[oid]->getVertexRaw(eid).pos;
		p2=selectedObjects[oid]->getVertexRaw(eid+1).pos;
	}

	for(int i=0; i<selectedObjects[oid]->getVertexCount(); i+=3){
		if( (selectedObjects[oid]->getVertexRaw(i).pos==p1 && selectedObjects[oid]->getVertexRaw(i+1).pos==p2) ||
			(selectedObjects[oid]->getVertexRaw(i).pos==p2 && selectedObjects[oid]->getVertexRaw(i+1).pos==p1) ){

			if(selectedEdges[oid].find(i)==-1){selectedEdges[oid].pushBack(i);}
		}

		if( (selectedObjects[oid]->getVertexRaw(i+1).pos==p1 && selectedObjects[oid]->getVertexRaw(i+2).pos==p2) ||
			(selectedObjects[oid]->getVertexRaw(i+1).pos==p2 && selectedObjects[oid]->getVertexRaw(i+2).pos==p1) ){

			if(selectedEdges[oid].find(i+1)==-1){selectedEdges[oid].pushBack(i+1);}
		}

		if( (selectedObjects[oid]->getVertexRaw(i+2).pos==p1 && selectedObjects[oid]->getVertexRaw(i).pos==p2) ||
			(selectedObjects[oid]->getVertexRaw(i+2).pos==p2 && selectedObjects[oid]->getVertexRaw(i).pos==p1) ){

			if(selectedEdges[oid].find(i+2)==-1){selectedEdges[oid].pushBack(i+2);}
		}
	}

}


void Editor::addAllVertices(int oid, int eid){
	for(int i=0; i<selectedObjects[oid]->getVertexCount(); i++){
		if( selectedObjects[oid]->getVertexRaw(i).pos==selectedObjects[oid]->getVertexRaw(eid).pos ){
			if(selectedVertices[oid].find(i)==-1){selectedVertices[oid].pushBack(i);}
		}
	}
}

//http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/

#define EPS 0.00001f

/*
   Calculate the line segment PaPb that is the shortest route between
   two lines P1P2 and P3P4. Calculate also the values of mua and mub where
      Pa = P1 + mua (P2 - P1)
      Pb = P3 + mub (P4 - P3)
   Return FALSE if no solution exists.
*/
int lineLineIntersect(
   FloatVector3d p1,FloatVector3d p2,FloatVector3d p3,FloatVector3d p4,FloatVector3d *pa,FloatVector3d *pb,
   double *mua, double *mub)
{
   FloatVector3d p13,p43,p21;
   double d1343,d4321,d1321,d4343,d2121;
   double numer,denom;

   p13.x = p1.x - p3.x;
   p13.y = p1.y - p3.y;
   p13.z = p1.z - p3.z;
   p43.x = p4.x - p3.x;
   p43.y = p4.y - p3.y;
   p43.z = p4.z - p3.z;
   
   if (math.abs(p43.x)  < EPS && math.abs(p43.y)  < EPS && math.abs(p43.z)  < EPS){
      return(FALSE);
   }

   p21.x = p2.x - p1.x;
   p21.y = p2.y - p1.y;
   p21.z = p2.z - p1.z;
   if (math.abs(p21.x)  < EPS && math.abs(p21.y)  < EPS && math.abs(p21.z)  < EPS)
      return(FALSE);

   d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
   d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
   d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
   d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
   d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

   denom = d2121 * d4343 - d4321 * d4321;
   if (math.abs(denom) < EPS)
      return(FALSE);
   numer = d1343 * d4321 - d1321 * d4343;

   *mua = numer / denom;
   *mub = (d1343 + d4321 * (*mua)) / d4343;

   pa->x = p1.x + *mua * p21.x;
   pa->y = p1.y + *mua * p21.y;
   pa->z = p1.z + *mua * p21.z;
   pb->x = p3.x + *mub * p43.x;
   pb->y = p3.y + *mub * p43.y;
   pb->z = p3.z + *mub * p43.z;

   return(TRUE);
}

void Editor::doEditorPickingEdges(){
	if(!mouse.l){return;}

	if(level->objects.size()<1){
		return;
	}

	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	level->camera->projectionMatrix();
	//level->camera->adjust();

	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	level->camera->transform();

	level->camera->viewport();

	glGetIntegerv(GL_VIEWPORT,viewport);

	FloatVector3d o,d,ta,tb,ca,cb;
	GLdouble x,y,z,ba,bb;

	for(int i=0; i<selectedObjects.size(); i++){
		if(selectedObjects[i]->visible){

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				glTranslatef(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);
				
				
				glRotatef(selectedObjects[i]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(selectedObjects[i]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(selectedObjects[i]->rot.x,1.0f,0.0f,0.0f);

				glScalef(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);
			#endif

			glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			#endif

			float mx=mouse.x;
			float my=conf->sizeY+mouse.y;

			int r=gluUnProject(mx,my,0.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			o.x=x;o.y=y;o.z=z;
			
			r=gluUnProject(mx,my,1.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			d.x=x;d.y=y;d.z=z;

			int eid=-1;
			float edist=-1;


			for(int v=0; v<selectedObjects[i]->getVertexCount(); v+=3){
				
				
				for(int f=0; f<3; f++){

					if(f==0){
						ca=selectedObjects[i]->getVertexRaw(v).pos;
						cb=selectedObjects[i]->getVertexRaw(v+1).pos;
					}else if(f==1){
						ca=selectedObjects[i]->getVertexRaw(v+1).pos;
						cb=selectedObjects[i]->getVertexRaw(v+2).pos;
					}else if(f==2){
						ca=selectedObjects[i]->getVertexRaw(v+2).pos;
						cb=selectedObjects[i]->getVertexRaw(v).pos;
					}

					int suc=lineLineIntersect(o,d,ca,cb,&ta,&tb,&ba,&bb);

					if(suc && (ta-tb).magnitude()<0.1f && ba>0 && ba<1 && bb>0 && bb<1){
						
						if(edist<0 || ba < edist){
							edist=ba;
							eid=f+v;
						}

						
					}
				}
			}

			if(eid!=-1){
				int objsel=i;

				if(keyboard.shift){

					while(selectedEdges.size()<=objsel){
						selectedEdges.pushBack();
					}

					if(selectedEdges[objsel].find(eid)==-1){
						addAllEdges(objsel,eid);
					}
				}else if(keyboard.ctrl){

					while(selectedEdges.size()<=objsel){
						selectedEdges.pushBack();
					}

				}else{

					while(selectedEdges.size()<=objsel){
						selectedEdges.pushBack();
					}

					if(mouse.lDown){
						for(int b=0; b<selectedEdges.size(); b++){
							selectedEdges[b].clear();
						}
					}

					if(selectedEdges[objsel].find(eid)==-1){
						addAllEdges(objsel,eid);
					}
				}
			}
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}	


void Editor::doEditorPickingVertices(){
	if(!mouse.l){return;}

	if(level->objects.size()<1){
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

	FloatVector3d o,d,t;
	GLdouble x,y,z;

	float a,b,c;

	for(int i=0; i<selectedObjects.size(); i++){
		if(selectedObjects[i]->visible){

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				glTranslatef(selectedObjects[i]->pos.x,selectedObjects[i]->pos.y,selectedObjects[i]->pos.z);
				
				
				glRotatef(selectedObjects[i]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(selectedObjects[i]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(selectedObjects[i]->rot.x,1.0f,0.0f,0.0f);

				glScalef(selectedObjects[i]->scale.x,selectedObjects[i]->scale.y,selectedObjects[i]->scale.z);
			#endif

			glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPopMatrix();
			#endif

			float mx=mouse.x;
			float my=conf->sizeY+mouse.y;

			int r=gluUnProject(mx,my,0.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			o.x=x;o.y=y;o.z=z;
			
			r=gluUnProject(mx,my,1.0f,modelMatrix,projMatrix,viewport,&x,&y,&z);
			d.x=x;d.y=y;d.z=z;

			d=d-o;

			DoubleVector3d w;
			gluProject(o.x,o.y,o.z,modelMatrix,projMatrix,viewport,&w.x,&w.y,&w.z);


			for(int v=0; v<selectedObjects[i]->getVertexCount(); v++){
				a=math.dotProduct(d,d);

				t=o-selectedObjects[i]->getVertexRaw(v).pos;
				b=math.dotProduct(float(2)*(t) , d);
				c=math.dotProduct(t,t)-0.1f;

				if(4*a*c<b*b){
					int objsel=i;


					if(objsel!=-1){

						if(keyboard.shift){

							while(selectedVertices.size()<=objsel){
								selectedVertices.pushBack();
							}

							if(selectedVertices[objsel].find(v)==-1){
								addAllVertices(objsel,v);
							}
						}else if(keyboard.ctrl){

							while(selectedVertices.size()<=objsel){
								selectedVertices.pushBack();
							}

						}else{

							while(selectedVertices.size()<=objsel){
								selectedVertices.pushBack();
							}

							if(mouse.lDown){
								for(int b=0; b<selectedVertices.size(); b++){
									selectedVertices[b].clear();
								}
							}

							if(selectedVertices[objsel].find(v)==-1){
								addAllVertices(objsel,v);
							}
						}
					}
				}
			}
		}
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

#endif //DEVELOPER

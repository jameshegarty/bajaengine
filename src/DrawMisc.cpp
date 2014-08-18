#include "GlHeader.hpp"
										
#include "draw.hpp"
#include "level.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mouse.h"
#include "picking.h"
#include "hardware.hpp"
#include "random.h"
#include "SceneGraph.hpp"
#include "conf.h"

#include "Log.hpp"
#include "sort.hpp"
#include "HelperLibMath.hpp"

#include "lightmap.h"

#ifdef _WIN32
	#include <math.h>
#endif

#include "picker_matrix.h"

void drawNormals(){

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	shader.reset();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	float amb[4];
	amb[0]=1;
	amb[1]=1;
	amb[2]=1;
	amb[3]=1;

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,amb);
	glMaterialfv(GL_FRONT,GL_SPECULAR,amb);
	glMaterialfv(GL_FRONT,GL_AMBIENT,amb);


	for(int i=0; i<level->objects.size(); i++){
		level->objects[i]->drawNormals();
	}
}

void drawImplicitCallback(ObjectAddress obj){
	if(obj.type==NULL3D){
		obj.null->drawRaw();
	}

	if(obj.type==CURVE){

		obj.curve->drawRaw();
	}
}

void drawImplicit(){
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	shader.reset();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor4f(0,float(0.4),0,1);

	float amb[4];
	amb[0]=1;
	amb[1]=1;
	amb[2]=1;
	amb[3]=1;

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,amb);
	glMaterialfv(GL_FRONT,GL_SPECULAR,amb);
	glMaterialfv(GL_FRONT,GL_AMBIENT,amb);


	for(int i=0; i<level->nulls.size(); i++){
		level->nulls[i]->draw();
	}

	for(int i=0; i<level->curves.size(); i++){
		level->curves[i]->draw();
	}

	level->sceneGraph.enableDrawList();
	level->sceneGraph.draw(drawImplicitCallback);
}

void drawBB(){

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	shader.reset();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	float amb[4];
	amb[0]=1;
	amb[1]=1;
	amb[2]=1;
	amb[3]=1;

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,amb);
	glMaterialfv(GL_FRONT,GL_SPECULAR,amb);
	glMaterialfv(GL_FRONT,GL_AMBIENT,amb);

	FloatVector3d dist3;
	float dist=0;

	for(int i=0; i<level->objects.size(); i++){
		dist3=level->objects[i]->box.center()-level->camera->pos;
		dist=math.dotProduct(dist3,dist3);
		dist=math.sqrt(dist);

		if(conf->drawbbDistance==0 || dist<conf->drawbbDistance || level->objects[i]->name==conf->drawbbSolo){
			if(conf->drawbbSolo!="" && level->objects[i]->name!=conf->drawbbSolo){

			}else{
				level->objects[i]->drawBB();
			}
		}
	}
}

void drawMouse(){

	if(!mouse.visible){
		return;
	}

	if(!mouse.virtualMouse){
		return;
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);

	glDisable(GL_CULL_FACE);

	

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	shader.reset();


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

	if(mouse.textureId!=-1){

		glColor4f(mouse.color.x,mouse.color.y,mouse.color.z,mouse.color.w);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,mouse.textureId);
	}else{
		glDisable(GL_TEXTURE_2D);
		glColor4f(mouse.color.x,mouse.color.y,mouse.color.z,mouse.color.w);
	}

	float sx=mouse.textureScale.x;
	float sy=mouse.textureScale.y;
	
	float px=mouse.x;
	float py=mouse.y;
	float pz=0;

	glBegin(GL_QUADS);
		glTexCoord2d(0,texture.info(mouse.textureId).offsetTR.y); glVertex3f(px		,py,pz);
		glTexCoord2d(1,texture.info(mouse.textureId).offsetTR.y); glVertex3f(px+sx	,py,pz);
		glTexCoord2d(1,-1); glVertex3f(px+sx	,py-sy,pz);
		glTexCoord2d(0,-1); glVertex3f(px		,py-sy,pz);
	glEnd();
	
}

void drawWidescreenPanel(float fade=0){

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawWidescreenPanel");
#endif

	if(!conf->widescreen){
		return;
	}

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();

	glOrtho(0,conf->sizeX,-conf->sizeY,0,-100,100);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix

	glLoadIdentity ();	
	

	glDisable(GL_BLEND);

	glDisable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);

	glDisable(GL_CULL_FACE);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	shader.reset();


	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
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

	glDisable(GL_TEXTURE_2D);
	glColor4f(0,0,0,0);
	

	FloatVector2d b1pos;
	FloatVector2d b1scale;

	FloatVector2d b2pos;
	FloatVector2d b2scale;

	float py=conf->sizeY-(conf->sizeX*(1/conf->widescreenRatio));
	py=py/(float)2;

	float sy=conf->sizeX*(1/conf->widescreenRatio);
	float sx=conf->sizeX;

	float px=0;

	b1pos.x=px;
	b1pos.y=0;

	b1scale.x=sx;
	
	b1scale.y=(float((conf->sizeY)-sy)/(float)2);

	//forces it to round up and not leave a white bar 1 px high
	if(floor(sy)!=sy){
		b1scale.y+=1;
	}

	

	b2pos.x=px;
	b2pos.y=-(conf->sizeY-b1scale.y);

	b1scale.y+=1;	//just in case

	b2scale.x=sx;
	b2scale.y=(conf->sizeY-sy)/(float)2;


	if(conf->sizeX >  conf->sizeY*conf->widescreenRatio){
		sy=conf->sizeY;
		sx=conf->sizeY*conf->widescreenRatio;
		py=0;
		px=(conf->sizeX-sx)/(float)2;

		//----------------------
		b1pos.x=0;
		b1pos.y=0;

		b1scale.x=px;
		b1scale.y=conf->sizeY;


		b2pos.x=px+sx-1;
		b2pos.y=0;

		b2scale.x=px;
		b2scale.y=conf->sizeY;
	}

	if(fade>0){
		glEnable(GL_BLEND);

		glColor4f(0,0,0,1);

		float xfade=fade;
		float yfade=fade;

		if(b1scale.x==conf->sizeX){
			xfade=0;
		}

		if(b1scale.y==conf->sizeY){
			yfade=0;
		}

		glBegin(GL_QUADS);
			glVertex3f(b1pos.x		,b1pos.y,0);
			glVertex3f(b1pos.x+b1scale.x-xfade	,b1pos.y,0);
			glVertex3f(b1pos.x+b1scale.x-xfade	,b1pos.y-b1scale.y+yfade,0);
			glVertex3f(b1pos.x		,b1pos.y-b1scale.y+yfade,0);
		glEnd();
		
		if(b1scale.y==conf->sizeY){
				

			glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex3f(b1pos.x+b1scale.x-xfade		,b1pos.y,0);
				glColor4f(0,0,0,0);
				glVertex3f(b1pos.x+b1scale.x	,b1pos.y,0);
				glVertex3f(b1pos.x+b1scale.x	,b1pos.y-b1scale.y+yfade,0);
				glColor4f(0,0,0,1);
				glVertex3f(b1pos.x+b1scale.x-xfade		,b1pos.y-b1scale.y+yfade,0);
			glEnd();
		}else{
			
			glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex3f(b1pos.x		,b1pos.y-b1scale.y+yfade,0);
				glVertex3f(b1pos.x+b1scale.x-xfade	,b1pos.y-b1scale.y+yfade,0);
				glColor4f(0,0,0,0);
				glVertex3f(b1pos.x+b1scale.x-xfade	,b1pos.y-b1scale.y,0);
				glVertex3f(b1pos.x		,b1pos.y-b1scale.y,0);
			glEnd();
		}

		//////////////////////////////////////////////////////////////////////

		glColor4f(0,0,0,1);

		glBegin(GL_QUADS);
			glVertex3f(b2pos.x+xfade		,b2pos.y-yfade,0);
			glVertex3f(b2pos.x+b2scale.x	,b2pos.y-yfade,0);
			glVertex3f(b2pos.x+b2scale.x	,b2pos.y-b2scale.y,0);
			glVertex3f(b2pos.x+xfade		,b2pos.y-b2scale.y,0);
		glEnd();
		
		if(b2scale.y==conf->sizeY){
				

			glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex3f(b2pos.x+xfade		,b2pos.y,0);
				glColor4f(0,0,0,0);
				glVertex3f(b2pos.x	,b2pos.y,0);
				glVertex3f(b2pos.x	,b2pos.y-b2scale.y,0);
				glColor4f(0,0,0,1);
				glVertex3f(b2pos.x+xfade		,b2pos.y-b2scale.y,0);
			glEnd();
		}else{
			
			glBegin(GL_QUADS);
				glColor4f(0,0,0,1);
				glVertex3f(b2pos.x		,b2pos.y-yfade,0);
				glVertex3f(b2pos.x+b2scale.x	,b2pos.y-yfade,0);
				glColor4f(0,0,0,0);
				glVertex3f(b2pos.x+b2scale.x	,b2pos.y,0);
				glVertex3f(b2pos.x		,b2pos.y,0);
			glEnd();
		}


		glDisable(GL_BLEND);
	}else{

	glBegin(GL_QUADS);
		glVertex3f(b1pos.x		,b1pos.y,0);
		glVertex3f(b1pos.x+b1scale.x	,b1pos.y,0);
		glVertex3f(b1pos.x+b1scale.x	,b1pos.y-b1scale.y,0);
		glVertex3f(b1pos.x		,b1pos.y-b1scale.y,0);
	glEnd();
	
	glBegin(GL_QUADS);
		glVertex3f(b2pos.x		,b2pos.y,0);
		glVertex3f(b2pos.x+b2scale.x	,b2pos.y,0);
		glVertex3f(b2pos.x+b2scale.x	,b2pos.y-b2scale.y,0);
		glVertex3f(b2pos.x		,b2pos.y-b2scale.y,0);
	glEnd();
		
	}
}


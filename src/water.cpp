#include "water.hpp"

#ifdef _WIN32
	#include <windows.h>	// Header File For Windows
#endif
			
#include <string>
#include <iostream>
#include <fstream>

#include "GlHeader.hpp"

#include "level.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "draw.hpp"
#include "hardware.hpp"

#include "timeing.hpp"
#include "foam.hpp"
#include "HelperLibMath.hpp"

using namespace std;

FloatVector2d waterDirection;
float waterZBias=1;
float waterDistort=2;
float waterFresnel=2;

#ifdef FEATURE_WATER

ShaderId waterFragmentShaderFresnel;
ShaderId waterVertexShaderFresnel;

ShaderId waterFragmentShaderFresnelNOZ;
ShaderId waterVertexShaderFresnelNOZ;
unsigned int glWaterName;
int waterX=512;
int waterY=512;
unsigned int bufferV;
unsigned int bufferN;
unsigned int bufferT0;
int glWaterList=-1;


void config_projective_pass(){
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	
	static Matrix4d4d textureMatrix(1.0f, 0.0f, 0.0f, 0.0f,
									0.0f, 1.0f, 0.0f,0.0f,
									0.0f, 0.0f, 1.0f, 0.0f,
									0.0f, 0.0f, 0.0f, 1.0f);
	
	
	float row[4];
	
	row[0]=textureMatrix.row(0).x;
	row[1]=textureMatrix.row(0).y;
	row[2]=textureMatrix.row(0).z;
	row[3]=textureMatrix.row(0).w;
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, row);
	glEnable(GL_TEXTURE_GEN_S);
	
	row[0]=textureMatrix.row(1).x;
	row[1]=textureMatrix.row(1).y;
	row[2]=textureMatrix.row(1).z;
	row[3]=textureMatrix.row(1).w;
	
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, row);
	glEnable(GL_TEXTURE_GEN_T);
	
	row[0]=textureMatrix.row(2).x;
	row[1]=textureMatrix.row(2).y;
	row[2]=textureMatrix.row(2).z;
	row[3]=textureMatrix.row(2).w;
	
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, row);
	glEnable(GL_TEXTURE_GEN_R);
	
	row[0]=textureMatrix.row(3).x;
	row[1]=textureMatrix.row(3).y;
	row[2]=textureMatrix.row(3).z;
	row[3]=textureMatrix.row(3).w;
	
	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, row);
	glEnable(GL_TEXTURE_GEN_Q);
}



void drawReflect(){

	if(level->water!=NULL){
		glViewport(0,0,waterX,waterY);					// Set Our Viewport (Match Texture Size)

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		bool widetemp=conf->widescreen;
		conf->widescreen=false;

		//adjust for the z-depth shore ripple effect
		float zadjust=(float(1)/(level->camera->pos.y-level->water->pos.y))*100;

		if(waterZBias==0){zadjust=0;}
		
		//I HATE MATH
		//account for rotation of water plane
		glTranslatef(0,level->water->pos.y,0);
		glRotatef(level->water->rot.x*2,	1.0f,	0,		0);
		glRotatef(level->water->rot.y*2,	0,		1.0f,	0);
		glRotatef(level->water->rot.z*2,	0,		0.0f,	1.0f);

		glTranslatef(0,level->water->pos.y+zadjust,0);
		
		glScalef(1, -1, 1);  //"reflect" across Y=0 plane. 

		glCullFace(GL_FRONT);


		Matrix4d4d m;
		Matrix4d4d mv;

		{

			GLdouble planeEqn[4] = { 0.0, -1.0, 0.0, -level->water->pos.y };

			planeEqn[0]=level->water->getVertex(0).normal.x;
			planeEqn[1]=level->water->getVertex(0).normal.y;
			planeEqn[2]=level->water->getVertex(0).normal.z;




			glEnable( GL_CLIP_PLANE0 );    // Enable clipping
			glClipPlane( GL_CLIP_PLANE0, planeEqn );  // Define the plane equation


		}

		drawLevel();	///<<<<<<<<<<<<<<<<///////////////////////////////////////////////////////////

		glCullFace(GL_BACK);

		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D,glWaterName);			// Bind To The Blur Texture

		// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, waterX, waterY, 0);

		level->fog.update();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear The Screen And Depth Buffer

		glFrontFace( GL_CCW );

		glDisable( GL_CLIP_PLANE0 );

		//to make the shaders no longer clip LOL
		GLdouble planeEqn2[4] = {0.0,0.0,0.0,0.0};
		glClipPlane( GL_CLIP_PLANE0, planeEqn2 );

		conf->widescreen=widetemp;

		level->camera->adjust();
		level->camera->transform();
		level->camera->draw();	//draw widescreen overlay
	}
}

void buildWaterList(){
	
	if(glWaterList!=-1){return;}

	glWaterList=glGenLists(1);
	glNewList(glWaterList,GL_COMPILE);	

		for(unsigned int v=0; v<level->water->getVertexCount(); v++){


				if(level->water->getVertexRaw(v).end==START_TRI){
					glBegin(GL_TRIANGLES);
				}else if(level->water->getVertexRaw(v).end==START_STRIP){
					glBegin(GL_TRIANGLE_STRIP);
				}

				glTexCoord2f(level->water->getVertexRaw(v).uvLightmap.x, level->water->getVertexRaw(v).uvLightmap.y);	//should be like this (insted of a multitexture call), b/c we're using the texture matrix

				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,level->water->getVertexRaw(v).uv2.x , level->water->getVertexRaw(v).uv2.y);	//bumpcoord layer1, displacement
				glMultiTexCoord3fARB(GL_TEXTURE3_ARB,level->water->getVertexRaw(v).tangent.x , level->water->getVertexRaw(v).tangent.y, level->water->getVertexRaw(v).tangent.z);
				glMultiTexCoord3fARB(GL_TEXTURE4_ARB,level->water->getVertexRaw(v).binormal.x , level->water->getVertexRaw(v).binormal.y, level->water->getVertexRaw(v).binormal.z);

				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,level->water->getVertexRaw(v).uv3.x , level->water->getVertexRaw(v).uv3.y);	//bumpcoord layer2

				if(level->materials[level->water->material].textureDiffuse!=-1){
					glMultiTexCoord2fARB(GL_TEXTURE6_ARB,level->water->getVertexRaw(v).uv4.x , level->water->getVertexRaw(v).uv4.y);	//bumpcoord layer2

				}

				glNormal3f(level->water->getVertexRaw(v).normal.x, 
					level->water->getVertexRaw(v).normal.y, 
					level->water->getVertexRaw(v).normal.z);

				glVertex3f(level->water->getVertexRaw(v).pos.x,
					level->water->getVertexRaw(v).pos.y,
					level->water->getVertexRaw(v).pos.z);


				
				if(level->water->getVertexRaw(v).end==END_TRI){
					glEnd();
				}else if(level->water->getVertexRaw(v).end==END_STRIP){
					glEnd();
				}

			
		}
	glEndList();
}


void drawWaterList(){
	glCallList(glWaterList);
}

void drawWaterBest(){
	
	if(level->water!=NULL){
		
		glLineWidth(1);
		
		if(waterZBias==0){
			shader.set(waterFragmentShaderFresnelNOZ);
			shader.set(waterVertexShaderFresnelNOZ);
		}else{
			shader.set(waterFragmentShaderFresnel);
			shader.set(waterVertexShaderFresnel);
		}
		
		#ifndef SOFTWARE_TRANSFORMS
		glEnable(GL_NORMALIZE);
		
		Matrix4d4d m;
		m.loadIdentity();
		m.translate(level->water->pos.x,level->water->pos.y,level->water->pos.z);			
		m.rotateZ(level->water->rot.z);
		m.rotateY(level->water->rot.y);
		m.rotateX(level->water->rot.x);
		m.scale(level->water->scale.x,level->water->scale.y,level->water->scale.z);
		
		FloatVector4d c1=m.column(0);
		FloatVector4d c2=m.column(1);
		FloatVector4d c3=m.column(2);
		FloatVector4d c4=m.column(3);
		
		shader.localParameterVertex(0,m.column(0));
		shader.localParameterVertex(1,m.column(1));
		shader.localParameterVertex(2,m.column(2));
		shader.localParameterVertex(3,m.column(3));
		shader.localParameterVertex(4,level->camera->pos.x,level->camera->pos.y,level->camera->pos.z,0);
		
		Matrix4d4d mm;
		mm.loadIdentity();
		mm.rotateZ(level->water->rot.z);
		mm.rotateY(level->water->rot.y);
		mm.rotateX(level->water->rot.x);
		
		shader.localParameterVertex(5,mm.column(0));
		shader.localParameterVertex(6,mm.column(1));
		shader.localParameterVertex(7,mm.column(2));
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(level->water->pos.x,level->water->pos.y,level->water->pos.z);			
		
		glRotatef(level->water->rot.z,0.0f,0.0f,1.0f);
		glRotatef(level->water->rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->water->rot.x,1.0f,0.0f,0.0f);
		
		glScalef(level->water->scale.x,level->water->scale.y,level->water->scale.z);
#endif
		
		glFrontFace( GL_CCW );
		glCullFace(GL_BACK);
		
		glActiveTextureARB(GL_TEXTURE0_ARB);
		
		config_projective_pass();

		glActiveTextureARB(GL_TEXTURE0_ARB);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		
		//

		glTranslatef(.5f, .5f, 0.f);

		glScalef(.5f, .5f, 1.0f);
		
		level->camera->projectionMatrix();
		
		level->camera->transform();

		#ifndef SOFTWARE_TRANSFORMS
		
		glTranslatef(level->water->pos.x,level->water->pos.y,level->water->pos.z);			
		
		glRotatef(level->water->rot.z,0.0f,0.0f,1.0f);
		glRotatef(level->water->rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->water->rot.x,1.0f,0.0f,0.0f);

		glScalef(level->water->scale.x,level->water->scale.y,level->water->scale.z);

		#endif
		glMatrixMode(GL_MODELVIEW);

		static bool wait=true;

		if(!wait){	//wait a frame for bounding boxes to be built
			float d=level->water->box.px - level->water->box.nx;
			d=(level->water->uv2Box.px - level->water->uv2Box.nx)/d;

			level->materials[level->water->material].tex3rot=level->materials[level->water->material].tex2rot;
			level->materials[level->water->material].tex3scale=level->materials[level->water->material].tex2scale;
			level->materials[level->water->material].tex4rot=level->materials[level->water->material].tex0rot;
			level->materials[level->water->material].tex4scale=level->materials[level->water->material].tex0scale;

			
			level->materials[level->water->material].tex2pos.x+=(waterDirection.x*1)*d*engineTime.time;
			level->materials[level->water->material].tex3pos.x+=(waterDirection.x*0.45)*d*engineTime.time;
			level->materials[level->water->material].tex4pos.x+=(waterDirection.x*2)*d*engineTime.time;

			//////////////////////////////////////////////////////
			d=level->water->box.pz-level->water->box.nz;
			d=(level->water->uv2Box.py-level->water->uv2Box.ny)/d;

			level->materials[level->water->material].tex2pos.y+=(waterDirection.y*1)*d*engineTime.time;
			level->materials[level->water->material].tex3pos.y+=(waterDirection.y*0.35)*d*engineTime.time;
			level->materials[level->water->material].tex4pos.y+=(waterDirection.y*1)*d*engineTime.time;
		}

		wait=false;

		level->water->update();

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,glWaterName);
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);


		//set up the sun!~!!
		glEnable(GL_LIGHTING);
		GLfloat LightDiffuse[4];
		LightDiffuse[0]=conf->sunColor.x;
		LightDiffuse[1]=conf->sunColor.y;
		LightDiffuse[2]=conf->sunColor.z;
		LightDiffuse[3]=conf->sunColor.w;

		GLfloat LightPos[4];
		LightPos[0]=conf->sunPos.x;
		LightPos[1]=conf->sunPos.y;
		LightPos[2]=conf->sunPos.z;
		LightPos[3]=0.0f;

		glLightfv(GL_LIGHT0, GL_DIFFUSE,LightDiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION,LightPos);
		glEnable(GL_LIGHT0);

		glColor4f(level->materials[level->water->material].diffuse.x,level->materials[level->water->material].diffuse.y,level->materials[level->water->material].diffuse.z,level->materials[level->water->material].diffuse.w);

		shader.localParameterFragment(0,FloatVector4d(level->materials[level->water->material].shiny,0,waterDistort,waterFresnel));
		shader.localParameterFragment(2,level->materials[level->water->material].reflectivity);

		float y=level->camera->pos.y-level->water->pos.y;

		y=float(1)/y;

		y*=0.08;
		y*=waterZBias;

		shader.localParameterFragment(1,FloatVector4d(0,0,y,0));	//fake depth/z-pertrube bias

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glEnable(GL_TEXTURE_2D);
		
		if(level->water->material!=-1){
			glBindTexture(GL_TEXTURE_2D,level->textures[level->materials[level->water->material].textureDisplacement].glName());
		}

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glEnable(GL_TEXTURE_2D);
		
		if(level->water->material!=-1){
			glBindTexture(GL_TEXTURE_2D,level->textures[level->materials[level->water->material].textureBump].glName());
		}

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		

		glActiveTextureARB(GL_TEXTURE4_ARB);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);

		glActiveTextureARB(GL_TEXTURE5_ARB);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);

		glActiveTextureARB(GL_TEXTURE6_ARB);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);

		glActiveTextureARB(GL_TEXTURE7_ARB);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);

		glActiveTextureARB(GL_TEXTURE8_ARB);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		
		for(unsigned int v=0; v<level->water->getVertexCount(); v++){
			
			
			if(level->water->getVertexRaw(v).end==START_TRI){
				glBegin(GL_TRIANGLES);
			}else if(level->water->getVertexRaw(v).end==START_STRIP){
				glBegin(GL_TRIANGLE_STRIP);
			}
			
			glMultiTexCoord2fARB(GL_TEXTURE1_ARB,level->water->getVertexRaw(v).uv2.x , level->water->getVertexRaw(v).uv2.y);	//bumpcoord layer1, displacement
			
			glMultiTexCoord3fARB(GL_TEXTURE3_ARB,level->water->getVertexRaw(v).tangent.x , level->water->getVertexRaw(v).tangent.y, level->water->getVertexRaw(v).tangent.z);
			glMultiTexCoord3fARB(GL_TEXTURE4_ARB,level->water->getVertexRaw(v).binormal.x , level->water->getVertexRaw(v).binormal.y, level->water->getVertexRaw(v).binormal.z);
			
			glMultiTexCoord2fARB(GL_TEXTURE5_ARB,level->water->getVertexRaw(v).uv3.x , level->water->getVertexRaw(v).uv3.y);	//bumpcoord layer2
			
			glNormal3f(level->water->getVertexRaw(v).normal.x, 
					   level->water->getVertexRaw(v).normal.y, 
					   level->water->getVertexRaw(v).normal.z);
			
			glVertex3f(level->water->getVertexRaw(v).pos.x,
					   level->water->getVertexRaw(v).pos.y,
					   level->water->getVertexRaw(v).pos.z);
			
			
			
			if(level->water->getVertexRaw(v).end==END_TRI){
				glEnd();
			}else if(level->water->getVertexRaw(v).end==END_STRIP){
				glEnd();
			}
			
			
		}
		
		#ifndef SOFTWARE_TRANSFORMS
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		#endif

		shader.reset();

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);


		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

#ifdef FEATURE_FOAM
		drawFoam();
#endif

	}

}

void drawWaterGood(){

}


static int luaWaterBuild(lua_State *L){
#ifdef FEATURE_FOAM
	buildFoam();
#endif
	return 0;
}



#endif


void drawWaterBad(){

	if(level->water!=NULL){
		shader.reset();

		static bool wait=true;

		if(!wait){	//wait a frame for bounding boxes to be built
	

			float d=level->water->box.px - level->water->box.nx;
			d=(level->water->uv0Box.px - level->water->uv0Box.nx)/d;

			level->materials[level->water->material].tex3rot=level->materials[level->water->material].tex0rot;
			level->materials[level->water->material].tex3scale=level->materials[level->water->material].tex0scale;
						
			level->materials[level->water->material].tex0pos.x+=(waterDirection.x*1)*d*engineTime.time;
			level->materials[level->water->material].tex3pos.x+=(waterDirection.x*0.45)*d*engineTime.time;

			//////////////////////////////////////////////////////
			d=level->water->box.pz-level->water->box.nz;
			d=(level->water->uv0Box.py-level->water->uv0Box.ny)/d;

			level->materials[level->water->material].tex0pos.y+=(waterDirection.y*1)*d*engineTime.time;
			level->materials[level->water->material].tex3pos.y+=(waterDirection.y*0.35)*d*engineTime.time;
		}

		wait=false;

		level->water->update();
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE4_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE5_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE6_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE7_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE8_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);

		glEnable(GL_TEXTURE_2D);

		if(level->materials[level->water->material].textureDiffuse!=-1){
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[level->materials[level->water->material].textureDiffuse].glName());
		}else{
			glDisable(GL_TEXTURE_2D);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(0);

		glDisable(GL_LIGHTING);


		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);

		for(int i=0; i<2; i++){

			#ifndef SOFTWARE_TRANSFORM
			glMatrixMode(GL_MODELVIEW);
					glPushMatrix();
					glTranslatef(level->water->pos.x,level->water->pos.y,level->water->pos.z);			
					
					if(i==1){
						glTranslatef(0,0.03,0);			
						glColor4f(1,1,1,level->materials[level->water->material].diffuse.w*0.5);
					}else{
						glColor4f(1,1,1,level->materials[level->water->material].diffuse.w);
					}

					glRotatef(level->water->rot.z,0.0f,0.0f,1.0f);
					glRotatef(level->water->rot.y,0.0f,1.0f,0.0f);
					glRotatef(level->water->rot.x,1.0f,0.0f,0.0f);

					glScalef(level->water->scale.x,level->water->scale.y,level->water->scale.z);


			#endif

			for(unsigned int v=0; v<level->water->getVertexCount(); v++){

					if(level->water->getVertexRaw(v).end==START_TRI){
						glBegin(GL_TRIANGLES);
					}else if(level->water->getVertexRaw(v).end==START_STRIP){
						glBegin(GL_TRIANGLE_STRIP);
					}

					if(i==1){
						glTexCoord2f(level->water->getVertexRaw(v).uv0.x, level->water->getVertexRaw(v).uv0.y);
					}else{
						glTexCoord2f(level->water->getVertexRaw(v).uv3.x, level->water->getVertexRaw(v).uv3.y);
					}

					glNormal3f(level->water->getVertexRaw(v).normal.x, 
						level->water->getVertexRaw(v).normal.y, 
						level->water->getVertexRaw(v).normal.z);

					glVertex3f(level->water->getVertexRaw(v).pos.x,
						level->water->getVertexRaw(v).pos.y,
						level->water->getVertexRaw(v).pos.z);

					if(level->water->getVertexRaw(v).end==END_TRI){
						glEnd();
					}else if(level->water->getVertexRaw(v).end==END_STRIP){
						glEnd();
					}
			}

			#ifndef SOFTWARE_TRANSFORM
				glPopMatrix();
			#endif

		}
	}

	glDepthMask(1);
}


bool waterScriptInit(){
	
	LuaType* w=script().add("water");

	LuaType* wd=script().add("direction",w);
				
					script().add("x",&waterDirection.x,wd);
					script().add("y",&waterDirection.y,wd);

#ifdef FEATURE_WATER
		script().add("build",&luaWaterBuild,w);
		script().add("zBias",&waterZBias,w);
		script().add("fresnel",&waterFresnel,w);
		script().add("distort",&waterDistort,w);


		
#ifdef FEATURE_FOAM


		LuaType* f=script().add("foam",w);
				
					script().add("age",&fAge,f);
					script().add("decay",&fDecay,f);
					script().add("size",&fSize,f);
					script().add("speed",&fSpeed,f);
					script().add("alpha",&fAlpha,f);
#endif

#endif

	waterDirection.x=1;
	waterDirection.y=3;

	return true;

}

bool waterInit(){

#ifdef FEATURE_WATER
	//don't bother
	if(!hardware.fragmentProgram){
		return true;
	}


	waterFragmentShaderFresnel=shader.addFragmentFile("shaders/water_spec_fresnel.fp","8f6d9aed7c899ab63539e717d73bd3a5");

#ifdef SOFTWARE_TRANSFORMS
	waterVertexShaderFresnel=shader.addVertexFile("shaders/water_spec_fresnel.vp");
	waterVertexShaderFresnelNOZ=shader.addVertexFile("shaders/water_spec_fresnel.vp");
#else
	waterVertexShaderFresnel=shader.addVertexFile("shaders/water_spec_fresnel_hardware.vp","aed8757966779e638f37d3e11c8cf17c");
	waterVertexShaderFresnelNOZ=shader.addVertexFile("shaders/water_spec_fresnel_hardware.vp");
#endif

	Path wsf;
	wsf.setRelative("shaders/water_spec_fresnel.fp");


	std::ifstream in(wsf.getAbsolute().cStr());


	std::string s, line;
	while(getline(in, line)){
		s += line + "\n";
	}

	String ts=s;
	ts.replace("SUB result.depth, fragment.position.z, z;","");
	waterFragmentShaderFresnelNOZ=shader.addFragment(ts);	

	glWaterName=EmptyTexture(waterX,waterY);
	

#endif
	
	
	return true;
}

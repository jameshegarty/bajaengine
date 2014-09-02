#include "level.hpp"
#include "texture.hpp"
#include "conf.h"
#include "draw.hpp"
#include "timeing.hpp"
#include "shader.hpp"
#include "hardware.hpp"

#ifdef FEATURE_REFLECT

 

ShaderId reflectFragmentShader;
ShaderId reflectVertexShader;
Array<unsigned int> glReflectTextures;
int reflectTexX=512;
int reflectTexY=512;


void config_projective_passR()
{
	

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


bool reflectiveObjectsInit(){

	//don't bother initing if we can't do reflections
	if(!hardware.fragmentProgram){
		return true;
	}


	reflectFragmentShader=shader.addFragmentFile("shaders/reflect.fp");

	#ifdef SOFTWARE_TRANSFORMS
		reflectVertexShader=shader.addVertexFile("shaders/reflect.vp");
	#else
		reflectVertexShader=shader.addVertexFile("shaders/reflect_hardware.vp");
	#endif

	return true;
}


void drawReflectiveObjectsReflect(){

	if(conf->reflect=="best"){

	}else{
		return;
	}

	for(int i=0; i<level->reflectiveObjects.size(); i++){

		if(glReflectTextures.size()<=i){
			unsigned int lol=EmptyTexture(reflectTexX,reflectTexY);
			glReflectTextures.pushBack(lol);
		}

		glViewport(0,0,reflectTexX,reflectTexY);					// Set Our Viewport (Match Texture Size)

		GLdouble planeEqn[4] = { 0.0, -1.0, 0.0, level->reflectiveObjects[i]->pos.y };

		glEnable( GL_CLIP_PLANE0 );    // Enable clipping
		glClipPlane( GL_CLIP_PLANE0, planeEqn );  // Define the plane equation

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		bool widetemp=conf->widescreen;
		conf->widescreen=false;
		glTranslatef(0,level->reflectiveObjects[i]->pos.y*2,0);
		glScalef(1, -1, 1);  //"reflect" across Y=0 plane. 


		//glFrontFace( GL_CW );
		glCullFace(GL_FRONT);
		drawLevel();	///<<<<<<<<<<<<<<<<///////////////////////////////////////////////////////////
		glCullFace(GL_BACK);

		glPopMatrix();

		glBindTexture(GL_TEXTURE_2D,glReflectTextures[i]);			// Bind To The Blur Texture

		// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, reflectTexX, reflectTexY, 0);

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

void drawReflectiveObjectsBest(){

	for(int i=0; i<level->reflectiveObjects.size(); i++){

		if(glReflectTextures.size()<=i){
			unsigned int lol=EmptyTexture(reflectTexX,reflectTexY);
			glReflectTextures.pushBack(lol);
		}

		shader.set(reflectFragmentShader);
		shader.set(reflectVertexShader);

		#ifndef SOFTWARE_TRANSFORMS
			glEnable(GL_NORMALIZE);

				Matrix4d4d m;
				m.loadIdentity();
				m.translate(level->reflectiveObjects[i]->pos.x,level->reflectiveObjects[i]->pos.y,level->reflectiveObjects[i]->pos.z);			
				m.rotateZ(level->reflectiveObjects[i]->rot.z);
				m.rotateY(level->reflectiveObjects[i]->rot.y);
				m.rotateX(level->reflectiveObjects[i]->rot.x);
				m.scale(level->reflectiveObjects[i]->scale.x,level->reflectiveObjects[i]->scale.y,level->reflectiveObjects[i]->scale.z);

				shader.localParameterVertex(0,m.column(0));
				shader.localParameterVertex(1,m.column(1));
				shader.localParameterVertex(2,m.column(2));
				shader.localParameterVertex(3,m.column(3));
				shader.localParameterVertex(4,level->camera->pos.x,level->camera->pos.y,level->camera->pos.z,0);

				Matrix4d4d mm;
				mm.loadIdentity();
				mm.rotateZ(level->reflectiveObjects[i]->rot.z);
				mm.rotateY(level->reflectiveObjects[i]->rot.y);
				mm.rotateX(level->reflectiveObjects[i]->rot.x);

				shader.localParameterVertex(5,mm.column(0));
				shader.localParameterVertex(6,mm.column(1));
				shader.localParameterVertex(7,mm.column(2));

			glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslatef(level->reflectiveObjects[i]->pos.x,level->reflectiveObjects[i]->pos.y,level->reflectiveObjects[i]->pos.z);			
				
				glRotatef(level->reflectiveObjects[i]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(level->reflectiveObjects[i]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(level->reflectiveObjects[i]->rot.x,1.0f,0.0f,0.0f);

				glScalef(level->reflectiveObjects[i]->scale.x,level->reflectiveObjects[i]->scale.y,level->reflectiveObjects[i]->scale.z);

		#endif

		glFrontFace( GL_CCW );
		glCullFace(GL_BACK);

		glActiveTextureARB(GL_TEXTURE0_ARB);


		config_projective_passR();


		glActiveTextureARB(GL_TEXTURE0_ARB);

		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		
		//

		glTranslatef(.5f, .5f, 0.f);

		glScalef(.5f, .5f, 1.0f);
		

		level->camera->projectionMatrix();
		
		level->camera->transform();
		
		#ifndef SOFTWARE_TRANSFORMS
		
		glTranslatef(level->reflectiveObjects[i]->pos.x,level->reflectiveObjects[i]->pos.y,level->reflectiveObjects[i]->pos.z);			
		
		glRotatef(level->reflectiveObjects[i]->rot.z,0.0f,0.0f,1.0f);
		glRotatef(level->reflectiveObjects[i]->rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->reflectiveObjects[i]->rot.x,1.0f,0.0f,0.0f);

		glScalef(level->reflectiveObjects[i]->scale.x,level->reflectiveObjects[i]->scale.y,level->reflectiveObjects[i]->scale.z);

		#endif

		glMatrixMode(GL_MODELVIEW);

		level->reflectiveObjects[i]->update();

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,level->textures[level->materials[level->reflectiveObjects[i]->material].textureDiffuse].glName());

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,glReflectTextures[i]);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,level->lightmaps[level->reflectiveObjects[i]->lightmap].glName());

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

		glColor4f(level->materials[level->reflectiveObjects[i]->material].diffuse.x,
			level->materials[level->reflectiveObjects[i]->material].diffuse.y,
			level->materials[level->reflectiveObjects[i]->material].diffuse.z,
			level->materials[level->reflectiveObjects[i]->material].diffuse.w);
		
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

		glActiveTextureARB(GL_TEXTURE0_ARB);

		level->materials[level->reflectiveObjects[i]->material].reflectivity.w=1;
		shader.localParameterFragment(0,level->materials[level->reflectiveObjects[i]->material].reflectivity);

		for(unsigned int v=0; v<level->reflectiveObjects[i]->getVertexCount(); v++){


				if(level->reflectiveObjects[i]->getVertexRaw(v).end==START_TRI){
					glBegin(GL_TRIANGLES);
				}else if(level->reflectiveObjects[i]->getVertexRaw(v).end==START_STRIP){
					glBegin(GL_TRIANGLE_STRIP);
				}

				glTexCoord2f(level->reflectiveObjects[i]->getVertexRaw(v).uvLightmap.x, level->reflectiveObjects[i]->getVertexRaw(v).uvLightmap.y);

				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,level->reflectiveObjects[i]->getVertexRaw(v).uv0.x , level->reflectiveObjects[i]->getVertexRaw(v).uv0.y);
				glMultiTexCoord2fARB(GL_TEXTURE2_ARB,level->reflectiveObjects[i]->getVertexRaw(v).uvLightmap.x , level->reflectiveObjects[i]->getVertexRaw(v).uvLightmap.y);
				glMultiTexCoord3fARB(GL_TEXTURE3_ARB,level->reflectiveObjects[i]->getVertexRaw(v).tangent.x , level->reflectiveObjects[i]->getVertexRaw(v).tangent.y, level->reflectiveObjects[i]->getVertexRaw(v).tangent.z);
				glMultiTexCoord3fARB(GL_TEXTURE4_ARB,level->reflectiveObjects[i]->getVertexRaw(v).binormal.x , level->reflectiveObjects[i]->getVertexRaw(v).binormal.y, level->reflectiveObjects[i]->getVertexRaw(v).binormal.z);

				glMultiTexCoord2fARB(GL_TEXTURE5_ARB,level->reflectiveObjects[i]->getVertexRaw(v).uv0.x , level->reflectiveObjects[i]->getVertexRaw(v).uv0.y);

				glNormal3f(level->reflectiveObjects[i]->getVertexRaw(v).normal.x, 
					level->reflectiveObjects[i]->getVertexRaw(v).normal.y, 
					level->reflectiveObjects[i]->getVertexRaw(v).normal.z);

				glVertex3f(level->reflectiveObjects[i]->getVertexRaw(v).pos.x,
					level->reflectiveObjects[i]->getVertexRaw(v).pos.y,
					level->reflectiveObjects[i]->getVertexRaw(v).pos.z);


				
				if(level->reflectiveObjects[i]->getVertexRaw(v).end==END_TRI){
					glEnd();
				}else if(level->reflectiveObjects[i]->getVertexRaw(v).end==END_STRIP){
					glEnd();
				}

			
		}

		#ifndef SOFTWARE_TRANSFORMS
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		#endif


		shader.reset();
	
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);


		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		
	}
}




#endif


void drawReflectiveObjectsBad(){

	for(int i=0; i<level->reflectiveObjects.size(); i++){

		shader.reset();

		#ifndef SOFTWARE_TRANSFORMS
			glEnable(GL_NORMALIZE);

			glMatrixMode(GL_MODELVIEW);
				glPushMatrix();
				glTranslatef(level->reflectiveObjects[i]->pos.x,level->reflectiveObjects[i]->pos.y,level->reflectiveObjects[i]->pos.z);			
				
				glRotatef(level->reflectiveObjects[i]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(level->reflectiveObjects[i]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(level->reflectiveObjects[i]->rot.x,1.0f,0.0f,0.0f);

				glScalef(level->reflectiveObjects[i]->scale.x,level->reflectiveObjects[i]->scale.y,level->reflectiveObjects[i]->scale.z);
		#endif

		glFrontFace( GL_CCW );
		glCullFace(GL_BACK);

		glActiveTextureARB(GL_TEXTURE0_ARB);


		glMatrixMode(GL_MODELVIEW);

		level->reflectiveObjects[i]->update();



		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,level->textures[level->materials[level->reflectiveObjects[i]->material].textureDiffuse].glName());


		glActiveTextureARB(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,level->lightmaps[level->reflectiveObjects[i]->lightmap].glName());


		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

		//set up the sun!~!!
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);

    int textureDiffuse = level->materials[level->reflectiveObjects[i]->material].textureDiffuse;

    if(textureDiffuse!=-1){
      glColor4f(1,1,1,level->materials[level->reflectiveObjects[i]->material].diffuse.w);	//textured objs always full bright
    }else{
      glColor4f(level->materials[level->reflectiveObjects[i]->material].diffuse.x,
                level->materials[level->reflectiveObjects[i]->material].diffuse.y,
                level->materials[level->reflectiveObjects[i]->material].diffuse.z,
                level->materials[level->reflectiveObjects[i]->material].diffuse.w);
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

		glActiveTextureARB(GL_TEXTURE0_ARB);


		for(unsigned int v=0; v<level->reflectiveObjects[i]->getVertexCount(); v++){


				if(level->reflectiveObjects[i]->getVertexRaw(v).end==START_TRI){
					glBegin(GL_TRIANGLES);
				}else if(level->reflectiveObjects[i]->getVertexRaw(v).end==START_STRIP){
					glBegin(GL_TRIANGLE_STRIP);
				}

				glMultiTexCoord2fARB(GL_TEXTURE0_ARB,level->reflectiveObjects[i]->getVertexRaw(v).uv0.x , level->reflectiveObjects[i]->getVertexRaw(v).uv0.y);
				glMultiTexCoord2fARB(GL_TEXTURE1_ARB,level->reflectiveObjects[i]->getVertexRaw(v).uvLightmap.x , level->reflectiveObjects[i]->getVertexRaw(v).uvLightmap.y);

	
				glNormal3f(level->reflectiveObjects[i]->getVertexRaw(v).normal.x, 
					level->reflectiveObjects[i]->getVertexRaw(v).normal.y, 
					level->reflectiveObjects[i]->getVertexRaw(v).normal.z);

				glVertex3f(level->reflectiveObjects[i]->getVertexRaw(v).pos.x,
					level->reflectiveObjects[i]->getVertexRaw(v).pos.y,
					level->reflectiveObjects[i]->getVertexRaw(v).pos.z);


				
				if(level->reflectiveObjects[i]->getVertexRaw(v).end==END_TRI){
					glEnd();
				}else if(level->reflectiveObjects[i]->getVertexRaw(v).end==END_STRIP){
					glEnd();
				}

			
		}

		#ifndef SOFTWARE_TRANSFORMS
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		#endif


		shader.reset();

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);


		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

	}
}

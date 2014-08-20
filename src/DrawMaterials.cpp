#ifdef _WIN32
#include <windows.h>	// Header File For Windows
#include <gl/gl.h>												// Header File For The OpenGL32 Library
#include <gl/glu.h>	
#elif LINUX
	#include <gl.h>
#else
#include <GLUT/glut.h>
#endif				
											// Header File For The GLu32 Library
#include "Extensions.h"
#include "draw.hpp"
#include "DrawDynamic.h"
#include "level.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mouse.h"
#include "picking.h"
#include "hardware.hpp"
#include "random.h"

#include "Log.hpp"
#include "sort.hpp"


#include <math.h>


void updateLights(Object* obj){

	if(obj->lightmap==-1){
		lightState=true;

	}else if(obj->lightmap!=-1){
		lightState=false;
	}

	if(lightState){

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);

	
	}else{
		int lm=obj->lightmap;

		if(lm!=-1){
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glEnable(GL_TEXTURE_2D);


			glBindTexture(GL_TEXTURE_2D,level->lightmaps[lm].glName());
		}else{
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);
		}	
	}

		//first, sort

		//then set
		for(int i=0; i<maxLights; i++){

			GLenum thelight=GL_LIGHT0;

			GLint maxlight=0;
			glGetIntegerv(GL_MAX_LIGHTS,&maxlight);

			if(i==0 && maxlight>0){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light0");
#endif
				thelight=GL_LIGHT0;
			}else if(i==1 && maxlight>1){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light1");
#endif
				thelight=GL_LIGHT1;
			}else if(i==2 && maxlight>2){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light2");
#endif
				thelight=GL_LIGHT2;
			}else if(i==3 && maxlight>3){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light3");
#endif
				thelight=GL_LIGHT3;
			}else if(i==4 && maxlight>4){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light4");
#endif
				thelight=GL_LIGHT4;
			}else if(i==5 && maxlight>5){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light5");
#endif
				thelight=GL_LIGHT5;
			}else if(i==6 && maxlight>6){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light6");
#endif
				thelight=GL_LIGHT6;
			}else if(i==7 && maxlight>7){
#ifdef LOOP_DEBUG
	logs().main.write("updateLights light7");
#endif
				thelight=GL_LIGHT7;
			}else{
#ifdef LOOP_DEBUG
	logs().main.write("updateLights lightEXCEED");
#endif
			}

			if(i<level->lights.size()){

				glEnable(thelight);

				GLfloat LightDiffuse[4];

				LightDiffuse[0]=level->lights[i]->color.x;
				LightDiffuse[1]=level->lights[i]->color.y;
				LightDiffuse[2]=level->lights[i]->color.z;
				LightDiffuse[3]=1;


				GLfloat LightPosition[4];
				LightPosition[0]=level->lights[i]->pos.x;
				LightPosition[1]=level->lights[i]->pos.y;
				LightPosition[2]=level->lights[i]->pos.z;
				LightPosition[3]=1;

				glLightfv(thelight, GL_DIFFUSE, LightDiffuse);
				
				glLightfv(thelight, GL_POSITION, LightPosition);

				
				
			}else{
				glDisable(thelight);
				
			}

			
		}

}

bool envReset=true;	
bool shadReset=true;

void updateMaterial(Object* obj,bool transform){
	
	if(shadReset || !transform){	//when transform==false, we are building nonstatic objs, where THE ORDER CAN CHANGE, so we always need to reset the shader

		glDisable(GL_ALPHA_TEST);

		//we do some calls outside this function to prevent rebuilding lists with env map objects in them - this allows that to happen
		if(!(level->materials[obj->material].shader & MATERIAL_SHADER_ENVIRONMENT) || conf->environmentMaps=="none"){
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);

			glActiveTextureARB(GL_TEXTURE0_ARB);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
			glDisable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		}

		
		glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_2D);

		//this is always the lightmap... so we dont want to disable it
		//glActiveTextureARB(GL_TEXTURE1_ARB);
		//glDisable(GL_TEXTURE_2D);

		
#ifdef LOOP_DEBUG
	logs().main.write("updateMaterial texture DISABLE");
#endif

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
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

		if(hardware.fragmentProgram && !transform && 
			((level->materials[obj->material].shader & MATERIAL_SHADER_DISPLACEMENT) || 
			(level->materials[obj->material].shader & MATERIAL_SHADER_SPECULAR)) ||
			obj->envelopes.size()>0 ){

		}else{
			shader.reset();
			
		}

		shadReset=false;
	}

	if(envReset && (!(level->materials[obj->material].shader & MATERIAL_SHADER_ENVIRONMENT) ||  conf->environmentMaps=="none") ){
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);


		envReset=false;


		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);

	}

	if(obj->material!=-1){

		GLfloat LightAmbient[4];

		if(obj->lightmap==-1){
			LightAmbient[0]=level->materials[obj->material].ambient.x;
			LightAmbient[1]=level->materials[obj->material].ambient.y;
			LightAmbient[2]=level->materials[obj->material].ambient.z;
			LightAmbient[3]=1;
		}else{
			LightAmbient[0]=1;
			LightAmbient[1]=1;
			LightAmbient[2]=1;
			LightAmbient[3]=1;
		}	




		glMaterialfv(GL_FRONT,GL_AMBIENT,LightAmbient);

		GLfloat MatDiffuse[4];
		MatDiffuse[0]=level->materials[obj->material].diffuse.x;
		MatDiffuse[1]=level->materials[obj->material].diffuse.y;
		MatDiffuse[2]=level->materials[obj->material].diffuse.z;
		MatDiffuse[3]=level->materials[obj->material].diffuse.w;


		glMaterialfv(GL_FRONT,GL_DIFFUSE,MatDiffuse);


		// for stencil stuff, ie, if this is a mask
		if(level->materials[obj->material].diffuse.x>0.5){
			// add to mask
			glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		}else{
			// subtract from mask
			glStencilOp(GL_KEEP,GL_KEEP,GL_ZERO);
		}

		glColor4f(level->materials[obj->material].diffuse.x,
			level->materials[obj->material].diffuse.y,
			level->materials[obj->material].diffuse.z,
			level->materials[obj->material].diffuse.w);

		GLfloat LightSpecular[4];
		LightSpecular[0]=level->materials[obj->material].specular.x;
		LightSpecular[1]=level->materials[obj->material].specular.y;
		LightSpecular[2]=level->materials[obj->material].specular.z;
		LightSpecular[3]=level->materials[obj->material].specular.w;

		glMaterialfv(GL_FRONT,GL_SPECULAR,LightSpecular);

		glMateriali(GL_FRONT, GL_SHININESS, (int)level->materials[obj->material].shiny);	//PARAM specExp = state.material.shininess; in fragment program

		int textureDiffuse=level->materials[obj->material].textureDiffuse;
		int textureDisplacement=level->materials[obj->material].textureDisplacement;
		int textureSpecular=level->materials[obj->material].textureSpecular;
		int textureBump=level->materials[obj->material].textureBump;
		int textureEnvironmentFront=level->materials[obj->material].textureEnvironmentFront;
		int textureEnvironmentBack=level->materials[obj->material].textureEnvironmentBack;
		int textureEnvironmentLeft=level->materials[obj->material].textureEnvironmentLeft;
		int textureEnvironmentRight=level->materials[obj->material].textureEnvironmentRight;
		int textureEnvironmentTop=level->materials[obj->material].textureEnvironmentTop;
		int textureEnvironmentBottom=level->materials[obj->material].textureEnvironmentBottom;

		if( (level->materials[obj->material].shader & MATERIAL_SHADER_MIXER)  && 
			textureDiffuse!=-1 && 
			textureEnvironmentFront!=-1 && 
			textureEnvironmentBack!=-1  && 
			hardware.fragmentProgram &&
			#ifdef FEATURE_MULTITEXTURE 
			true 
			#else 
			false 
			#endif
			){
			
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());

			//color1
			glActiveTextureARB(GL_TEXTURE2_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureEnvironmentFront].glName());
				
			//color1_w
			glActiveTextureARB(GL_TEXTURE3_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureEnvironmentBack].glName());

			//color2
			if(textureEnvironmentLeft!=-1){
				glActiveTextureARB(GL_TEXTURE4_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,level->textures[textureEnvironmentLeft].glName());
			}

			//color2_w
			if(textureEnvironmentRight!=-1){
				glActiveTextureARB(GL_TEXTURE5_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,level->textures[textureEnvironmentRight].glName());
			}

			//color3
			if(textureEnvironmentTop!=-1){
				glActiveTextureARB(GL_TEXTURE6_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,level->textures[textureEnvironmentTop].glName());
			}

			//color3_w
			if(textureEnvironmentBottom!=-1){
				glActiveTextureARB(GL_TEXTURE7_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,level->textures[textureEnvironmentBottom].glName());

				//the hacked 8th texcoord... we can only send 7 so we derive the last from the lightmap coords
				
			}

			if(textureEnvironmentTop!=-1 && textureEnvironmentBottom!=-1){
				shader.set(mixer_3_v);
				shader.set(mixer_3_f);
			}else if(textureEnvironmentLeft!=-1 && textureEnvironmentRight!=-1){
				shader.set(mixer_2_v);
				shader.set(mixer_2_f);
			}else{
				shader.set(mixer_1_v);
				shader.set(mixer_1_f);
			}


			FloatVector4d t=FloatVector4d(level->materials[obj->material].tex6scale.x,level->materials[obj->material].tex6scale.y,1,1);
			shader.localParameterFragment(0,t);

			shadReset=true;

		}else if( conf->displacement  && 
				textureSpecular!=-1  && 
				textureDisplacement!=-1 && 
				hardware.fragmentProgram   && 
				(level->materials[obj->material].shader & MATERIAL_SHADER_DISPLACEMENT) &&
				#ifdef FEATURE_DISPLACEMENT
				true
				#else
				false
				#endif
				){

		
			if(transform){
				if(textureBump!=-1){
					shader.set(displaceSpecularBumpFragmentShader);
					shader.set(displaceSpecularBumpVertexShader);
				}else{
					shader.set(displaceSpecularFragmentShader);
					shader.set(displaceSpecularVertexShader);

					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GREATER,0.1);
				}
			}

			#ifndef SOFTWARE_TRANSFORMS
			if(transform){
				Matrix4d4d m;
				m.loadIdentity();
				m.translate(obj->pos.x,obj->pos.y,obj->pos.z);			
				m.rotateZ(obj->rot.z);
				m.rotateY(obj->rot.y);
				m.rotateX(obj->rot.x);
				m.scale(obj->scale.x,obj->scale.y,obj->scale.z);

				FloatVector4d c1=m.column(0);
				FloatVector4d c2=m.column(1);
				FloatVector4d c3=m.column(2);
				FloatVector4d c4=m.column(3);

				shader.localParameterVertex(0,m.column(0));
				shader.localParameterVertex(1,m.column(1));
				shader.localParameterVertex(2,m.column(2));
				shader.localParameterVertex(3,m.column(3));

				Matrix4d4d mm;
				mm.loadIdentity();
				mm.rotateZ(obj->rot.z);
				mm.rotateY(obj->rot.y);
				mm.rotateX(obj->rot.x);

				shader.localParameterVertex(4,mm.column(0));
				shader.localParameterVertex(5,mm.column(1));
				shader.localParameterVertex(6,mm.column(2));


			}
			#endif

			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());
				
			glActiveTextureARB(GL_TEXTURE2_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureDisplacement].glName());
				
			glActiveTextureARB(GL_TEXTURE3_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureSpecular].glName()); 

			if(textureBump!=-1){

				glActiveTextureARB(GL_TEXTURE4_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,level->textures[textureBump].glName()); 
			}
			
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

			shader.localParameterFragment(0,level->materials[obj->material].shiny);
			shadReset=true;
			
		}else if(conf->displacement  && 
				hardware.fragmentProgram && 
				textureDisplacement!=-1  && 
				textureSpecular==-1  && 
				(level->materials[obj->material].shader & MATERIAL_SHADER_DISPLACEMENT) &&
				#ifdef FEATURE_DISPLACEMENT
				true
				#else
				false
				#endif
				){

			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());

			glActiveTextureARB(GL_TEXTURE2_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureDisplacement].glName());

			if(transform){
				shader.set(displaceFragmentShader);
				shader.set(displaceVertexShader);
			}

			#ifndef SOFTWARE_TRANSFORMS
			if(transform){
				Matrix4d4d m;
				m.loadIdentity();
				m.translate(obj->pos.x,obj->pos.y,obj->pos.z);			
				m.rotateZ(obj->rot.z);
				m.rotateY(obj->rot.y);
				m.rotateX(obj->rot.x);
				m.scale(obj->scale.x,obj->scale.y,obj->scale.z);

				FloatVector4d c1=m.column(0);
				FloatVector4d c2=m.column(1);
				FloatVector4d c3=m.column(2);
				FloatVector4d c4=m.column(3);

				shader.localParameterVertex(0,m.column(0));
				shader.localParameterVertex(1,m.column(1));
				shader.localParameterVertex(2,m.column(2));
				shader.localParameterVertex(3,m.column(3));

				Matrix4d4d mm;
				mm.loadIdentity();
				mm.rotateZ(obj->rot.z);
				mm.rotateY(obj->rot.y);
				mm.rotateX(obj->rot.x);

				shader.localParameterVertex(4,mm.column(0));
				shader.localParameterVertex(5,mm.column(1));
				shader.localParameterVertex(6,mm.column(2));

			}
				
			#endif

			shadReset=true;


		}else if(textureSpecular!=-1  && 
			hardware.fragmentProgram && 
			textureDisplacement==-1 &&
			(level->materials[obj->material].shader & MATERIAL_SHADER_SPECULAR) &&
			#ifdef FEATURE_SPECULAR
			true
			#else
			false
			#endif
			){

			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());

			glActiveTextureARB(GL_TEXTURE2_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,level->textures[textureSpecular].glName());

			if(transform){
				shader.set(specularFragmentShader);
				shader.set(specularVertexShader);
			}

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

			#ifndef SOFTWARE_TRANSFORMS
			if(transform){
				Matrix4d4d m;
				m.loadIdentity();
				m.translate(obj->pos.x,obj->pos.y,obj->pos.z);			
				m.rotateZ(obj->rot.z);
				m.rotateY(obj->rot.y);
				m.rotateX(obj->rot.x);
				m.scale(obj->scale.x,obj->scale.y,obj->scale.z);

				shader.localParameterVertex(0,m.column(0));
				shader.localParameterVertex(1,m.column(1));
				shader.localParameterVertex(2,m.column(2));
				shader.localParameterVertex(3,m.column(3));

				Matrix4d4d mm;
				mm.loadIdentity();
				mm.rotateZ(obj->rot.z);
				mm.rotateY(obj->rot.y);
				mm.rotateX(obj->rot.x);

				shader.localParameterVertex(4,mm.column(0));
				shader.localParameterVertex(5,mm.column(1));
				shader.localParameterVertex(6,mm.column(2));
			}
			#endif

			shader.localParameterFragment(0,level->materials[obj->material].shiny);

			shadReset=true;
			
		}else if(level->materials[obj->material].textureEnvironmentFront!=-1  && 
			conf->environmentMaps!="none"  && 
			(level->materials[obj->material].shader & MATERIAL_SHADER_ENVIRONMENT) &&
			#ifdef FEATURE_CUBIC_ENVIRONMENT_MAPS
			true
			#else
			false 
			#endif
			){

			assert(false);
			
			envReset=true;

			if(textureDiffuse!=-1){
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());

				glActiveTextureARB(GL_TEXTURE2_ARB);

			}else if(obj->lightmap==-1){

				glDisable(GL_LIGHTING);

				glColor4f(level->materials[obj->material].diffuse.x,
				level->materials[obj->material].diffuse.y,
				level->materials[obj->material].diffuse.z,
				level->materials[obj->material].diffuse.w);

				glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glActiveTextureARB(GL_TEXTURE3_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glActiveTextureARB(GL_TEXTURE4_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glActiveTextureARB(GL_TEXTURE0_ARB);

				envReset=true;
			}else{
				glActiveTextureARB(GL_TEXTURE2_ARB);
			}

			
			glBindTexture(GL_TEXTURE_CUBE_MAP_ARB,level->textures[level->materials[obj->material].textureEnvironmentFront].glName());	//all cube map sections have the same name
		
			glDisable(GL_TEXTURE_2D);
			glEnable(GL_TEXTURE_CUBE_MAP_ARB);

			shadReset=true;
			//need to undo all camera transforms to view space

		}else if(obj->envelopes.size()>0){
		
			shadReset=true;

			if(textureDiffuse!=-1){
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glEnable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());
				shadReset=true;
			}else{
				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
			}	

			if(obj->lightmap==-1){
				glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);
			}

			glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
			glActiveTextureARB(GL_TEXTURE3_ARB);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		}else{
			//plain boring nothing shader
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_CUBE_MAP_ARB);

			glColor4f(level->materials[obj->material].diffuse.x,
				level->materials[obj->material].diffuse.y,
				level->materials[obj->material].diffuse.z,
				level->materials[obj->material].diffuse.w);
			
			if(textureDiffuse!=-1){
				glColor4f(1,1,1,level->materials[obj->material].diffuse.w);	//textured objs always full bright
				glBindTexture(GL_TEXTURE_2D,level->textures[textureDiffuse].glName());
				shadReset=true;
			}

			if(obj->lightmap==-1){
				glDisable(GL_LIGHTING);

				glActiveTextureARB(GL_TEXTURE1_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_2D);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

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
			}
		}

	}

	
}

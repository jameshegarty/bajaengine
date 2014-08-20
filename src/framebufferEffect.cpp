#include "framebufferEffect.hpp"


#ifdef _WIN32
	#include <windows.h>	// Header File For Windows
#endif
	
#include "GlHeader.hpp"

#include "shader.hpp"
#include "texture.hpp"
#include "conf.h"
#include "level.hpp"
#include "Log.hpp"
#include <math.h>
#include "hardware.hpp"

ShaderId fbShader=SHADER_NONE;

int FBrBlurTexX;
int FBrBlurTexY;
unsigned int FBglRBlurBase;		//unfiltered

int FBlayer0;
int FBlayer1;
int FBlayer2;
int FBlayer3;
int FBlayer4;
int FBlayer5;
int FBlayer6;

void updateFBufferSize(){

	if(!hardware.fragmentProgram){
		return;

	}

	if((conf->sizeX>2048 || conf->sizeY>2048) && FBrBlurTexX<4096){
		logs().renderer.write("FB buffer resized to 4096");


		FBrBlurTexX=4096;
		FBrBlurTexY=4096;

		FBglRBlurBase=EmptyTexture(FBrBlurTexX,FBrBlurTexY);
	}else if((conf->sizeX>1024 || conf->sizeY>1024) && FBrBlurTexX<2048){
		logs().renderer.write("FB buffer resized to 2048");

		FBrBlurTexX=2048;
		FBrBlurTexY=2048;

		FBglRBlurBase=EmptyTexture(FBrBlurTexX,FBrBlurTexY);
	}else if( ((conf->sizeX<=1024 && conf->sizeY<=1024) && FBrBlurTexX>1024) || FBrBlurTexX==0){
		logs().renderer.write("FB buffer resized to 1024");

		FBrBlurTexX=1024;
		FBrBlurTexY=1024;

		FBglRBlurBase=EmptyTexture(FBrBlurTexX,FBrBlurTexY);
	}

}

void drawFramebufferEffect(){

	if(!hardware.fragmentProgram){
		return;

	}

	if(!conf->framebufferEffectOn){
		return;
	}

	updateFBufferSize();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glBindTexture(GL_TEXTURE_2D,FBglRBlurBase);

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	
	float bsly=(float)conf->sizeY/FBrBlurTexY;
	float bslx=(float)conf->sizeX/FBrBlurTexX;

	glDisable(GL_BLEND);

	#ifdef _WIN32
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	#else
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#endif
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);


	glMatrixMode (GL_TEXTURE);
		glLoadIdentity();

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();


	glOrtho(0,1,-1,0,-100,100);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();


	glDisable(GL_LIGHTING);


	glDisable(GL_BLEND);

	glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);

	glFrontFace( GL_CW );

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	glColor4f(1.0f,1.0f,1.0f,1.0f);

	if(conf->framebufferEffectFragmentProgram.size()>0){
		shader.reset();
		if(fbShader==SHADER_NONE){
			fbShader=shader.addFragmentFile(conf->framebufferEffectFragmentProgram);

			if(conf->framebufferEffectLayer1.size()>0){
				FBlayer1=texture.loadNoMip(conf->framebufferEffectLayer1);
			}

			if(conf->framebufferEffectLayer2.size()>0){
				FBlayer2=texture.loadNoMip(conf->framebufferEffectLayer2);
			}

			if(conf->framebufferEffectLayer3.size()>0){
				FBlayer3=texture.loadNoMip(conf->framebufferEffectLayer3);
			}

			if(conf->framebufferEffectLayer4.size()>0){
				FBlayer4=texture.loadNoMip(conf->framebufferEffectLayer4);
			}

			if(conf->framebufferEffectLayer5.size()>0){
				FBlayer5=texture.loadNoMip(conf->framebufferEffectLayer5);
			}

		}
		shader.set(fbShader);

		shader.localParameterFragment(0,conf->framebufferEffectParameter0);
		shader.localParameterFragment(1,conf->framebufferEffectParameter1);
		shader.localParameterFragment(2,conf->framebufferEffectParameter2);
		shader.localParameterFragment(3,conf->framebufferEffectParameter3);
		shader.localParameterFragment(4,conf->framebufferEffectParameter4);
	}
	
	
	glActiveTextureARB(GL_TEXTURE2_ARB);
	if(conf->framebufferEffectLayer2.size()>0){
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,FBlayer2);
	}else{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	}
	
	glActiveTextureARB(GL_TEXTURE3_ARB);
	if(conf->framebufferEffectLayer3.size()>0){
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,FBlayer3);
	}else{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	}
	
	glActiveTextureARB(GL_TEXTURE4_ARB);
	if(conf->framebufferEffectLayer4.size()>0){
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,FBlayer4);
	}else{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	}
	
	glActiveTextureARB(GL_TEXTURE5_ARB);
	if(conf->framebufferEffectLayer5.size()>0){
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,FBlayer5);
	}else{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	}
	
	glActiveTextureARB(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	
	glActiveTextureARB(GL_TEXTURE7_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	if(conf->framebufferEffectLayer1.size()>0){
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glBindTexture(GL_TEXTURE_2D,FBlayer1);
	}else{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	}
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_2D,FBglRBlurBase);
	
	
	float hbsly=bsly*0.75;
	float hbslx=bslx*0.25;
	
	// for passthrough: 
	hbsly=bsly;
	bsly=0;
	hbslx=1;
	
	
	
	glBegin(GL_QUADS);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,hbsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0); 
		glVertex3f(0,0,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,hbslx,hbsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,0); 
		glVertex3f(1,0,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,hbslx,bsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,1,1); 
		glVertex3f(1,-1,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,bsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,1); 
		glVertex3f(0,-1,0.0f);
	glEnd();
	
	shader.reset();
}


#ifdef FEATURE_GLOW

#include "glow.h"

#include "GlHeader.hpp"

#include "shader.hpp"
#include "texture.hpp"
#include "conf.h"
#include "level.hpp"
#include "Log.hpp"
#include <math.h>
#include "hardware.hpp"
#include "draw.hpp"

unsigned int glBlurBase;		//unfiltered
unsigned int glBlurBuffer;		//blured
ShaderId blurShader=SHADER_NONE;
ShaderId blurShaderF=SHADER_NONE;		
ShaderId blurShaderV=SHADER_NONE;		
ShaderId blurShaderH=SHADER_NONE;		
ShaderId blurExposure=SHADER_NONE;		
ShaderId fastblur=SHADER_NONE;	
ShaderId exposure=SHADER_NONE;	
ShaderId blurT;
 
float glowRescaleFac=0.25;
int blurTexX;
int blurTexY;
int blurBlurTexX;
int blurBlurTexY;

/* For Best Results:
use GL_CLAMP_TO_EDGE_EXT, and set it RIGHT BEFORE CALLING copytex
*/

#define GLOW_GLSL

bool glowInit(){

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return true;
	}

	blurTexX=0;
	blurTexY=0;
	blurBlurTexX=0;
	blurBlurTexY=0;


	fastblur=shader.addFragmentFile("shaders/fastblur.fp");
	exposure=shader.addFragmentFile("shaders/exposure.fp");

#ifdef GLOW_GLSL
	//blurShader=shader.addFragmentFile("shaders/glow.glf");
	//blurShaderF=shader.addFragmentFile("shaders/red.fp");
	//blurShader=shader.addVertexFile("shaders/glow.glv",blurShader);
#else
	blurShaderV=SHADER_NONE;
	blurShaderF=shader.addFragmentFile("hdrblur.fp");
	blurShaderH=shader.addFragmentFile("hdrblurh.fp");
	blurExposure=shader.addFragmentFile("hdrexposure.fp");
	blurT=shader.addFragmentFile("hdt.fp");
#endif

	return true;
}


void updateGlowBufferSize(){

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return;
	}


	if((conf->sizeX>2048 || conf->sizeY>2048) && conf->sizeX<=4096 && conf->sizeY<=4096 && (blurTexX!=4096)){
		logs().renderer.write("glow buffer resized to 4096");


		blurTexX=4096;
		blurTexY=4096;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
	}else if((conf->sizeX>1024 || conf->sizeY>1024) && conf->sizeX<=2048 && conf->sizeY<=2048 && (blurTexX!=2048)){
		logs().renderer.write("glow buffer resized to 2048");

		blurTexX=2048;
		blurTexY=2048;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
	}else if((conf->sizeX>512 || conf->sizeY>512) && conf->sizeX<=1024 && conf->sizeY<=1024 &&  (blurTexX!=1024)){
		logs().renderer.write("glow buffer resized to 1024");

		blurTexX=1024;
		blurTexY=1024;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
	}else if((conf->sizeX>256 || conf->sizeY>256) && conf->sizeX<=512 && conf->sizeY<=512 && (blurTexX!=512)){
		logs().renderer.write("glow buffer resized to 512");

		blurTexX=512;
		blurTexY=512;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
	}else if((conf->sizeX<=256 && conf->sizeY<=256) && (blurTexX!=256)){
		logs().renderer.write("glow buffer resized to 256");

		blurTexX=256;
		blurTexY=256;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
	}



	if((float(conf->sizeX)*glowRescaleFac>2048 || float(conf->sizeY)*glowRescaleFac>2048) && 
		float(conf->sizeX)*glowRescaleFac<=4096 && float(conf->sizeY)*glowRescaleFac<=4096 && (blurBlurTexX!=4096)){
		logs().renderer.write("glow blur buffer resized to 4096");


		blurBlurTexX=4096;
		blurBlurTexY=4096;

		glBlurBuffer=EmptyTexture(blurBlurTexX,blurBlurTexY);
	}else if((float(conf->sizeX)*glowRescaleFac>1024 || float(conf->sizeY)*glowRescaleFac>1024) && 
		float(conf->sizeX)*glowRescaleFac<=2048 && float(conf->sizeY)*glowRescaleFac<=2048 &&(blurBlurTexX!=2048)){
		logs().renderer.write("glow blur buffer resized to 2048");

		blurBlurTexX=2048;
		blurBlurTexY=2048;

		glBlurBuffer=EmptyTexture(blurTexX,blurTexY);
	}else if((float(conf->sizeX)*glowRescaleFac>512 || float(conf->sizeY)*glowRescaleFac>512) && 
		float(conf->sizeX)*glowRescaleFac<=1024 && float(conf->sizeY)*glowRescaleFac<=1024 && (blurBlurTexX!=1024)){
		logs().renderer.write("glow blur buffer resized to 1024");

		blurBlurTexX=1024;
		blurBlurTexY=1024;

		glBlurBuffer=EmptyTexture(blurBlurTexX,blurBlurTexY);
	}else if((float(conf->sizeX)*glowRescaleFac>256 || float(conf->sizeY)*glowRescaleFac>256) && 
		float(conf->sizeX)*glowRescaleFac<=512 && float(conf->sizeY)*glowRescaleFac<=512 && (blurBlurTexX!=512)){
		logs().renderer.write("glow blur buffer resized to 512");

		blurBlurTexX=512;
		blurBlurTexY=512;

		glBlurBuffer=EmptyTexture(blurBlurTexX,blurBlurTexY);
	}else if((float(conf->sizeX)*glowRescaleFac<=256 && float(conf->sizeY)*glowRescaleFac<=256) && (blurBlurTexX!=256)){
		logs().renderer.write("glow blur buffer resized to 256");

		blurBlurTexX=256;
		blurBlurTexY=256;

		glBlurBuffer=EmptyTexture(blurBlurTexX,blurBlurTexY);
	}
}


void updateGlowBufferSizeOLD(){

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return;
	}


	if((conf->sizeX>2048 || conf->sizeY>2048) && blurTexX<4096){
		logs().renderer.write("glow buffer resized to 4096");


		blurTexX=4096;
		blurTexY=4096;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
		glBlurBuffer=EmptyTexture(blurTexX,blurTexY);
	}else if((conf->sizeX>1024 || conf->sizeY>1024) && blurTexX<2048){
		logs().renderer.write("glow buffer resized to 2048");

		blurTexX=2048;
		blurTexY=2048;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
		glBlurBuffer=EmptyTexture(blurTexX,blurTexY);
	}else if((conf->sizeX<=1024 && conf->sizeY<=1024) && blurTexX>1024){
		logs().renderer.write("glow buffer resized to 1024");

		blurTexX=1024;
		blurTexY=1024;

		glBlurBase=EmptyTexture(blurTexX,blurTexY);
		glBlurBuffer=EmptyTexture(blurTexX,blurTexY);
	}

}


void doBlur(){

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return;
	}


	float bsly=(float)conf->sizeY/(float)blurTexY;
	float bslx=(float)conf->sizeX/(float)blurTexX;


	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);

	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	shader.set(blurShaderH);

	FloatVector4d t(conf->glowSize/40);
	shader.localParameterFragment(0,t);

	glBegin(GL_QUADS);
		glTexCoord2d(1,bsly); glVertex3f(1,1,0.0f);
		glTexCoord2d(1,0); glVertex3f(1,-1,0.0f);
		glTexCoord2d(0,0); glVertex3f(-1,-1,0.0f);
		glTexCoord2d(0,bsly); glVertex3f(-1,1,0.0f);

	glEnd();

	
	
	// draw the horizontal blur
	
	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.set(blurShaderH);

	shader.localParameterFragment(0,t);

	glBegin(GL_QUADS);
		glTexCoord2d(bslx,bsly); glVertex3f(1,1,0.0f);
		glTexCoord2d(bslx,0); glVertex3f(1,-1,0.0f);
		glTexCoord2d(0,0); glVertex3f(-1,-1,0.0f);
		glTexCoord2d(0,bsly); glVertex3f(-1,1,0.0f);

	glEnd();

	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);
	
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);


	shader.reset();

}


void doBlurNOSHADER(){
	/*square the alpha to do a bell curve*/

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return;
	}

	shader.reset();


	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);
	#ifdef _WIN32
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE_EXT);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE_EXT);
	#else
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	#endif
	
	glBindTexture(GL_TEXTURE_2D,glBlurBase);

	#ifdef _WIN32
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE_EXT);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE_EXT);
	#else
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	#endif

	float bsly=(float)conf->sizeY/blurTexY;

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int samples;

	samples=9*conf->glowSamples;

	if((samples%2)==0){
		samples+=1;
	}


	float alpha=1;
	float size=(conf->glowSize*0.05)/(float)samples;	//size per increment
	float start=-(samples-1)/2;
	float xoffset=start*size;

	for(int i=0; i<samples; i++){

		glColor4f(1.0f,1.0f,1.0f,alpha);

		//left
		glBegin(GL_QUADS);
			glTexCoord2d(0,bsly); glVertex3f(-1+xoffset,1,0.0f);
			glTexCoord2d(0,0); glVertex3f(-1+xoffset,-1,0.0f);
			glTexCoord2d(0,0); glVertex3f(-2+xoffset,-1,0.0f);
			glTexCoord2d(0,bsly); glVertex3f(-2+xoffset,1,0.0f);

			glTexCoord2d(1,bsly); glVertex3f(1+xoffset,1,0.0f);
			glTexCoord2d(1,0); glVertex3f(1+xoffset,-1,0.0f);
			glTexCoord2d(0,0); glVertex3f(-1+xoffset,-1,0.0f);
			glTexCoord2d(0,bsly); glVertex3f(-1+xoffset,1,0.0f);

			glTexCoord2d(1,bsly); glVertex3f(2+xoffset,1,0.0f);
			glTexCoord2d(1,0); glVertex3f(2+xoffset,-1,0.0f);
			glTexCoord2d(1,0); glVertex3f(1+xoffset,-1,0.0f);
			glTexCoord2d(1,bsly); glVertex3f(1+xoffset,1,0.0f);

		glEnd();

		xoffset+=size;
		alpha=(float)1/float(i+2);
		
	}

	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);	//should come before glCopyTexSubImage2D

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glDisable(GL_BLEND);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);	//should come after bind


	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);

	float yoffset=start*size;

	yoffset=0;

	alpha=1;


	for(int i=0; i<samples; i++){

		glColor4f(1.0f,1.0f,1.0f,alpha);

		glBegin(GL_QUADS);
			//top
			glTexCoord2d(1,bsly); glVertex3f(1,		2+yoffset,0.0f);
			glTexCoord2d(1,0); glVertex3f(1,		1+yoffset,0.0f);
			glTexCoord2d(0,0); glVertex3f(-1,		1+yoffset,0.0f);
			glTexCoord2d(0,bsly); glVertex3f(-1,	2+yoffset,0.0f);

			glTexCoord2d(1,bsly); glVertex3f(1,1+yoffset,0.0f);
			glTexCoord2d(1,0); glVertex3f(1,-1+yoffset,0.0f);
			glTexCoord2d(0,0); glVertex3f(-1,-1+yoffset,0.0f);
			glTexCoord2d(0,bsly); glVertex3f(-1,1+yoffset,0.0f);

			//bottom
			glTexCoord2d(1,bsly); glVertex3f(1,		-1+yoffset,0.0f);
			glTexCoord2d(1,0); glVertex3f(1,		-2+yoffset,0.0f);
			glTexCoord2d(0,0); glVertex3f(-1,		-2+yoffset,0.0f);
			glTexCoord2d(0,bsly); glVertex3f(-1,	-1+yoffset,0.0f);

		glEnd();

		yoffset+=size;
		alpha=(float)1/float(i+2);
		
	}

	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);	//should come before glCopyTexSubImage2D

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	glDisable(GL_BLEND);

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);	//DON'T FORGET TO DISABLE BLEND AND STUFF BEFORE CALLING111!11

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



void doBlurRESIZE(){
	/*square the alpha to do a bell curve*/

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return;
	}

	shader.reset();

#ifdef LOOP_DEBUG
	logs().main.write("doBlurRESIZE texture DISABLE");
#endif

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
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

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_BLEND);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glBindTexture(GL_TEXTURE_2D,glBlurBase);


	shader.set(fastblur);

	static float xBias=0;
	static float yBias=0;
	
	float sy=0.05*conf->glowSize;
	float sx=0.05*conf->glowSize*(float(conf->sizeY)/float(conf->sizeX));

	
	shader.localParameterFragment(1,sx,sy,0,0);

	float bsly=(float)conf->sizeY/blurTexY;
	float bslx=(float)conf->sizeX/blurTexX;

	if(conf->widescreen){
		Box2d t=level->camera->getViewport();
		t.nx/=conf->sizeX;
		t.px/=conf->sizeX;
		t.ny/=conf->sizeY;
		t.py/=conf->sizeY;

		xBias=-0.006;
		yBias=-0.006;
		shader.localParameterFragment(0,t.px*bslx+xBias,t.py*bsly+yBias,0,0);

		xBias=0.006;
		yBias=0.006;
		shader.localParameterFragment(2,t.nx*bslx+xBias,t.ny*bsly+yBias,0,0);

	}else{
		xBias=-0.01;
		yBias=-0.01;

		shader.localParameterFragment(0,bslx+xBias,bsly+yBias,0,0);

		shader.localParameterFragment(2,0,0,0,0);
	}

	glColor4f(1.0f,1.0f,1.0f,1.0f);
		glBegin(GL_QUADS);
			glTexCoord2d(0,		0);			glVertex3f(0,		-0.75,	0.0f);
			glTexCoord2d(bslx,		0);			glVertex3f(0.25,	-0.75,	0.0f);
			glTexCoord2d(bslx,		bsly);		glVertex3f(0.25,	-1.0,	0.0f);
			glTexCoord2d(0,		bsly);		glVertex3f(0,		-1.0,	0.0f);


		glEnd();


	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);	//should come before glCopyTexSubImage2D

	static float mxBias=0;
	static float myBias=0;

	float sizx=float(conf->sizeX)*glowRescaleFac+mxBias;
	float sizy=float(conf->sizeY)*glowRescaleFac+myBias;

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,sizx,sizy);	//should come after bind
	
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void drawGlow(){

	if(!hardware.fragmentProgram){	
		//still ok, just don't run
		return;
	}

	if(conf->glowExposure<=0){
		return;
	}

	updateGlowBufferSize();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);



	glBindTexture(GL_TEXTURE_2D,glBlurBase);

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	
	

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

	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);

	#ifdef _WIN32
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE_EXT);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE_EXT);
	#else
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	#endif
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);


	glMatrixMode (GL_TEXTURE);										// Select The Projection Matrix
	glLoadIdentity ();


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

	////////////////////////////////////////
	doBlurRESIZE();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_2D,glBlurBase);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);

	glDisable(GL_BLEND);


	glColor4f(1.0f,1.0f,1.0f,1.0f);


	shader.reset();
	

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

	glActiveTextureARB(GL_TEXTURE7_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D,glBlurBuffer);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D,glBlurBase);

	shader.set(exposure);

	if(conf->glowExposure<=0){
		shader.localParameterFragment(0,float(1));
	}else{

		FloatVector4d t(float(1)/(conf->glowExposure+1));
		t.w=0;

		shader.localParameterFragment(0,t);
	}




	float bsly=(float)conf->sizeY/blurTexY;
	float bslx=(float)conf->sizeX/blurTexX;

	float hbslx=((float)conf->sizeX*glowRescaleFac)/blurBlurTexX;
	float hbbly=((float)conf->sizeY*glowRescaleFac)/blurBlurTexY;
	float hbsly=0;
	
		glBegin(GL_QUADS);
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,hbsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,bsly); 
		glVertex3f(0,0,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,hbslx,hbsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,bslx,bsly); 
		glVertex3f(1,0,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,hbslx,hbbly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,bslx,0); 
		glVertex3f(1,-1,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,hbbly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0); 
		glVertex3f(0,-1,0.0f);
		glEnd();


	shader.reset();

	if(conf->widescreen){
		drawWidescreenPanel(20);
	}

}


#endif

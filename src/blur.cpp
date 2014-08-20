
#ifdef FEATURE_BLUR

#include "glow.h"

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

ShaderId fullblur;
int rBlurTexX;
int rBlurTexY;
unsigned int glRBlurBase;		//unfiltered
unsigned int glRBlurBuffer;		//filtered
ShaderId simpleblend;	

bool blurInit(){

	if(!hardware.fragmentProgram){
		return true;

	}

	rBlurTexX=1024;
	rBlurTexY=1024;

	glRBlurBase=EmptyTexture(rBlurTexX,rBlurTexY);
	glRBlurBuffer=EmptyTexture(rBlurTexX,rBlurTexY);

	fullblur=shader.addFragmentFile("shaders/fullblur.fp");
	simpleblend=shader.addFragmentFile("shaders/simpleblend.fp");

    return true;
}


void updateBlurBufferSize(){

	if(!hardware.fragmentProgram){
		return;

	}

	if((conf->sizeX>2048 || conf->sizeY>2048) && rBlurTexX<4096){
		logs().renderer.write("blur buffer resized to 4096");


		rBlurTexX=4096;
		rBlurTexY=4096;

		glRBlurBase=EmptyTexture(rBlurTexX,rBlurTexY);
		glRBlurBuffer=EmptyTexture(rBlurTexX,rBlurTexY);
	}else if((conf->sizeX>1024 || conf->sizeY>1024) && rBlurTexX<2048){
		logs().renderer.write("blur buffer resized to 2048");

		rBlurTexX=2048;
		rBlurTexY=2048;

		glRBlurBase=EmptyTexture(rBlurTexX,rBlurTexY);
		glRBlurBuffer=EmptyTexture(rBlurTexX,rBlurTexY);
	}else if((conf->sizeX<=1024 && conf->sizeY<=1024) && rBlurTexX>1024){
		logs().renderer.write("blur buffer resized to 1024");

		rBlurTexX=1024;
		rBlurTexY=1024;

		glRBlurBase=EmptyTexture(rBlurTexX,rBlurTexY);
		glRBlurBuffer=EmptyTexture(rBlurTexX,rBlurTexY);
	}

}


void doRBlurRESIZE(){
	/*square the alpha to do a bell curve*/

	if(!hardware.fragmentProgram){
		return;
	}

	shader.reset();

#ifdef LOOP_DEBUG
	logs().main.write("doRBlurRESIZE texture DISABLE");
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
	
	glBindTexture(GL_TEXTURE_2D,glRBlurBase);


	shader.set(fullblur);

	shader.localParameterFragment(0,conf->blurSize*0.01);

	float bsly=(float)conf->sizeY/rBlurTexY;
	float bslx=(float)conf->sizeX/rBlurTexX;

	glColor4f(1.0f,1.0f,1.0f,1.0f);


		glBegin(GL_QUADS);
			glTexCoord2d(0,		0);			glVertex3f(0,		0,	0.0f);
			glTexCoord2d(bslx,		0);			glVertex3f(0.25,	0,	0.0f);
			glTexCoord2d(bslx,		bsly);		glVertex3f(0.25,	-0.25,	0.0f);
			glTexCoord2d(0,		bsly);		glVertex3f(0,		-0.25,	0.0f);
		glEnd();


	glBindTexture(GL_TEXTURE_2D,glRBlurBuffer);	//should come before glCopyTexSubImage2D

	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0,conf->sizeX,conf->sizeY);	//should come after bind
}


void drawBlur(){

	if(!hardware.fragmentProgram){
		return;

	}

	if(!conf->blur){
		return;
	}

	if(conf->blurAlpha<=0){

		return;
	}

	updateBlurBufferSize();



	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glBindTexture(GL_TEXTURE_2D,glRBlurBase);
	
	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));
	
	float bsly=(float)conf->sizeY/rBlurTexY;
	float bslx=(float)conf->sizeX/rBlurTexX;

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

	doRBlurRESIZE();

	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_BLEND);

	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	shader.set(simpleblend);
	shader.localParameterFragment(0,conf->blurAlpha);
	
	
	
	
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
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_2D,glRBlurBuffer);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glBindTexture(GL_TEXTURE_2D,glRBlurBase);
	
	float hbsly=bsly*0.75;
	float hbslx=bslx*0.25;
	
	/* for passthrough: 
	 hbsly=bsly;
	 bsly=0;
	 hbslx=1;
	 */
	
	glBegin(GL_QUADS);
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,hbsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,bsly); 
		glVertex3f(0,0,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,hbslx,hbsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,bslx,bsly); 
		glVertex3f(1,0,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,hbslx,bsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,bslx,0); 
		glVertex3f(1,-1,0.0f);

		glMultiTexCoord2fARB(GL_TEXTURE0_ARB,0,bsly); 
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB,0,0); 
		glVertex3f(0,-1,0.0f);
	glEnd();
	
	shader.reset();
}



#endif


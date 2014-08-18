/*
 *  line.cpp
 *  xcode
 *
 *  Created by Zeus on 11/1/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "line2d.hpp"
#include "GlHeader.hpp"
#include "shader.hpp"
#include "conf.h"

Line2d::Line2d(){
	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;
	thickness=1;
	visible=true;
}


void Line2d::draw(){

	if(!visible){return;}


	glLineWidth(thickness);
	
	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glColor4f(color.x,color.y,color.z,color.w);
	
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

	glActiveTextureARB(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	shader.reset();
	
	FloatVector3d rpos=pos;
	FloatVector3d rscale=scale;

	if(percent && !keepAspectRatio){
		rpos.x*=conf->sizeX;
		rpos.y*=conf->sizeY;
		rscale.x*=conf->sizeX;
		rscale.y*=conf->sizeY;
	}else if (percent && keepAspectRatio){
		rpos.x*=conf->sizeX;
		rpos.y*=conf->sizeY;
		rscale.x*=conf->sizeX;
		rscale.y*=conf->sizeY;
	}

	glBegin(GL_LINES);
		glVertex3f(rpos.x,rpos.y,rpos.z);
		glVertex3f(rpos.x+rscale.x,rpos.y+rscale.y,rpos.z+rscale.z);
	glEnd();
}
/*
 *  point.cpp
 *  xcode
 *
 *  Created by Zeus on 11/1/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "point.h"
#include "GlHeader.hpp"
#include "shader.hpp"

Point3d::Point3d(){
	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;
	scale=1;
	visible=true;
}


void Point3d::draw(){
	glLineWidth(1);
	glPointSize(scale);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	
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
	
	glBegin(GL_POINTS);
		glVertex3f(pos.x,pos.y,pos.z);
	glEnd();

	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLint viewport[4];
	glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
	glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
	glGetIntegerv(GL_VIEWPORT,viewport);

}

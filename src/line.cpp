/*
 *  line.cpp
 *  xcode
 *
 *  Created by Zeus on 11/1/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "line.h"
#include "GlHeader.hpp"
#include "shader.hpp"

Line3d::Line3d(){
	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;
	thickness=1;
	visible=true;
}


void Line3d::draw(){
	glLineWidth(thickness);
	
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	
	glColor4f(color.x,color.y,color.z,color.w);
	
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

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	shader.reset();
	
	glBegin(GL_LINES);
		glVertex3f(pos.x,pos.y,pos.z);
		glVertex3f(pos.x+scale.x,pos.y+scale.y,pos.z+scale.z);
	glEnd();
}
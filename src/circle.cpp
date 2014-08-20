#include "circle.hpp"
#include "GlHeader.hpp"
#include "shader.hpp"
#include "conf.h"

#include "Helperlib/HelperLibMath.hpp"

Circle::Circle(){
	color.x=1;
	color.w=1;
	scale.x=10;
	scale.y=10;
	percent=false;
	keepAspectRatio=false;
	thickness=2;
	visible=true;
	scale.x=1;
	scale.y=1;
}

void Circle::draw(){
	if(!visible){return;}

	glLineWidth(thickness);
	
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

	int steps=20;
	float inc=(2*math.pi)/float(steps);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(rpos.x,rpos.y,rpos.z);
	glScalef(rscale.x,rscale.y,1.0f);

	glBegin(GL_LINES);

		for(int i=0; i<steps; i++){
			glVertex3f(math.cosRadians(i*inc),math.sinRadians(i*inc),1.0f);
			glVertex3f(math.cosRadians((i+1)*inc),math.sinRadians((i+1)*inc),1.0f);
		}

		glVertex3f(rpos.x,rpos.y,rpos.z);
		glVertex3f(rpos.x+rscale.x,rpos.y+rscale.y,rpos.z+rscale.z);
	glEnd();

	glPopMatrix();
}

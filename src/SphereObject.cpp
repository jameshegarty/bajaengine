#include "SphereObject.hpp"
#include "Helperlib/HelperLibMath.hpp"

#ifdef _WIN32
	#include <gl/gl.h>
	#include "gl/extensions.h"
#elif LINUX
	#include <gl.h>
#else
	#include <GLUT/glut.h>
#endif

Sphere::Sphere(){
	visible=true;
	detail.x=16;
	detail.y=16;

	scale.x=1;
	scale.y=1;
	scale.z=1;

	extent.x=360;
	extent.y=180;

	backface=true;
}

void Sphere::draw(){

	if(visible){
	
		#ifndef SOFTWARE_TRANSFORMS
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(pos.x,pos.y,pos.z);
			
	
	glRotatef(rot.z,0.0f,0.0f,1.0f);
	glRotatef(rot.y,0.0f,1.0f,0.0f);
	glRotatef(rot.x,1.0f,0.0f,0.0f);

	glScalef(scale.x,scale.y,scale.z);



#endif

	glColor4d(0,0,1,0.5f);

	float du=360/detail.x;
	float dv=180/detail.y;

	glBegin(GL_QUADS);

	for(int v=1; v<=detail.y; v++){

		float pr=0;
		float pz=0;

		pr=math.sinDegrees((v-1)*dv*(extent.y/180));
		pz=math.cosDegrees((v-1)*dv*(extent.y/180));


		float r=math.sinDegrees(v*dv*(extent.y/180));
		float z=math.cosDegrees(v*dv*(extent.y/180));

		for(int u=1; u<=detail.x; u++){
			FloatVector2d pext;
			FloatVector2d ext;

			pext.x=math.cosDegrees((u-1)*du*(extent.x/360));
			pext.y=math.sinDegrees((u-1)*du*(extent.x/360));

			ext.x=math.cosDegrees(u*du*(extent.x/360));
			ext.y=math.sinDegrees(u*du*(extent.x/360));

			
			
			
			glVertex3f(pext.x*pr,pz,pext.y*pr);
			glVertex3f(ext.x*pr,pz,ext.y*pr);
			glVertex3f(ext.x*r,z,ext.y*r);
			glVertex3f(pext.x*r,z,pext.y*r);

			if(backface){
				glVertex3f(pext.x*r,z,pext.y*r);
				glVertex3f(ext.x*r,z,ext.y*r);
				glVertex3f(ext.x*pr,pz,ext.y*pr);
				glVertex3f(pext.x*pr,pz,pext.y*pr);
			}
		}
	}

	glEnd();

	#ifndef SOFTWARE_TRANSFORMS
	glPopMatrix();
#endif
	}

}
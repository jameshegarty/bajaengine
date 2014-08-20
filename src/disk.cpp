#include "disk.hpp"
#include "HelperLibMath.hpp"
#include "GlHeader.hpp"


Disk::Disk(){
	visible=true;
	detail.x=16;
	detail.y=16;

	scale.x=1;
	scale.y=1;
	scale.z=1;

	backface=true;

	extent.x=360;
	extent.y=180;
}

void Disk::draw(){

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

	glColor4d(0,0,0,1);
	glColor4d(0,0,0,0.5f);

	float dv=360/detail.x;

	glBegin(GL_TRIANGLES);

	for(int v=1; v<=detail.x; v++){

		float pr=0;
		float pz=0;

		pr=math.sinDegrees((v-1)*dv*(extent.x/360));
		pz=math.cosDegrees((v-1)*dv*(extent.x/360));

		float r=math.sinDegrees(v*dv*(extent.x/360));
		float z=math.cosDegrees(v*dv*(extent.x/360));
			
		glVertex3f(0,0,0);
		glVertex3f(z,0,r);
		glVertex3f(pz,0,pr);
	
		if(backface){
			glVertex3f(pz,0,pr);
			glVertex3f(z,0,r);
			glVertex3f(0,0,0);
		}
	}

	glEnd();

	#ifndef SOFTWARE_TRANSFORMS
	glPopMatrix();
#endif
	}

}

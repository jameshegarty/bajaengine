#include "camera.hpp"


#ifdef _WIN32
#include <windows.h>	// Header File For Windows
#endif

#include "GlHeader.hpp"

#include "conf.h"
#include "keyboard.h"
#include "mouse.h"

#include "timeing.hpp"
#include "HelperLibMath.hpp"

#include "level.hpp"

#include "script.hpp"

#include "physics.h"

#include <math.h>

#include <OVR.h>
extern ovrMatrix4f riftProjection[2];
using namespace OVR;

void Camera::projectionMatrix(){


	if(orthographic){
		if(conf->widescreen){
	
			if(centerOrtho){
				glOrtho(-fov,fov,-fov/conf->widescreenRatio,fov/conf->widescreenRatio,clipNear,clipFar);
			}else{
				glOrtho(0,fov,0,fov/conf->widescreenRatio,clipNear,clipFar);
			}
		}else{
			glOrtho(0,fov,0,fov/conf->widescreenRatio,clipNear,clipFar);
		}
	}else{
		if(conf->widescreen){
      //			gluPerspective (fov, conf->widescreenRatio,clipNear,clipFar);		
            glLoadTransposeMatrixf((float*)riftProjection[eye].M);
		}else{
			gluPerspective (fov, (float)(conf->sizeX)/(float)(conf->sizeY),clipNear,clipFar);	
		}
	}



}
const Vector3f RightVector(1.0f, 0.0f, 0.0f);
const Vector3f UpVector(0.0f, 1.0f, 0.0f);
const Vector3f ForwardVector(0.0f, 0.0f, -1.0f); // -1 because HMD looks along -Z at identity orientation

Posef VirtualWorldTransformfromRealPose(const Posef &sensorHeadPose)
{
  //  Quatf baseQ = Quatf(Vector3f(0,1,0), BodyYaw.Get());
  Quatf baseQ = Quatf(Vector3f(0,1,0), level->camera->rot.y/57.29f);

  //  return Posef(baseQ * sensorHeadPose.Rotation,
  //               BodyPos + baseQ.Rotate(sensorHeadPose.Translation));

  return Posef(baseQ * sensorHeadPose.Rotation,
               Vector3f(level->camera->pos.x,level->camera->pos.y, level->camera->pos.z) + baseQ.Rotate(sensorHeadPose.Translation));
}

Matrix4f CalculateViewFromPose(const Posef& pose)
{
  Posef worldPose = VirtualWorldTransformfromRealPose(pose);

  // Rotate and position View Camera
  Vector3f up      = worldPose.Rotation.Rotate(UpVector);
  Vector3f forward = worldPose.Rotation.Rotate(ForwardVector);

  // Transform the position of the center eye in the real world (i.e. sitting in your chair)
  // into the frame of the player's virtual body.

  // It is important to have head movement in scale with IPD.
  // If you shrink one, you should also shrink the other.
  // So with zero IPD (i.e. everything at infinity),
  // head movement should also be zero.
  //  Vector3f viewPos = ForceZeroHeadMovement ? ThePlayer.BodyPos : worldPose.Translation;
  Vector3f viewPos = worldPose.Translation;

  Matrix4f view = Matrix4f::LookAtRH(viewPos, viewPos + forward, up);
  return view;
}

void Camera::adjust(){


	if(conf->widescreen){
		float py=conf->sizeY-(conf->sizeX*(1/conf->widescreenRatio));
		py=py/(float)2;

		float sy=conf->sizeX*(1/conf->widescreenRatio);
		
		float sx=conf->sizeX;

		float px=level->viewportPos.x*conf->sizeX;

		if(conf->sizeX >  conf->sizeY*conf->widescreenRatio){
			sy=conf->sizeY;
			sx=conf->sizeY*conf->widescreenRatio;
			py=0;
			px=(conf->sizeX-sx)/(float)2;
		}

    //		glViewport (px, (GLsizei)(py), (GLsizei)(sx), (GLsizei)(sy));				// Reset The Current Viewport (x,y,width,height)
	}else{

    //		glViewport (level->viewportPos.x*conf->sizeX, 
    //					-level->viewportPos.y*conf->sizeY, 
    //					(GLsizei)(conf->sizeX*level->viewportScale.x), 
    //					(GLsizei)(conf->sizeY*level->viewportScale.y));

	}

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();													// Reset The Projection Matrix

	if(orthographic){
		if(conf->widescreen){
	
			if(centerOrtho){
				glOrtho(-fov,fov,-fov/conf->widescreenRatio,fov/conf->widescreenRatio,clipNear,clipFar);
			}else{
				glOrtho(0,fov,0,fov/conf->widescreenRatio,clipNear,clipFar);
			}
		}else{
			glOrtho(0,fov,0,fov/conf->widescreenRatio,clipNear,clipFar);
		}
	}else{
		if(conf->widescreen){
      /*float M[4][4];
      for(int i=0; i<4; i++){
        for(int j=0; j<4; j++){
          M[i][j] = riftProjection[eye].M[j][i];
        }
        }*/
            glLoadTransposeMatrixf((float*)riftProjection[eye].M);
      //      glLoadMatrixf((float*)M);
      //            			gluPerspective (fov, conf->widescreenRatio,clipNear,clipFar);		
      //      printf("HERE\n");
		}else{

			gluPerspective (fov, (float)(conf->sizeX)/(float)(conf->sizeY),clipNear,clipFar);
		}
	}
    
  /*  float d =0.5;
    if(eye==0){
            glTranslatef(d,0.f,0.f);
            printf("E1\n");
                  }else{
            glTranslatef(-d,0.f,0.f);
            printf("E2\n");

            }*/
					
	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix
	glLoadIdentity ();		

}
extern ovrPosef            EyeRenderPose[2]; 

void Camera::move(){
	float changeZ=0;
	float pspeed=25*speed;

	float rotateSpeed=0.5;
	rotateSpeed=1;

	float strife=0;
	FloatVector3d newPos;

	if(console().visible){
		pspeed=0;
	}

	if(keyboard.shift){
		pspeed=50*speed;
	}
	
	#ifdef DEVELOPER
		if(keyboard.ctrl){
			pspeed=500*speed;
		}
	#endif

	if(keyboard.arrow_down || mouse.right()){
		changeZ-=engineTime.time*pspeed*0.01;
	}

	if(keyboard.arrow_up || mouse.left()){
		changeZ+=engineTime.time*pspeed*0.01;
	}

	if(!freeze){
		if(keyboard.arrow_right){
			rot.y-=engineTime.time*3*rotateSpeed;
		}else if(keyboard.arrow_left){
			rot.y+=engineTime.time*3*rotateSpeed;
		}else if(!freeze){
			rot.y-=mouse.change_x*0.1*rotateSpeed;
		}

		if(keyboard.page_down){
			rot.x-=engineTime.time*3*rotateSpeed;
		}else if(keyboard.page_up){
			rot.x+=engineTime.time*3*rotateSpeed;
		}else if(!freeze){
			rot.x+=mouse.change_y*0.1*rotateSpeed;
		}
	}
	

	#ifdef DEVELOPER
	if(keyboard.home){
		newPos.y+=engineTime.time*pspeed*0.05;
	}

	if(keyboard.end){
		newPos.y-=engineTime.time*pspeed*0.05;
	}


	
	if(mouse.wheelUp){
		newPos.y+=engineTime.time*pspeed*0.05;
	}

	if(mouse.wheelDown){
		newPos.y-=engineTime.time*pspeed*0.05;
	}
	#endif

	newPos.x += (float) sin((-rot.y) * PI/180 ) * changeZ;
    newPos.z -= (float) cos((-rot.y) * PI/180 ) * changeZ;


	if(keyboard.comma){
		strife-=engineTime.time*pspeed*0.01;
	}

	if(keyboard.period){
		strife+=engineTime.time*pspeed*0.01;
	}

	newPos.x += (float) sin( (-rot.y+90) * PI/180 ) * strife;
    newPos.z -= (float) cos( (-rot.y+90) * PI/180 ) * strife;
	
	if(fly && !freeze){
		pos+=newPos;
	}else if(!freeze){
		gravityVelocityHack+=physics.gravity*(engineTime.time*0.05);

		newPos+=gravityVelocityHack;

		EngineTime().startTimer("collide");
		
		pos=GetPosition(pos,newPos);

		
		if(collided){
			gravityVelocityHack.x=0;
			gravityVelocityHack.y=0;
			gravityVelocityHack.z=0;
		}

		EngineTime().endTimer("collide");
	}

	if(limitXOn){
		if(rot.x>limitXMax){rot.x=limitXMax;}
		if(rot.x<limitXMin){rot.x=limitXMin;}
	}

	glLoadIdentity();

  /*  
  OVR::Posef pose = EyeRenderPose[eye];
  float yaw;
  float eyePitch;
  float eyeRoll;
  pose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &eyePitch, &eyeRoll);

  float rScale = 20.f;
  glTranslatef(-rScale*EyeRenderPose[eye].Position.x, -rScale*EyeRenderPose[eye].Position.y, -rScale*EyeRenderPose[eye].Position.z);

  //
  //
  glRotatef(-rot.z-eyeRoll*57.29f,	0,	0.0f,	1.0f);
  glRotatef(-rot.x-eyePitch*57.29f,	1.0f,	0,	0);
  glRotatef(-rot.y-yaw*57.29f,	0,	1.0f,	0);

  //	glRotatef(-eyeRoll*57.29f,	0,	0.0f,	1.0f);
  //  glRotatef(-eyePitch*57.29f,	1.0f,	0,	0);
  //  glRotatef(-yaw*57.29f,	0,	1.0f,	0);
	
  glTranslatef(-pos.x, -pos.y, -pos.z);
  */

	//glTranslatef(-pos.x-rScale*EyeRenderPose[eye].Position.x, -pos.y-rScale*EyeRenderPose[eye].Position.y, -pos.z-rScale*EyeRenderPose[eye].Position.z);

  Matrix4f view = CalculateViewFromPose(EyeRenderPose[eye]);
  glLoadTransposeMatrixf((float*)view.M);
}

void Camera::transform(){
	glRotatef(-rot.x,	1.0f,	0,	0);
	glRotatef(-rot.y,	0.0f,	1.0f,	0);
	glRotatef(-rot.z,	0.0f,	0.0f,	1.0f);
	
	glTranslatef(-pos.x, -pos.y, -pos.z);
}

void Camera::reverseTransform(){
	glTranslatef(pos.x, pos.y,pos.z);

	glRotatef(rot.z,	0.0f,	0.0f,	1.0f);
	glRotatef(rot.y,	0.0f,	1.0f,	0);
	glRotatef(rot.x,	1.0f,	0.0f,	0);
}


void Camera::reverseTransformRot(){
	glRotatef(rot.z,	0.0f,	0.0f,	1.0f);
	glRotatef(rot.y,	0.0f,	1.0f,	0);
	glRotatef(rot.x,	1.0f,	0.0f,	0);
}

void Camera::draw(){
	return;	
}

Camera::Camera(){
	fov=90;
	clipNear=1;
	clipFar=1000;

	ellipsoidRadius.x=1;
	ellipsoidRadius.y=8;
	ellipsoidRadius.z=1;

	fly=false;

	freezeFrustum=false;

	speed=1;
	
	freeze=false;

	limitXOn=false;
	limitXMax=360;
	limitXMin=0;

	collided=false;

	centerOrtho=false;
	orthographic=false;
}

void Camera::viewport(){
	if(conf->widescreen){
		float py=conf->sizeY-(conf->sizeX*(1/conf->widescreenRatio));
		py=py/(float)2;

		float sy=conf->sizeX*(1/conf->widescreenRatio);
		
		float sx=conf->sizeX;

		float px=0;

		if(conf->sizeX >  conf->sizeY*conf->widescreenRatio){
			sy=conf->sizeY;
			sx=conf->sizeY*conf->widescreenRatio;
			py=0;
			px=(conf->sizeX-sx)/(float)2;
		}

    //		glViewport (px, (GLsizei)(py), (GLsizei)(sx), (GLsizei)(sy));				// Reset The Current Viewport (x,y,width,height)
	}else{

    //		glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	}
}

Box2d Camera::getViewport(){
	if(conf->widescreen){
		float py=conf->sizeY-(conf->sizeX*(1/conf->widescreenRatio));
		py=py/(float)2;

		float sy=conf->sizeX*(1/conf->widescreenRatio);
		
		float sx=conf->sizeX;

		float px=0;

		if(conf->sizeX >  conf->sizeY*conf->widescreenRatio){
			sy=conf->sizeY;
			sx=conf->sizeY*conf->widescreenRatio;
			py=0;
			px=(conf->sizeX-sx)/(float)2;
		}

		Box2d t;
		t.nx=px;
		t.px=sx+px;
		t.ny=py;
		t.py=sy+py;

		return t;
	}else{
		Box2d t;
		t.nx=0;
		t.px=conf->sizeX;
		t.ny=0;
		t.py=conf->sizeY;
		return t;
	}
}

void Camera::write(std::ofstream& out){

}

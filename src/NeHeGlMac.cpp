/*
 *  NeHeGlMac.cpp
 *  xcode
 *
 *  Created by Zeus on 7/28/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#include "NeHeGlMac.h"
#include "Log.hpp"

#include "mouse.h"

#include <unistd.h>

extern "C"{
#include <GLUT/glut.h>
}

#include "conf.h"
#include "level.hpp"
#include "os.h"
#include "rsa.h"
#include "engine_security.h"
#include "md5.h"
#include "keyboard.h"
#include "editor.h"

#include "draw.hpp"
#include "reflect.h"

int nativeY;
#define MAC_HIDE_MOUSE 1
int firstFrameHack=0;

bool glutIsFullscreen=false;
int glutWindow=-1;
int glutX=0;
int glutY=0;
int glutBit=0;

#define VK_LEFT 247
#define VK_UP 248
#define VK_RIGHT 249
#define VK_DOWN 250
#define VK_INSERT 251
#define VK_HOME 252
#define VK_PRIOR 253
#define VK_NEXT 254
#define VK_SHIFT 255

void ChangeScreenResolution(int width, int height, int bitsPerPixel,bool fullscreen);

#include <OVR.h>
#include <OVR_CAPI_GL.h>

bool rift = true;

///ovrGLConfig cfg;
//ovrRenderAPIConfig cfg;
ovrGLTexture EyeTexture[2];
ovrEyeRenderDesc EyeRenderDesc[2];
ovrPosef            EyeRenderPose[2]; 
ovrFovPort eyesFov[2];
GLuint eyeTexBuffer[2];
GLuint eyeTex[2];
GLuint eyeDepth[2];
ovrHmd              Hmd;
ovrMatrix4f riftProjection[2];
//ovrPosef            EyeRenderPose[2];

ovrRenderAPIConfig Get_ovrRenderAPIConfig()
{
  ovrRenderAPIConfig result = ovrRenderAPIConfig();
  result.Header.API = ovrRenderAPI_OpenGL;
  result.Header.RTSize = OVR::Sizei(1920, 1080);
  result.Header.Multisample = 1;//Params.Multisample;
  return result;
}

ovrSizei recommenedTex0Size;
ovrSizei recommenedTex1Size;

void riftInit(){

  ovr_Initialize();

  Hmd = ovrHmd_Create(0);

  if (!Hmd)
    {
      // If we didn't detect an Hmd, create a simulated one for debugging.
      Hmd = ovrHmd_CreateDebug(ovrHmd_DK2);
      //      UsingDebugHmd = true;
      if (!Hmd)
        {   // Failed Hmd creation.
          logs().main.write("could not create simulated HMD");
          return;
        }
    }

  if(!Hmd){
    logs().main.write("Could not find HMD");
  }else{
    logs().main.write("Found HMD");
  }

  eyesFov[0] = Hmd->DefaultEyeFov[0];
  eyesFov[1] = Hmd->DefaultEyeFov[1];

  float DesiredPixelDensity = 1.f;

  recommenedTex0Size = ovrHmd_GetFovTextureSize(Hmd, ovrEye_Left,  eyesFov[0], DesiredPixelDensity);
  recommenedTex1Size = ovrHmd_GetFovTextureSize(Hmd, ovrEye_Right, eyesFov[1], DesiredPixelDensity);

  conf->renderTargetSizeX = recommenedTex0Size.w;
  conf->renderTargetSizeY = recommenedTex0Size.h;
  //  conf->widescreen=false;
  //  conf->sizeX = recommenedTex0Size.w;
  //  conf->sizeY = recommenedTex0Size.h;

  logs().main.write("size "+String(recommenedTex0Size.w)+" "+String(recommenedTex0Size.h));

  //cfg.OGL.Header.API         = ovrRenderAPI_OpenGL;
  //cfg.OGL.Header.RTSize      = OVR::Sizei(Hmd->Resolution.w, Hmd->Resolution.h);
  //cfg.OGL.Header.Multisample = 1;
  //cfg.OGL.Window             = window;
  //cfg.OGL.DC                 = dc;

   unsigned           distortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette;

   
   ovrRenderAPIConfig config         = Get_ovrRenderAPIConfig();
   //   cfg.Config = GetConfig();
  ovrBool result = ovrHmd_ConfigureRendering(Hmd, &config, distortionCaps,
                                             eyesFov, EyeRenderDesc);

  logs().main.write("eye fov "+String(EyeRenderDesc[0].Fov.UpTan)+" "+String(EyeRenderDesc[0].Fov.DownTan));

  ovrHmd_ConfigureTracking(Hmd, ovrTrackingCap_Orientation |
                           ovrTrackingCap_MagYawCorrection |
                           ovrTrackingCap_Position, 0);

  if(!(ovrHmd_GetTrackingState(Hmd, 0.0f).StatusFlags)){
      logs().main.write("no tracking");
    }

  if(!result){
    logs().main.write("configure failed");
  }

  for(int eye=0; eye<2; eye++){
    riftProjection[eye] = ovrMatrix4f_Projection( EyeRenderDesc[eye].Fov, level->camera->clipNear, level->camera->clipFar, true );

    glGenFramebuffers(1, &(eyeTexBuffer[eye]));
    glBindFramebuffer(GL_FRAMEBUFFER, eyeTexBuffer[eye]);
    
    glGenTextures(1, &(eyeTex[eye]));
 
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, eyeTex[eye]);
    
    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, recommenedTex0Size.w, recommenedTex0Size.h, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
 
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenRenderbuffers(1, &eyeDepth[eye]);
    glBindRenderbuffer(GL_RENDERBUFFER, eyeDepth[eye]);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, recommenedTex0Size.w, recommenedTex0Size.h);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eyeTex[eye], 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, eyeDepth[eye]);

    //    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    //    glDrawBuffers(1, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
      logs().main.write("ERROR CREATING RENDER TEXTURE");
    }

    OVR::Sizei RenderTargetSize(recommenedTex0Size.w, recommenedTex0Size.h);

    EyeTexture[eye].OGL.Header.API            = ovrRenderAPI_OpenGL;
    EyeTexture[eye].OGL.Header.TextureSize    = RenderTargetSize;
    EyeTexture[eye].OGL.Header.RenderViewport = OVR::Recti(RenderTargetSize);
    EyeTexture[eye].OGL.TexId                 = eyeTex[eye];

  }

  ovrHmd_DismissHSWDisplay(Hmd);

}

void Display(void){
	if(firstFrameHack==4){
		ChangeScreenResolution(conf->sizeXTEMP,conf->sizeYTEMP,conf->bitDepthTEMP,!conf->windowedTEMP);
		firstFrameHack++;
	}else if(firstFrameHack<4){
		firstFrameHack++;
	}
	
	update();      	 
  level->camera->move();

  if(rift){
    ovrFrameTiming hmdFrameTiming = ovrHmd_BeginFrame(Hmd, 0);
    for(int eye=0; eye<2; eye++){

      ovrEyeType eyet = Hmd->EyeRenderOrder[eye];
      EyeRenderPose[eye] = ovrHmd_GetEyePose(Hmd, eyet);

      //printf("DO E %d\n",eye);
      
      level->camera->eye = eye;
      
      glBindFramebuffer(GL_FRAMEBUFFER, eyeTexBuffer[eye]);
      glViewport(0,0,recommenedTex0Size.w,recommenedTex0Size.h);
      
      glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      
      

      level->camera->adjust();
      level->camera->transform();
      drawLevel();
      drawWaterBad();
      drawReflectiveObjectsBad();
      drawTransparent(0,true,true);
      //drawTransparent(level->water->pos.y,true,false);
      
      drawPanels();
      drawMouse();
      
      console().draw();
      }

      ovrHmd_EndFrame(Hmd, EyeRenderPose, (ovrTexture*)EyeTexture);
  //	glutSwapBuffers();
  }else{
    Draw ();
    glutSwapBuffers();
  }
}

void Animate(void){
	glutPostRedisplay();
	
}

void
Key(unsigned char key, int x, int y){
	keyboard.keyDownMessage[key]=true;
	
	if(glutGetModifiers()==GLUT_ACTIVE_SHIFT){
		keyboard.keyDownMessage[VK_SHIFT]=true;
	}
}

void KeyUp(unsigned char key, int x, int y){
	keyboard.keyUpMessage[key]=true;
	
	if(glutGetModifiers()!=GLUT_ACTIVE_SHIFT){
		keyboard.keyUpMessage[VK_SHIFT]=true;
	}
}



void Visible(int state){

}

void
Reshape(int width, int height){
	
	if(conf->windowed){
		conf->sizeX=width;
		conf->sizeY=height;
		conf->sizeXTEMP=width;
		conf->sizeYTEMP=height;
	}
}

void mouseEvent(int button, int state, int x, int y){
	mouse.messageX=x;
	mouse.messageY=y;
	
    mouse.messageLDown = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN));
	mouse.messageLUp = ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP));
	
	mouse.messageMDown = ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN));
	mouse.messageMUp = ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_UP));
	
	mouse.messageRDown = ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN));
	mouse.messageRUp = ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP));
				  
}

GLenum StrMode = GL_VENDOR;
GLboolean moving = GL_TRUE;

void
timingSelect(int value)
{
    switch(value)
    {
		case 1:
			StrMode = GL_VENDOR;
		break;
		case 2:
			StrMode = GL_RENDERER;
		break;
		case 3:
			StrMode = GL_VERSION;
		break;
		case 4:
			StrMode = GL_EXTENSIONS;
		break;
    }
}

void
menuSelect(int value)
{
    switch (value) {
    case 1:
        moving = GL_TRUE;
        glutIdleFunc(Animate);
        break;
    case 2:
        moving = GL_FALSE;
        glutIdleFunc(NULL);
        break;
    case 4:
        exit(0);
        break;
    }
}

void mFunc(int x, int y){
	mouse.messageX=x;
	mouse.messageY=y;
}

void pmFunc(int x, int y){
	mouse.messageX=x;
	mouse.messageY=y;
}

void specialFunc(int key, int x, int y){

	if(key==GLUT_KEY_UP){
			keyboard.keyDownMessage[VK_UP]=true;
	}else if(key==GLUT_KEY_DOWN){
			keyboard.keyDownMessage[VK_DOWN]=true;
	}else if(key==GLUT_KEY_LEFT){
			keyboard.keyDownMessage[VK_LEFT]=true;
	}else if(key==GLUT_KEY_RIGHT){
			keyboard.keyDownMessage[VK_RIGHT]=true;
	}else if(key==GLUT_KEY_PAGE_UP){
			keyboard.keyDownMessage[VK_PRIOR]=true;
	}else if(key==GLUT_KEY_PAGE_DOWN){
			keyboard.keyDownMessage[VK_NEXT]=true;
	}else if(key==GLUT_KEY_INSERT){
			keyboard.keyDownMessage[VK_INSERT]=true;
	}else if(key==GLUT_KEY_HOME){
			keyboard.keyDownMessage[VK_HOME]=true;
	}
	
}

void specialUpFunc(int key, int x, int y){

	if(key==GLUT_KEY_UP){
			keyboard.keyUpMessage[VK_UP]=true;
	}else if(key==GLUT_KEY_DOWN){
			keyboard.keyUpMessage[VK_DOWN]=true;
	}else if(key==GLUT_KEY_LEFT){
			keyboard.keyUpMessage[VK_LEFT]=true;
	}else if(key==GLUT_KEY_RIGHT){
			keyboard.keyUpMessage[VK_RIGHT]=true;
	}else if(key==GLUT_KEY_PAGE_UP){
			keyboard.keyUpMessage[VK_PRIOR]=true;
	}else if(key==GLUT_KEY_PAGE_DOWN){
			keyboard.keyUpMessage[VK_NEXT]=true;
	}else if(key==GLUT_KEY_INSERT){
			keyboard.keyUpMessage[VK_INSERT]=true;
	}else if(key==GLUT_KEY_HOME){
			keyboard.keyUpMessage[VK_HOME]=true;
	}
	
}



void ChangeScreenResolution(int width, int height, int bitsPerPixel,bool fullscreen){
	glutReshapeWindow(width,height);
	CGDirectDisplayID main=CGMainDisplayID();


	if(fullscreen && !glutIsFullscreen){
		if(macStartMode==NULL){
			macStartMode=CGDisplayCurrentMode(main);
			CFNumberRef ch=(CFNumberRef)CFDictionaryGetValue(macStartMode,kCGDisplayHeight);
			CFNumberGetValue(ch,kCFNumberIntType,&nativeY);
		}
		
		if(macModes.size()==0){os().screenModes();}
		
		String key=String(width)+"X"+String(height)+"X"+String(bitsPerPixel);
		if(!macModes.find(key)){
			console().write("Error, could not find screen mode");
			return;
		}
		CGError e;
		CFDictionaryRef r=macModes[key];
		CGDisplayConfigRef config;
		e=CGBeginDisplayConfiguration(&config);
		e=CGConfigureDisplayFadeEffect(config,1.0f,1.0f,0.0f,0.0f,0.0f);
		CGConfigureDisplayMode(config,kCGDirectMainDisplay,r);
		e=CGCompleteDisplayConfiguration(config,kCGConfigureForAppOnly);
		CGDisplaySwitchToMode(main,r);
		glutFullScreen();
		
		glutIsFullscreen=true;
	}
	
	if(fullscreen && glutIsFullscreen){
		if(macStartMode==NULL){
			macStartMode=CGDisplayCurrentMode(main);
			CFNumberRef ch=(CFNumberRef)CFDictionaryGetValue(macStartMode,kCGDisplayHeight);
			CFNumberGetValue(ch,kCFNumberIntType,&nativeY);
		}
		
		if(macModes.size()==0){os().screenModes();}
		
		CGError e;
		
		CFDictionaryRef r=macModes[String(width)+"X"+String(height)+"X"+String(bitsPerPixel)];
		CGDisplayConfigRef config;
		e=CGBeginDisplayConfiguration(&config);
		e=CGConfigureDisplayFadeEffect(config,1.0f,1.0f,0.0f,0.0f,0.0f);
		CGConfigureDisplayMode(config,kCGDirectMainDisplay,r);
		e=CGCompleteDisplayConfiguration(config,kCGConfigureForAppOnly);
		CGDisplaySwitchToMode(main,r);
		glutFullScreen();
		
		glutIsFullscreen=true;
		
	}
	
	if(!fullscreen && glutIsFullscreen){
		CGError e;
		CGDisplayConfigRef config;
		e=CGBeginDisplayConfiguration(&config);
		e=CGConfigureDisplayFadeEffect(config,1.0f,1.0f,0.0f,0.0f,0.0f);
		CGConfigureDisplayMode(config,kCGDirectMainDisplay,macStartMode);
		e=CGCompleteDisplayConfiguration(config,kCGConfigureForAppOnly);
		CGDisplaySwitchToMode(main,macStartMode);

		glutIsFullscreen=false;
	}
}

#ifdef HEADLESS
int main (int argc, char * const argv[]) {
    if(argc!=2){
	std::cout << "Usage: engineHeadless script.lua" << std::endl;
	return 0;
  }	


	logs().main.write("start engine");

	os().executableFile=argv[0];

	level=new Level;
	loadedLevels.pushBack(level);

	conf=new Conf;

	conf->script=argv[1];

	if(EarlyInitialize()==false){
		std::cout << "Initialize Error" << std::endl;
		return 0;
	}

	Initialize();

	return 0;
}
#else

int main(int argc, char **argv){

	GLboolean fullscreen = GL_FALSE; 
	
 	srand(0);
	logs().main.write("start engine");

	os().executableFile=argv[0];

	os().executableDirectory=argv[0];
	
	#ifdef HEADLESS
		os().workingDirectory=argv[0];
	#else

		CFBundleRef main=CFBundleGetMainBundle ();
		CFURLRef x=CFBundleCopyResourcesDirectoryURL(main);
		
		CFStringRef r;
		x=CFURLCopyAbsoluteURL(x);
		r=CFURLCopyFileSystemPath(x,kCFURLPOSIXPathStyle);

		char pbuff[255];
		CFStringGetCString(r,pbuff,255,kCFStringEncodingASCII);
		os().workingDirectory=String(pbuff);
		
		os().workingDirectory+="/";
		
		String wd=os().workingDirectory;
		
	#endif
	
	#ifdef TRIAL
		os().error("NOTE: this engine is a TRIAL version.  It is not for distribution.");
	#endif


	#ifdef RSA
		bool showKeyError=true;


		for(int i=0; i<RSA_KEY_BYTES; i++){
			if(user_public_modulus[i]!=0x02){

				showKeyError=false;
			}
		}

		for(int i=0; i<RSA_KEY_BYTES; i++){
			if(user_modulus_checksum[i]!=0x01){

				showKeyError=false;
			}
		}

		//first check: has this exe been signed at all?
		if(showKeyError){
			os().error("ERROR: this exe has not been signed.  It must be signed with signengine.exe");
			return 0;
		}

		//next: check if the public_modulus is valid
		String hexCheckStr;

		for(int i=0; i<RSA_KEY_BYTES; i++){
			hexCheckStr+=dechex(user_public_modulus[i]);
			if(i<(RSA_KEY_BYTES-1)){
				hexCheckStr+=":";
			}
		}

		char* hex=new char[33];

		MD5 md5checksum((unsigned char*)hexCheckStr.c_str(),hexCheckStr.size());
		hex=md5checksum.hex_digest();


		unsigned char* result;
		unsigned char public_exponent[RSA_KEY_BYTES];	//[]={'0xFF','0xFF','0x01'};

		for(int i=0; i<(RSA_KEY_BYTES-3); i++){public_exponent[i]=0;}
		public_exponent[RSA_KEY_BYTES-3]=0x01;	//65536
		public_exponent[RSA_KEY_BYTES-2]=0x00;	//0
		public_exponent[RSA_KEY_BYTES-1]=0x01;	//1

		int size=rsa_verify(&result,user_modulus_checksum,(unsigned char*)&public_exponent,release_modulus,RSA_KEY_BYTES);

		if(size!=32){
			os().error("ERROR: bad engine key");
			return 0;
		}

		showKeyError=false;

		for(int i=0; i<32; i++){
			if(result[i]!=hex[i]){

				showKeyError=true;
			}
		}
	
		if(showKeyError){
			os().error("ERROR: bad engine key");
			return 0;
		}

	#endif
	
	conf=new Conf;
	conf->windowed=true;

	bool wmode=false;
	bool hmode=false;
	
	String levscr="";
	
	for(int i=0; i<argc; i++){

		if(String(argv[i])==String("-w")){		
			wmode=true;
		}else if(String(argv[i])==String("-h")){
			hmode=true;
		}else if(String(argv[i])==String("-fullscreen")){
			conf->windowed=false;
		}else{
			if(hmode){
				conf->sizeY=stringFunctions.toInt(String(argv[i]));
				conf->middleY=conf->sizeY/2;
				hmode=false;
			}else if(wmode){
				conf->sizeX=stringFunctions.toInt(String(argv[i]));
				conf->middleX=conf->sizeX/2;
				wmode=false;
			}else if(i==1){
				if(argv[i]!=NULL){
					levscr=argv[i];
				}
			}else if(i==0){
				//ignore, it's the program name

			}else{

			}
		}

	}

#ifdef DEVELOPER
	if(os().fileExists("EDITOR")){
		editor.show();
		editor.showProjects();
		conf->editor=true;
	}
#endif
	
	if(true){
		levscr="main.lua";
	}

	String type=levscr.substr(levscr.size()-3,levscr.size());

	if(!conf->editor){
		if(type=="lml"){
			conf->level=levscr;
		}else if(type=="lms"){
			conf->script=levscr;
		}else if(type=="lua"){
			conf->script=levscr;
		}else if(type=="luc"){
			conf->script=levscr;
		}
		
		if(type=="lml"){
			level=new Level;
			loadedLevels.pushBack(level);
			level->load(conf->level);
		}else{
			level=new Level;
			loadedLevels.pushBack(level);
		}
	}

	if(levscr==""){
		os().error("No Level or script file Set!");
		return 0;
	}else{

	}

	if(conf->editor){
		level=new Level;
		loadedLevels.pushBack(level);
		level->fog.color.x=0;
		level->fog.color.y=0;
		level->fog.color.z=0;
		conf->glow=false;
	}

	if (EarlyInitialize () == false){
		logs().main.write("initialize unsuccessful!");
		os().error("early init unsuccessful!");
		return 0;
	}else{
			
	}
			
			
	glutInit(&argc, argv);


	if (argc > 1 && !strcmp(argv[1], "-w")){
		fullscreen = GL_FALSE;
	}
	
	glutInitDisplayString("rgba double depth=24");

	//get stuff from script
	if(conf->sizeXTEMP!=conf->sizeX){
		conf->sizeX=conf->sizeXTEMP;
	}

	if(conf->sizeYTEMP!=conf->sizeY){
		conf->sizeY=conf->sizeYTEMP;
	}

	if(conf->bitDepthTEMP!=conf->bitDepth){
		conf->bitDepth=conf->bitDepthTEMP;
	}

	if(conf->windowedTEMP!=conf->windowed){
		conf->windowed=conf->windowedTEMP;
	}
	

	if(conf->windowed==false){
		CGDirectDisplayID mainD=CGMainDisplayID();
		
		
		if(macStartMode==NULL){
			
			macStartMode=CGDisplayCurrentMode(mainD);
			
			CFNumberRef ch=(CFNumberRef)CFDictionaryGetValue(macStartMode,kCGDisplayHeight);
			CFNumberGetValue(ch,kCFNumberIntType,&nativeY);
		}
		if(macModes.size()==0){os().screenModes();}
		
		CFDictionaryRef r=macModes[String(conf->sizeX)+"X"+String(conf->sizeY)+"X"+String(conf->bitDepth)];
		
		CGError e;
		CGDisplayConfigRef config;
		e=CGBeginDisplayConfiguration(&config);
		e=CGConfigureDisplayFadeEffect(config,1.0f,1.0f,0.0f,0.0f,0.0f);
		CGConfigureDisplayMode(config,kCGDirectMainDisplay,r);
		e=CGCompleteDisplayConfiguration(config,kCGConfigureForAppOnly);

		CGDisplaySwitchToMode(mainD,r);
	}
	
	 glutInitWindowSize(conf->sizeX, conf->sizeY);
	  glutWindow=glutCreateWindow(conf->title.c_str());
	  glutIsFullscreen=false;
	  
	  if(conf->windowed==false){
		firstFrameHack=true;
	  }
	
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Key);
	glutKeyboardUpFunc(KeyUp);
	
	glutMouseFunc(mouseEvent);
    glutIdleFunc(Animate);
    glutVisibilityFunc(Visible);
    glutMotionFunc(mFunc);
	glutPassiveMotionFunc(pmFunc);
	glutSpecialFunc(specialFunc);
	glutSpecialUpFunc(specialUpFunc);
	
	if (Initialize () == false)					// Call User Intialization
			{
				logs().main.write("initialize unsuccessful!");
				os().error("initialize unsuccessful!");
				return 0;
			}
			
	#ifdef MAC_HIDE_MOUSE
	glutSetCursor( GLUT_CURSOR_NONE);
	#endif
	

  if(rift){  	riftInit();	}
	glutPostRedisplay();
    glutMainLoop();
	
    return 0;             /* ANSI C requires main to return int. */
}

#endif

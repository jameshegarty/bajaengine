
//memory leak 

#include "GlHeader.hpp"

#include "mouse.h"
#include "keyboard.h"
#include <vector>
#include <string>
#include "timeing.hpp"
#include "level.hpp"
#include "camera.hpp"
#include "texture.hpp"
#include "os.h"
#include "conf.h"
#include "script.hpp"
#include "Text.hpp"
#include "engine.h"
#include "Audio.hpp"
#include "shader.hpp"
#include "glow.h"
#include "physics.h"
#include "draw.hpp"
#include "hardware.hpp"
#include "picking.h"
#include "imagetools.h"
#include "blur.h"
#include "reflect.h"
#include "Log.hpp"
#include "Tga.hpp"
#include "random.h"
#include "lightmap.h"
#include "SceneGraph.hpp"
#include "material.hpp"
#include "LuaGl.hpp"
#include "framebufferEffect.hpp"

#ifdef _WIN32
#include "NeHeGL.h"
#endif

using namespace std;

#ifndef CDS_FULLSCREEN											// CDS_FULLSCREEN Is Not Defined By Some
#define CDS_FULLSCREEN 4										// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors

#ifdef _WIN32
GL_Window*	g_window;
Keys*		g_keys;
#endif

// User Defined Variables
float		angle;												// Used For Rotation

bool		sp;													// Space Bar Pressed?
bool		env=true;											// Environment Mapping (Default On)
bool		ep;													// 'E' Pressed?
bool		bg=true;											// Background (Default On)
bool		bp;													// 'B' Pressed?

GLUquadricObj *quadratic;										// Storage For Our Quadratic Objects

float test,test1,test2;
extern  int tolua_test_open (lua_State* tolua_S);

bool scriptInit(){
	logs().main.write("START SCRIPT INIT");
	test=1;
	test1=2.4;
	test2=4.4;


	if(!engine.init()){	
		os().error("error: could not init engine");
			return false;
	}else{
		logs().main.write("engine init success");
	}

	if(!level->scriptInit()){	
		os().error("script error: could not init level");
			return false;
	}else{
		logs().main.write("level init success");
	}


	if(!waterScriptInit()){
		os().error("water script error: init error");
			return false;
	}else{
		logs().main.write("water init success");
	}
	

	if(!audio.scriptInit()){
		os().error("audio script error: init error");
			return false;
	}else{
		logs().main.write("audio init success");
	}
	
	if(!shader.scriptInit()){
		os().error("shader script error: init error");
			return false;
	}else{
		logs().main.write("shader init success");
	}

	#ifdef DEVELOPER
		if(!lightmap.scriptInit()){
			os().error("lightmap script error: init error");
				return false;
		}else{
			logs().main.write("lightmap init success");
		}
	#endif

	if(!mouse.scriptInit()){
		os().error("mouse script error: init error");
			return false;
	}else{
		logs().main.write("mouse init success");
	}

	if(!keyboard.scriptInit()){
		os().error("keyboard script error: init error");
			return false;
	}else{
		logs().main.write("keyboard init success");
	}

	if(!hardware.scriptInit()){
		os().error("hardware script error: init error");
			return false;
	}else{
		logs().main.write("hardware init success");
	}

	if(!level->sceneGraph.scriptInit()){
		os().error("scene graph script error: init error");
			return false;
	}else{
		logs().main.write("scene graph init success");
	}

	#ifdef DEVELOPER
		if(!editor.scriptInit()){
			os().error("editor script error: init error");
				return false;
		}else{
			logs().main.write("editor init success");
		}
	#endif

	if(!text.scriptInit()){
		os().error("text script error: init error");
			return false;
	}else{
		logs().main.write("text init success");
	}

	
	if(!material.scriptInit()){
		os().error("material script error: init error");
			return false;
	}else{
		logs().main.write("material init success");
	}

	if(!luaGl.scriptInit()){
		os().error("LuaGl script error: init error");
			return false;
	}else{
		logs().main.write("LuaGl init success");
	}


	if(!osScriptInit()){
		os().error("os script error: init error");
			return false;
	}

	if(!imageToolsScriptInit()){
		os().error("image tools script error: init error");
			return false;
	}else{
		logs().main.write("image tools init success");
	}


	//needs to be last(so that it can access all the stuff above)
	if(!conf->editor){
		if(!script().runFile(conf->script)){	
			os().error("script error: init error");
				return false;
		}else{
			logs().main.write("user script load success");
		}
	}else{
		script().run("function main() end");
	}

	return true;
}

bool EarlyInitialize (){
	if(!scriptInit()){
		os().error("script init error");
		return false;
	}else{
		logs().main.write("script init success");
	}

	hardware.minimumDebug();

	return true;
}

int trialWatermarkId=-1;
bool trialWatermarkInit=false;

void initTrialWatermark(){
	String triallmtpath="trial.lmt";

	if(os().fileExists("../tools/trial.lmt")){
		triallmtpath="../tools/trial.lmt";
	}else if(os().fileExists("../../tools/trial.lmt")){
		triallmtpath="../../tools/trial.lmt";
	}else{
		if(!os().fileExists("trial.lmt")){
			os().error("Error: trial.lmt not found!");
		}
	}

	ImageData im=tga.load(triallmtpath);
	TextureImage ti;
	ti.bytesPP=im.bytesPP;
	ti.data=im.data;
	ti.x=im.width;
	ti.y=im.height;

	texture.load(&ti);

	trialWatermarkId=ti.glName();
}

float trialWatermarkCountdown=0;
FloatVector2d trialWatermarkPos1;
FloatVector2d trialWatermarkPos2;

void annoyingTrialWatermark(){
	if(!trialWatermarkInit){
		initTrialWatermark();
		trialWatermarkInit=true;
	}

	trialWatermarkCountdown-=engineTime.time;

	if(trialWatermarkCountdown<0){
		trialWatermarkPos1.x=randomGen.IRandom(0,conf->sizeX-313);
		trialWatermarkPos1.y=-randomGen.IRandom(0,conf->sizeY-49);
		trialWatermarkPos2.x=randomGen.IRandom(0,conf->sizeX-313);
		trialWatermarkPos2.y=-randomGen.IRandom(0,conf->sizeY-49);
		trialWatermarkCountdown=300;
	}

	for(int i=0; i<2; i++){

		FloatVector3d color;
		FloatVector2d trialWatermarkPos;

		if(i==0){
			color.x=1;
			color.y=1;
			color.z=1;
			trialWatermarkPos=trialWatermarkPos1;
		}else{
			color.x=0;
			color.y=0;
			color.z=0;
			trialWatermarkPos=trialWatermarkPos2;
		}

		glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

		glMatrixMode(GL_TEXTURE);
			glLoadIdentity();

		glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
		glLoadIdentity ();

		glOrtho(0,conf->sizeX,-conf->sizeY,0,-100,100);

		glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix

		glLoadIdentity ();	
		

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);

		glDisable(GL_LIGHTING);

		glDisable(GL_CULL_FACE);

		

		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);

		shader.reset();


		glActiveTextureARB(GL_TEXTURE0_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE2_ARB);
		glDisable(GL_TEXTURE_CUBE_MAP_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE3_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE4_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE5_ARB);
		glDisable(GL_TEXTURE_2D);

		glActiveTextureARB(GL_TEXTURE0_ARB);

		float amb[4];

		amb[0]=1;
		amb[1]=1;
		amb[2]=1;
		amb[3]=1;

		glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);

		glDisable (GL_FOG);


		glColor4f(color.x,color.y,color.z,0.05);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,trialWatermarkId);


		float sx=313;
		float sy=49;
		
		float px=trialWatermarkPos.x;
		float py=trialWatermarkPos.y;
		float pz=0;

		glBegin(GL_QUADS);
			glTexCoord2d(0,0); glVertex3f(px		,py,pz);
			glTexCoord2d(1,0); glVertex3f(px+sx	,py,pz);
			glTexCoord2d(1,-1); glVertex3f(px+sx	,py-sy,pz);
			glTexCoord2d(0,-1); glVertex3f(px		,py-sy,pz);
		glEnd();
	}
}

bool developerWatermarkInit=false;
int developerWatermarkId=-1;


#ifdef _WIN32
bool Initialize (GL_Window* window, Keys* keys){
    // Any GL Init Code & User Initialiazation Goes Here
    				
	logs().main.write("START INITIALIZE");

	g_window	= window;
	g_keys		= keys;
#else
bool Initialize(){
#endif
	
	angle		= 0.0f;											// Set Starting Angle To Zero
	glGetIntegerv(GL_MAX_LIGHTS,(GLint*)&maxLights);
	glClearColor (0.0f, 0.0f, 0.0f, 0.5f);						// Black Background
	glClearDepth (1.0f);										// Depth Buffer Setup
	glDepthFunc (GL_LEQUAL);									// The Type Of Depth Testing (Less Or Equal)
	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glShadeModel (GL_SMOOTH);									// Select Smooth Shading
	glEnable(GL_LIGHTING);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Set Perspective Calculations To Most Accurate
	quadratic=gluNewQuadric();									// Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);					// Create Smooth Normals 
	gluQuadricTexture(quadratic, GL_TRUE);						// Create Texture Coords 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);	// GL_FRONT/
	
	#ifndef HEADLESS
	#ifdef _WIN32

	if(!extensions.GL_ARB_multitexture_supported){
		os().error("Gl Multitexture required!");
		return false;
	}
	
	#endif
	

	if(!hardware.init()){
		os().error("hardware error: init error");
		return false;
	}else{
		logs().main.write("hardware init success");
	}
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);

	#endif


	if(!text.init()){
		os().error("text error: init error");
			return false;
	}else{
		logs().main.write("text init success");
	}

	if(!audio.init()){
		logs().audio.write("audio error: init error");
		hardware.audio=false;
		//return false;	//we want the engine to still run, even if there's no audio hardware
	}else{
		logs().main.write("audio init success");
		hardware.audio=true;
	}


	if(!physics.init()){
		os().error("physics error: init error");
			return false;
	}else{
		logs().main.write("physics init success");
	}

	if(!waterInit()){
		os().error("water error: init error");
		return false;
	}else{
		logs().main.write("water init success");
	}

	#ifdef FEATURE_REFLECT
	if(!reflectiveObjectsInit()){
		os().error("reflective Objects error: init error");
			return false;
	}else{
		logs().main.write("reflective Objects init success");
	}
	#endif

	#ifndef HEADLESS
	#ifdef DEVELOPER
	if(!editor.init()){
		os().error("editor error: init error");
		return false;
	}else{
		logs().main.write("editor init success");
	}
	#endif
	#endif

	#ifdef FEATURE_GLOW
		if(!glowInit()){
			os().error("glow error: init error");
			return false;
		}else{
			logs().main.write("glow init success");
		}
	#endif

	#ifdef FEATURE_BLUR
		if(!blurInit()){
			os().error("blur error: init error");
			return false;
		}else{
			logs().main.write("blur init success");
		}
	#endif

	if(!drawInit()){
		os().error("draw init error");
		return false;
	}else{
		logs().main.write("draw init success");
	}

	if(!script().run("main()")){
		os().error("script error: error calling main function");
			return false;
	
	}else{
		logs().main.write("user script run success");
	}

	hardware.minimumDebug();

	#ifdef NO_MIP
		console().write("NOTE: built without mip map support");
	#endif


	logs().main.write("END INITIALIZE:SUCCESS");

	return true;												// Return TRUE (Initialization Successful)
}

void Deinitialize (void)										// Any User DeInitialization Goes Here
{
											// Delete The Device Dependant Bitmap Object
	//CloseAVI(0);													// Close The AVI File
}

void update(){		// Perform Motion Updates Here
	engineTime.startTimer("update");

#ifdef LOOP_DEBUG
	logs().main.write("STARTUPDATE");
#endif

	engineTime.perFrame();
	mouse.update();
	keyboard.update();

	engineTime.startTimer("audio");
	audio.perFrame();
	engineTime.endTimer("audio");


	updateTextureLevel();

	#ifdef LOAD_PROGRESSIVE
	if(!conf->pause && (!currentlyLoading)  
#ifdef DEVELOPER
		&& !editor.isVisible()
#endif
		){
	#else
	if(!conf->pause){
	#endif

		engineTime.clearTimer("syncto");
		engineTime.clearTimer("syncfrom");

		engineTime.startTimer("script");

		//run main perframe
		script().run("if perframe~=nil and type(perframe)=='function' then perframe() end");

		engineTime.endTimer("script");

		#ifdef LOOP_DEBUG
			logs().main.write("LEVELUPDATE");
		#endif

		level->update();

		
		physics.perframe();
	}else if(!conf->pause && (!currentlyLoading) ){
		engineTime.startTimer("script");
		engineTime.endTimer("script");

		level->update();
	}
	
	hardware.textureMemoryUsed;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
#ifdef LOOP_DEBUG
	logs().main.write("ENDUPDATE()");
#endif
	engineTime.endTimer("update");
}

float camerarotx;
float cameraroty;

float cameraposx;
float cameraposy;
float cameraposz;



void DrawInner();

// set up the current level's stencil mask
void doMask(){
	if(level->maskLevel.size()>0){
		Level *oldLevel=level;

		for(int i=0; i<loadedLevels.size(); i++){
			if(loadedLevels[i]->name==oldLevel->maskLevel){

				level=loadedLevels[i];
			}

		}

		//now render the mask level into the stencil buffer
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		
		glColorMask(0,0,0,0);
		glEnable(GL_STENCIL_TEST);						// Enable Stencil Buffer For "marking" The Floor
		glStencilFunc(GL_ALWAYS, 1, 1);						// Always Passes, 1 Bit Plane, 1 As Mask
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);				// We Set The Stencil Buffer To 1 Where We Draw Any Polygon

		glEnable(GL_DEPTH_TEST);	

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawInner();

		glEnable(GL_DEPTH_TEST);						// Enable Depth Testing
		glColorMask(1,1,1,1);							// Set Color Mask to TRUE, TRUE, TRUE, TRUE
		glStencilFunc(GL_EQUAL, 1, 1);						// We Draw Only Where The Stencil Is 1
										// (I.E. Where The Floor Was Drawn)
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);					// Don't Change The Stencil Buffer

		level=oldLevel;
	}

}

void postDoMask(){
	if(level->maskLevel.size()>0){
		glClearStencil(1);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_STENCIL_TEST);
		
	}
}

void DrawInner(){
	doMask();

	level->perFrame();

		engineTime.startTimer("picking");

#ifdef LOOP_DEBUG
	logs().main.write("PICKING");
#endif
		if(conf->pickerOn){
			doPicking();
		}
		

		engineTime.endTimer("picking");

#ifdef DEVELOPER
		editor.preCamera();
#endif

		level->camera->adjust();
		level->camera->move();

#ifdef DEVELOPER
		editor.postCamera();
#endif
		
		
		engineTime.endTimer("overhead");
		
		engineTime.startTimer("render");

		#ifdef LOOP_DEBUG
	logs().main.write("REFLECT");
#endif
		#ifdef FEATURE_REFLECT
			if(conf->water=="best" && hardware.fragmentProgram){
				drawReflect();
			}

			if(level->reflectiveObjects.size()>0 && hardware.fragmentProgram){
				drawReflectiveObjectsReflect();
			}
		#endif

			#ifdef LOOP_DEBUG
	logs().main.write("DRAWLEVEL");
#endif

		drawLevel();
		
		#ifdef DEVELOPER
			editor.draw();
		#endif

		//particles, yay!

		
		#ifdef FEATURE_REFLECT
			if(level->reflectiveObjects.size()>0  && hardware.fragmentProgram && conf->reflect=="best"){
				drawReflectiveObjectsBest();
			}else if(level->reflectiveObjects.size()>0 && conf->reflect=="bad"){
				drawReflectiveObjectsBad();
			}else{

			}
		#else
			if(level->reflectiveObjects.size()>0 && conf->reflect!="none"){
				drawReflectiveObjectsBad();
			}
		#endif

#ifdef LOOP_DEBUG
	logs().main.write("WATER");
#endif

		#ifdef FEATURE_WATER
			if(conf->water=="best"  && hardware.fragmentProgram){
				drawWaterBest();
			}else if(conf->water=="good" || conf->water=="bad" || (conf->water=="best"  && !hardware.fragmentProgram) ){
				drawWaterBad();
			}else{
				//none - default
			}
		#else
			if(conf->water!="none"){
				drawWaterBad();
			}
		#endif

#ifdef LOOP_DEBUG
	logs().main.write("TRANSPARENT");
#endif

		if(level->water==NULL){
			drawTransparent(0,true,true);
		}else{
			drawTransparent(level->water->pos.y,true,false);	//draw transparent stuff that's ABOVE the water
		}

#ifdef LOOP_DEBUG
		logs().main.write("LOOP: Draw::drawWidescreenPanel");
#endif
		drawWidescreenPanel(0);

#ifdef LOOP_DEBUG
		logs().main.write("LOOP: Draw::glowPanels");
#endif
		if(conf->glowPanels){
			drawPanels();
			//drawThumbnails();
		}

		engineTime.startTimer("glow");

#ifdef LOOP_DEBUG
		logs().main.write("BLURGLOW");
#endif

		#ifdef FEATURE_BLUR
			if(conf->blur){
				drawBlur();
			}
		#endif

		#ifdef FEATURE_GLOW
			if(conf->glow){
				drawGlow();
			}
		#endif

			if(conf->framebufferEffectOn){
				drawFramebufferEffect();
			}

		engineTime.endTimer("glow");

		if(!conf->glowPanels){
			drawPanels();
		}

		#ifdef LOOP_DEBUG
	logs().main.write("MOUSE");
#endif

		drawMouse();
		
		postDoMask();

		engineTime.endTimer("render");
}

void Draw(){
    // Draw Our Scene

#ifdef LOOP_DEBUG
	logs().main.write("STARTDRAW");
#endif

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Clear Screen And Depth Buffer
	
	if(!conf->pause){
#ifdef LOOP_DEBUG
	logs().main.write("LEVELPERFRAME");
#endif
		
		Level *oldLevel=level;

		for(int i=loadedLevels.size()-1; i>=0; i--){
			level=loadedLevels[i];

			if(level->visible){
				DrawInner();
			}
		}

		level=oldLevel;
	}

	engineTime.startTimer("overhead");

	
	#ifdef DEVELOPER
		engineTime.startTimer("console");
		editor.drawStatus();
		console().draw();
		developerWatermark();
		engineTime.endTimer("console");
	#endif

	#ifdef TRIAL
		annoyingTrialWatermark();
	#endif

	if(conf->pause){
		if(conf->pauseFps!=0){
			double a=float(1)/double(conf->pauseFps);
			a*=1000;
			a-=engineTime.frameTime;

			#ifdef _WIN32
			Sleep( a );
			#else
			a*=1000000;
			struct timespec ts;
			ts.tv_sec=0;
			ts.tv_nsec=a;
			
			nanosleep(&ts,NULL);
			#endif
		}
	}

	if(conf->limitFps!=0){
		double a=float(1)/double(conf->limitFps);
		a*=1000;
		a-=engineTime.frameTime;

		#ifdef _WIN32
		Sleep( a );
		#else
		a*=1000000;
		struct timespec ts;
		ts.tv_sec=0;
		ts.tv_nsec=a;
		
		nanosleep(&ts,NULL);
		#endif
	}

	#ifdef LOOP_DEBUG
		logs().main.write("ENDDRAW");
	#endif
	
	if(conf->framebufferSave.set()){
		unsigned char* data=new unsigned char[conf->sizeX*conf->sizeY*3];

		glReadPixels( 0,0,conf->sizeX,conf->sizeY,GL_RGB,GL_UNSIGNED_BYTE,data);

		ImageData img;
		img.data=data;
		img.width=conf->sizeX;
		img.height=conf->sizeY;
		img.size=conf->sizeX*conf->sizeY*3;
		img.bytesPP=3;
		img.bitsPP=3*8;

		saveImage(conf->framebufferSave,img);

		delete[] data;

		conf->framebufferSave.clear();
	}
}

#include "conf.h"

Conf* conf;

void Conf::resetTextureLevel(){
	oldTextureLevel=textureLevel;
}

bool Conf::textureLevelChanged(){

	if(oldTextureLevel!=textureLevel){
		return true;
	}

	return false;

}

void Conf::forceTextureLevelChanged(){
	oldTextureLevel=0;
}

Conf::Conf(){

	pause=false;
	pauseFps=10;
	limitFps=0;

	level="";
	editor=false;
	title="3d Application";

	sizeX=1024;
	sizeY=768;
	sizeXTEMP=1024;
	sizeYTEMP=768;

	middleX=512;
	middleY=384;

	bitDepth=32;
	bitDepthTEMP=32;

	logDroppedFrames=true;

	drawImplicit=false;
	normals=false;
	drawbb=false;
	drawbbDistance=0;

	sceneGraph=true;

	windowed=false;
	windowed=true;
	windowedTEMP=true;

	wireframe=true;
	wireframe=false;

	widescreen=true;

	widescreenRatio=(float)16/(float)9;

	water="best";

	reflect="best";
	
	environmentMaps="best";
	
	waterFoam=true;

	glow=true;
	glowExposure=5;	
	glowPanels=false;
	glowSize=1;	//1= really 0.01
	glowSamples=1;

	glowAlpha=1;

	blur=false;
	blurSize=1;
	blurAlpha=1;

	framebufferEffectOn=false;
	framebufferEffectFragmentProgram="";
	framebufferEffectLayer0="framebuffer";
	framebufferEffectLayer1="";
	framebufferEffectLayer2="";
	framebufferEffectLayer3="";

	minimumHardwareDebug=false;

	optimize="modern";  // none,list,modern

	
	displacement=true;

	sunColor.x=1;
	sunColor.y=1;
	sunColor.z=1;
	sunColor.w=1;

	textureLevel=0;		//ultra
	//textureLevel=1;	//high
	//rescaleTextures=2;	//med
	//textureLevel=3;	//low

	oldTextureLevel=textureLevel;

	renderer="static";
	
	pickerFar=80;
	pickerNear=0;
	pickerOn=false;
	pickerScaleX=10;
	pickerScaleY=10;
	pickerPosX=0;
	pickerPosY=0;

	version="2.7.5";

	loadProgressive=true;

	#if defined(PROFESSIONAL)
		edition="professional";
	#elif defined(INDIE)
		edition="indie";
	#elif defined(VIEWER)
		edition="viewer";
	#elif defined(TRIAL)
		edition="trial";
	#else

	#endif

	#ifdef DEVELOPER
		developer=true;
	#else
		developer=false;
	#endif

	frustumDebug=false;

  riftIpd = 0.5f;
  riftScale = 20.f;
}

bool Conf::load(){
	return true;
}

bool Conf::save(){
	return true;
}

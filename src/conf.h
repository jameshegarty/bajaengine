/*//for testing memory leaks http://msdn.microsoft.com/library/default.asp?url=/library/en-us/vsdebug/html/_core_Using_C_Run2dTime_Library_Debugging_Support.asp
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define LEAK_TEST 1
//END leaks*/

#ifndef CONF_H
#define CONF_H

#include "HelperLibString.hpp"
#include "Types.hpp"
#include "os.h"

class Conf{
private:
		int oldTextureLevel;

	public:
		bool textureLevelChanged();
		void resetTextureLevel();
		void forceTextureLevelChanged();

		Path framebufferSave;

		//window sizes
		int middleX;
		int middleY;
		int sizeX;
		int sizeY;

		int sizeXTEMP;
		int sizeYTEMP;

		int bitDepth;
		int bitDepthTEMP;

		FloatVector3d sunPos;
		FloatVector4d sunColor;

		bool logDroppedFrames;

		/*int sortIterations;
		int sortDepth;
		bool sortFast;*/

		bool windowed;
		bool windowedTEMP;

		bool wireframe;
		bool widescreen;
		float widescreenRatio;	//ie: 16/9

		bool glow;
		float glowExposure;
		float glowSize;		//1=2%=of screen
		float glowSamples;	//1=9 .5=5 2=19
		float glowAlpha;
		bool glowPanels;	//apply to panels?

		bool blur;
		float blurSize;
		float blurAlpha;

		bool framebufferEffectOn;
		String framebufferEffectFragmentProgram;
		String framebufferEffectLayer0;
		String framebufferEffectLayer1;
		String framebufferEffectLayer2;
		String framebufferEffectLayer3;
		String framebufferEffectLayer4;
		String framebufferEffectLayer5;
		String framebufferEffectLayer6;
		FloatVector4d framebufferEffectParameter0;
		FloatVector4d framebufferEffectParameter1;
		FloatVector4d framebufferEffectParameter2;
		FloatVector4d framebufferEffectParameter3;
		FloatVector4d framebufferEffectParameter4;
		FloatVector4d framebufferEffectParameter5;

		bool sceneGraph;

		bool normals;	//draw normals
		bool drawImplicit;	//draw nulls+chains
		bool drawbb;	//draw bounding boxes
		float drawbbDistance;
		String drawbbSolo;
		String normalsSolo;
		String water;	//"best"=reflect px shadr "good"=envmap px shadr "bad"=just texture "none"
		//FloatVector2d waterDirection;	//in lua global "water."
		bool waterFoam;

		String reflect;	//best, none
		String environmentMaps;
		String optimize;	//none,list,vbo
		String renderer;	//"static","dynamic","mixed"

		bool displacement;

		bool pause;
		float pauseFps;	//lock FPS during pause to a certain amt to save CPU, 0 for no lock
		float limitFps;

		bool minimumHardwareDebug;

		String level;
		String script;
		bool editor;	//editor mode
		String title;	//window title
		//String filename;	//"filename" aka filename.lua without the lua

		bool frustumDebug;

		bool pickerOn;
		float pickerNear;
		float pickerFar;
		float pickerPosX;
		float pickerPosY;
		float pickerScaleX;
		float pickerScaleY;
		String pickerDraw;

		String edition;
		bool developer;	//is this a developer build?
		String version;

		bool loadProgressive;

		int textureLevel;	//resize to find crappy cards, 0=textures are regular size, 1= half, 2=quarter
		Conf();
		bool load();
		bool save();
};

extern Conf* conf;

#endif

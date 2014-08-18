#include "engine.h"
#include "script.hpp"

#include "conf.h"
#include "timeing.hpp"
#include "picking.h"
#include "script.hpp"

#ifdef _WIN32
#include "nehegl.h"
#else
void ChangeScreenResolution(int width, int height, int bitsPerPixel,bool fullscreen);
#endif

#include "hardware.hpp"

Engine engine;

static int luaReloadScript(lua_State *L){
	script().reload();
	return 0;
}

static int luaEnginePause(lua_State *L){
	conf->pause=true;
	return 0;
}
static int luaEngineResume(lua_State *L){
	conf->pause=false;
	return 0;
}

static int luaScriptSync(lua_State *L){
	script().syncFrom(NULL);
	script().syncTo(NULL);

	return 0;
}

static int luaScriptSyncTo(lua_State *L){
	
	script().syncTo(NULL);

	return 0;
}

static int luaLeakDump(lua_State *L){
	
	
#ifdef CRTDBG_MAP_ALLOC  
	_CrtDumpMemoryLeaks();

#endif
	return 0;
}


static int luaScriptSyncFrom(lua_State *L){
	script().syncFrom(NULL);


	return 0;
}

static int luaScriptDisableSync(lua_State *L){
	script().disableSync();


	return 0;
}

static int luaScriptEnableSync(lua_State *L){
	script().enableSync();
	return 0;
}




static int luaEngineApplyScreenSettings(lua_State *L){
	script().syncFrom(NULL);

	if(conf->sizeX==conf->sizeXTEMP &&
		conf->sizeY==conf->sizeYTEMP &&
		conf->bitDepth==conf->bitDepthTEMP &&
		conf->windowed==conf->windowedTEMP){
		return 0;
	}

	ScreenMode fixed;
	fixed.x=conf->sizeXTEMP;
	fixed.y=conf->sizeYTEMP;
	fixed.bitDepth=conf->bitDepthTEMP;
	
	if(!conf->windowedTEMP){
		fixed=hardware.validateScreenMode(conf->sizeXTEMP,conf->sizeYTEMP,conf->bitDepthTEMP);
	}
	
	ChangeScreenResolution(fixed.x,fixed.y,fixed.bitDepth,!conf->windowedTEMP);
	
	conf->sizeX=fixed.x;
	conf->sizeY=fixed.y;
	conf->bitDepth=fixed.bitDepth;
	conf->windowed=conf->windowedTEMP;

	return 0;
}

static int luaSaveFramebuffer(lua_State *L){
	Array<LuaType> t=script().getArgs(L);

	if(t.size()!=1){

		console().write("saveframebuffer args error");
		return 0;
	}

	conf->framebufferSave.setRelative(t[0].value.ss);

	return 0;
}

String suck="stringsucks";

bool Engine::init(){

	script().add("time",&engineTime.time);
	script().add("fps",&engineTime.fps);
	script().add("frameTime",&engineTime.frameTime);


	LuaType* lluaa=script().add("engine");
	
		script().add("reloadScript",&luaReloadScript,lluaa);
		script().add("pause",&luaEnginePause,lluaa);
		script().add("resume",&luaEngineResume,lluaa);
		script().add("leakDump",&luaLeakDump,lluaa);
		script().add("sync",&luaScriptSync,lluaa);
		script().add("disableSync",&luaScriptDisableSync,lluaa);
		script().add("enableSync",&luaScriptEnableSync,lluaa);
		script().add("syncTo",&luaScriptSyncTo,lluaa);
		script().add("edition",&conf->edition,lluaa);
		script().add("version",&conf->version,lluaa);
		script().add("developer",&conf->developer,lluaa);
		script().add("syncFrom",&luaScriptSyncFrom,lluaa);
		script().add("title",&conf->title,lluaa);
		script().add("minimumHardwareDebug",&conf->minimumHardwareDebug,lluaa);
		script().add("sceneGraph",&conf->sceneGraph,lluaa);
		script().add("drawbbDistance",&conf->drawbbDistance,lluaa);
		script().add("drawbbSolo",&conf->drawbbSolo,lluaa);
		script().add("frustumDebug",&conf->frustumDebug,lluaa);
		script().add("optimize",&conf->optimize,lluaa);
		script().add("logDroppedFrames",&conf->logDroppedFrames,lluaa);
		script().add("loadProgressive",&conf->loadProgressive,lluaa);
		script().add("saveFramebuffer",luaSaveFramebuffer,lluaa);

		script().add("frame",&engineTime.frameCount,lluaa);

		LuaType* rews=script().add("renderer",lluaa);
			script().add("quality",&conf->water,rews);
			script().add("pipeline",&conf->renderer,rews);

		script().add("displacement",&conf->displacement,lluaa);

		script().add("water",&conf->water,lluaa);

		script().add("reflect",&conf->reflect,lluaa);
		script().add("environmentMaps",&conf->environmentMaps,lluaa);
		script().add("wireframe",&conf->wireframe,lluaa);
		script().add("drawbb",&conf->drawbb,lluaa);
		script().add("drawImplicit",&conf->drawImplicit,lluaa);
		script().add("normals",&conf->normals,lluaa);
		script().add("normalsSolo",&conf->normalsSolo,lluaa);


		script().add("textureLevel",&conf->textureLevel,lluaa);

		script().add("pauseFps",&conf->pauseFps,lluaa);
		script().add("limitFps",&conf->limitFps,lluaa);

		LuaType* ews=script().add("widescreen",lluaa);
			
			script().add("on",&conf->widescreen,ews);
			script().add("ratio",&conf->widescreenRatio,ews);

		LuaType* ewin=script().add("window",lluaa);
			script().add("apply",&luaEngineApplyScreenSettings,ewin);
			script().add("windowed",&conf->windowedTEMP,ewin);
			LuaType* ewins=script().add("size",ewin);
				script().add("x",&conf->sizeXTEMP,ewins);
				script().add("y",&conf->sizeYTEMP,ewins);
			script().add("bitDepth",&conf->bitDepthTEMP,ewin);


		LuaType* gl=script().add("glow",lluaa);
			
			script().add("on",&conf->glow,gl);
			script().add("exposure",&conf->glowExposure,gl);
			script().add("size",&conf->glowSize,gl);
			script().add("samples",&conf->glowSamples,gl);
			script().add("alpha",&conf->glowAlpha,gl);
			script().add("panels",&conf->glowPanels,gl);

		LuaType* bl=script().add("blur",lluaa);
			script().add("on",&conf->blur,bl);
			script().add("size",&conf->blurSize,bl);
			script().add("alpha",&conf->blurAlpha,bl);

		LuaType* fb=script().add("framebufferEffect",lluaa);
			script().add("on",&conf->framebufferEffectOn,fb);
			script().add("fragmentProgram",&conf->framebufferEffectFragmentProgram,fb);
			script().add("layer1",&conf->framebufferEffectLayer1,fb);
			script().add("layer2",&conf->framebufferEffectLayer2,fb);
			script().add("layer3",&conf->framebufferEffectLayer3,fb);
			script().add("layer4",&conf->framebufferEffectLayer4,fb);
			script().add("layer5",&conf->framebufferEffectLayer5,fb);
			script().add("layer6",&conf->framebufferEffectLayer6,fb);
			LuaType* p0=script().add("parameter0",fb);
				script().add("x",&conf->framebufferEffectParameter0.x,p0);
				script().add("y",&conf->framebufferEffectParameter0.y,p0);
				script().add("z",&conf->framebufferEffectParameter0.z,p0);
				script().add("w",&conf->framebufferEffectParameter0.w,p0);
			LuaType* p1=script().add("parameter1",fb);
				script().add("x",&conf->framebufferEffectParameter1.x,p1);
				script().add("y",&conf->framebufferEffectParameter1.y,p1);
				script().add("z",&conf->framebufferEffectParameter1.z,p1);
				script().add("w",&conf->framebufferEffectParameter1.w,p1);
			LuaType* p2=script().add("parameter2",fb);
				script().add("x",&conf->framebufferEffectParameter2.x,p2);
				script().add("y",&conf->framebufferEffectParameter2.y,p2);
				script().add("z",&conf->framebufferEffectParameter2.z,p2);
				script().add("w",&conf->framebufferEffectParameter2.w,p2);
			LuaType* p3=script().add("parameter3",fb);
				script().add("x",&conf->framebufferEffectParameter3.x,p3);
				script().add("y",&conf->framebufferEffectParameter3.y,p3);
				script().add("z",&conf->framebufferEffectParameter3.z,p3);
				script().add("w",&conf->framebufferEffectParameter3.w,p3);
			LuaType* p4=script().add("parameter4",fb);
				script().add("x",&conf->framebufferEffectParameter4.x,p4);
				script().add("y",&conf->framebufferEffectParameter4.y,p4);
				script().add("z",&conf->framebufferEffectParameter4.z,p4);
				script().add("w",&conf->framebufferEffectParameter4.w,p4);

		LuaType* s=script().add("sun",lluaa);
			
			LuaType* sp=script().add("pos",s);
			
				script().add("x",&conf->sunPos.x,sp);
				script().add("y",&conf->sunPos.y,sp);
				script().add("z",&conf->sunPos.z,sp);

			LuaType* sc=script().add("color",s);
			
				script().add("r",&conf->sunColor.x,sc);
				script().add("g",&conf->sunColor.y,sc);
				script().add("b",&conf->sunColor.z,sc);
				script().add("a",&conf->sunColor.w,sc);

		LuaType* p=script().add("picker",lluaa);

#ifdef LEGACY_PICKER
			script().add("debug",&glPicker.debug,p);
#endif

			LuaType* spp=script().add("pos",p);
			
				script().add("x",&conf->pickerPosX,spp);
				script().add("y",&conf->pickerPosY,spp);
			
			LuaType* sppp=script().add("scale",p);
			
				script().add("x",&conf->pickerScaleX,sppp);
				script().add("y",&conf->pickerScaleY,sppp);

			LuaType* sppc=script().add("clip",p);
			
				script().add("near",&conf->pickerNear,sppc);
				script().add("far",&conf->pickerFar,sppc);

			script().add("draw",&conf->pickerDraw,p);
			script().add("on",&conf->pickerOn,p);
			
		
	return true;
}

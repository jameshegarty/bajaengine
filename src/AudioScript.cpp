#include "common.h"
#include "engine.h"
#include "HelperLibMath.hpp"
#include "Log.hpp"
#include "level.hpp"
#include "Audio.hpp"
#include "timeing.hpp"
#include "hardware.hpp"

#include <al.h>
#include <alc.h>

#include "script.hpp"

#include "os.h"

#include "GlHeader.hpp"


static int luaPlay(lua_State *L){

	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()==0){
		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}

		}

	}else{
		console().write("audio error: bad arguments");
	}
	
	if(!idfound){
		return 0;
	}

	audio.sources[id]->play();

	return 0;
}


static int luaStop(lua_State *L){

	int id;

	Array<LuaType> args=script().getArgs(L);

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
			}

		}

	}else{
		console().write("audio error: bad arguments");
	}

	audio.sources[id]->stop();

	return 0;
}


static int luaPause(lua_State *L){

	int id;

	Array<LuaType> args=script().getArgs(L);

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
			}

		}

	}else{
		console().write("audio error: bad arguments");
	}
	
	audio.sources[id]->pause();

	return 0;
}


static int luaRewind(lua_State *L){

	int id;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()==0){
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
			}

		}

	}else{
		console().write("audio error: bad arguments");
	}

	audio.sources[id]->rewind();

	return 0;
}



static int luaOffset(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()==0){
		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}
		}

	}else{
		console().write("audio error: bad arguments");
		return 0;
	}

	if(!idfound){
		console().write("audio error(internal): bad id");
		return 0;
	}

	if(args.size()==1){
		float lol=audio.sources[id]->getOffset();
		script().result(L,lol);
		return 1;

	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){
		console().write("loop error: bad arguments, should be a number");
		return 0;
	}

	audio.sources[id]->setOffset(args[1].value.ds);

	return 0;
}


static int luaState(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}
		}

	}else{
		console().write("audio error: bad arguments");
		return 0;
	}

	if(!idfound){
		console().write("audio error(internal): bad id");
		return 0;
	}

	String lol=audio.sources[id]->getState();

	script().result(L,lol);

	return 1;

}

static int luaVolume(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()==0){
		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}
		}
	}else{
		console().write("audio error: bad arguments");
		return 0;
	}

	if(!idfound){
		console().write("audio error(internal): bad id");
		return 0;
	}

	if(args.size()==1){
		if(audio.sources.size()>id){
			script().result(L,audio.sources[id]->volume);

			return 1;
		}else{
			return 0;
		}
	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){

		console().write("loop error: bad arguments, should be a number");
		return 0;
	}

	if(audio.sources.size()>id){
		if(audio.sources[id]->fade>0){
			audio.sources[id]->volume=args[1].value.ds;
			audio.sources[id]->fadeVolumeTarget=args[1].value.ds;
			
			if(audio.sources[id]->fadeVolumeCurrent==-1){
				audio.sources[id]->fadeVolumeCurrent=audio.sources[id]->volume;
			}	//else, use the current fade volume

		}else{
			audio.sources[id]->volume=args[1].value.ds;
		}
	}

	return 0;
}



static int luaFade(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()==0){
		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}
		}
	}else{
		console().write("audio error: bad arguments");
		return 0;
	}

	if(!idfound){
		console().write("audio error(internal): bad id");
		return 0;
	}

	if(args.size()==1){
		script().result(L,audio.sources[id]->fade);

		return 1;

	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){

		console().write("loop error: bad arguments, should be a number");
		return 0;
	}

	audio.sources[id]->fade=args[1].value.ds;

	return 0;
}


static int luaPos(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=4  && args.size()!=1){
		console().write("audio error: bad arguments, must be accessed as a class, and (x,y,z)");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}
		}

	}else{
		console().write("audio error: bad arguments");
		return 0;
	}

	if(!idfound){
		console().write("audio error(internal): bad id");
		return 0;
	}

	if(args.size()==1){
		script().resultTable(L);
		script().result(L,"x",audio.sources[id]->pos.x);
		script().result(L,"y",audio.sources[id]->pos.y);
		script().result(L,"z",audio.sources[id]->pos.z);

		return 1;
	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){

		console().write("loop error: bad arguments, should be a number");
		return 0;
	}

	if(args[2].type!=SCRIPT_DOUBLE_STORED){

		console().write("loop error: bad arguments, should be a number");
		return 0;
	}

	if(args[3].type!=SCRIPT_DOUBLE_STORED){

		console().write("loop error: bad arguments, should be a number");
		return 0;
	}

	audio.sources[id]->pos.x=args[1].value.ds;
	audio.sources[id]->pos.y=args[2].value.ds;
	audio.sources[id]->pos.z=args[3].value.ds;

	return 0;
}



static int luaLoop(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2 && args.size()!=1){
		console().write("loop error: bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}

		}

	}else{
		console().write("loop error: bad arguments, must be accessed as a class");
	}

	if(!idfound){
		return 0;
	}


	if(args.size()==1){
		if(audio.sources.size()>id){
			script().result(L,audio.sources[id]->loop);
		}

		return 1;
	}else if(args[1].type==SCRIPT_BOOL_STORED){
		if(audio.sources.size()>id){
			audio.sources[id]->loop=args[1].value.bs;
		}
	}else{
		console().write("loop error: bad arguments");
	}

	return 0;
}


static int luaDirectional(lua_State *L){
	int id;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2 && args.size()!=1){
		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
			}
		}
	}else{
		console().write("audio error: bad arguments, must be accessed as a class");
		return 0;
	}

	if(args.size()==1){
		script().result(L,audio.sources[id]->directional);
		return 1;
	}

	if(args[1].type!=SCRIPT_BOOL_STORED){
		console().write("audio error: bad arguments, should be BOOL");
		return 0;
	}

	if(audio.sources[id]->stereo && args[1].value.bs){
		audio.sources[id]->directional=false;
		console().write("audio error ["+audio.sources[id]->filename.getRelative()+"]: stereo sound files can't be directional.");
	}else{
		audio.sources[id]->directional=args[1].value.bs;
	}

	return 0;
}

static int luaLength(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("loop error: bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}

		}

	}else{
		console().write("audio error: bad arguments");
		return 0;
	}
	
		script().result(L,audio.sources[id]->lengthSeconds);
		return 1;
	

}

static int luaRange(lua_State *L){
	int id;
	bool idfound=false;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2 && args.size()!=1){
		console().write("loop error: bad number of arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
				idfound=true;
			}

		}

	}else{
		console().write("audio error: bad arguments");
		return 0;
	}
	
	if(args.size()==1){
		script().result(L,audio.sources[id]->range);
		return 1;
	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){
		console().write("audio error: bad arguments, should be a number");
		return 0;
	}

	if(!idfound){
		console().write("audio error(internal): bad id");
		return 0;
	}

	audio.sources[id]->range=args[1].value.ds;



	return 0;
}


static int luaPitch(lua_State *L){
	int id;

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){

		console().write("audio error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="id"){
				id=args[0].children[i].value.ds;
			}
		}

	}else{
		console().write("audio error: bad arguments, must be accessed as a class");
		return 0;
	}

	if(args[1].type!=SCRIPT_DOUBLE_STORED){

		console().write("audio error: bad arguments, should be number");
		return 0;
	}

	audio.sources[id]->pitch=args[1].value.ds;

	return 0;
}

int luaListActive(lua_State *L){

	for(int i=0; i<audio.alSources.size(); i++){
		if(audio.alSourcePriority[i]>0){
			console().write(String(audio.alSourcePriority[i])+String(" ")+audio.sources[i]->filename.getRelative());
		}
	}	

	return 0;
}

int luaLoadAudio(lua_State *L){
	String s=luaL_checkstring(L,1);

	int id=audio.load(s);

	if(id!=-1){
		script().resultTable(L);
		script().result(L,"id",id);
		script().result(L,"play",luaPlay);
		script().result(L,"fade",luaFade);
		script().result(L,"offset",luaOffset);
		script().result(L,"pause",luaPause);
		script().result(L,"stop",luaStop);
		script().result(L,"rewind",luaRewind);
		script().result(L,"loop",luaLoop);
		script().result(L,"volume",luaVolume);
		script().result(L,"directional",luaDirectional);
		script().result(L,"range",luaRange);
		script().result(L,"pitch",luaPitch);
		script().result(L,"pos",luaPos);
		script().result(L,"length",luaLength);
		script().result(L,"state",luaState);
		return 1;
	}else{
		console().write("error loading");
		return 0;
	}
}

#include "level.hpp"

#include "animation.hpp"
#include "script.hpp"
#include "engine.h"
#include "timeing.hpp"
#include "Helperlib/HelperLibMath.hpp"
#include "SceneGraph.hpp"

Animation animation;

ObjectAnimation::ObjectAnimation(){
	loop=false;
	play=false;
	fps=30;
	frame=0;
	oldFrame=0;
	name="default";
	start=0;
	end=0;
	blend=1;
	oldBlend=1;
}

unsigned int ObjectAnimation::size(){
	unsigned int size=0;

	size+=sizeof(unsigned int);	//keycount
	size+=sizeof(type);
	size+=sizeof(fps);
	size+=sizeof(start);
	size+=sizeof(end);
	size+=variable.filesize();
	size+=name.filesize();

	for(int i=0; i<keys.size(); i++){
		size+=sizeof(float);
		size+=sizeof(float);

		if(type==2){
			size+=sizeof(float);
			size+=sizeof(float);
			size+=sizeof(float);
			size+=sizeof(float);

		}
	}

	return size;
}

bool ObjectAnimation::write(std::ofstream& out){
	unsigned int keycount=keys.size();
	out.write((char*)&keycount,sizeof(keycount));
	out.write((char*)&type,sizeof(type));
	out.write((char*)&fps,sizeof(fps));
	out.write((char*)&start,sizeof(start));
	out.write((char*)&end,sizeof(end));
	variable.write(&out);
	name.write(&out);

	for(int i=0; i<keys.size(); i++){
		out.write((char*)&keys[i].frame,sizeof(keys[i].frame));
		out.write((char*)&keys[i].value,sizeof(keys[i].value));

		if(type==2){
			out.write((char*)&keys[i].leftTanX,sizeof(keys[i].leftTanX));
			out.write((char*)&keys[i].leftTanY,sizeof(keys[i].leftTanY));
			out.write((char*)&keys[i].rightTanX,sizeof(keys[i].rightTanX));
			out.write((char*)&keys[i].rightTanY,sizeof(keys[i].rightTanY));

		}
	}

	return true;
}

#ifndef XSICONVERT 

static int luaAnimationPlay(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Animation Play Error: bad arguments");
		return 0;
	}

	if(SCRIPT_STRING_STORED==args[0].type){

		Object* obj=level->objectKey[args[0].value.ss];

		if(obj->flags & OBJECT_STATIC){
			console().write("error, object '"+obj->name+"' is static, so animation cannot be displayed");
			return 0;
		}

		for(int i=0; i<obj->animations.size();i++){
			obj->animations[i].play=true;
		}

	}

	return 0;
}

static int luaAnimationFps(lua_State *L){

	return 0;
}

static int luaAnimationLoop(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		return 0;
	}

	String obj;
	String ani;

	if(args[0].type==SCRIPT_STRING_STORED){
		obj=args[0].value.ss;	
	}else{
		console().write("anim argument error");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		ani=args[1].value.ss;	
	}else{
		console().write("anim argument error");
		return 0;
	}

	Array<ObjectAnimation>* oa;

	if(level->objectKey.find(obj)){

		oa=&level->objectKey[obj]->animations;	
	}else if(level->ikEffectorKey.find(obj)){
		
		oa=&level->ikEffectorKey[obj]->animations;	
	}else if(level->ikJointKey.find(obj)){

		oa=&level->ikJointKey[obj]->animations;	
	}else if(level->ikRootKey.find(obj)){

		oa=&level->ikRootKey[obj]->animations;	
	}else{

		console().write("anim obj not found");
		return 0;
	}

	for(int i=0; i<oa->size(); i++){
		if((*oa)[i].name==ani){
			script().result(L,bool((*oa)[i].loop));
			return 1;
		}
	}


	return 0;
}

static int luaAnimationBlend(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		return 0;
	}

	String obj;
	String ani;

	if(args[0].type==SCRIPT_STRING_STORED){
		obj=args[0].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		ani=args[1].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}


	Array<ObjectAnimation>* oa;

	if(level->objectKey.find(obj)){
		oa=&level->objectKey[obj]->animations;	
	}else if(level->ikEffectorKey.find(obj)){
		oa=&level->ikEffectorKey[obj]->animations;	
	}else if(level->ikJointKey.find(obj)){
		oa=&level->ikJointKey[obj]->animations;	
	}else if(level->ikRootKey.find(obj)){
		oa=&level->ikRootKey[obj]->animations;	
	}else{

		console().write("state err, obj not found");
		return 0;
	}

	for(int i=0; i<oa->size(); i++){
		if((*oa)[i].name==ani){
			script().result(L,(*oa)[i].blend);
			return 1;
		}
	}


	return 0;
}

static int luaAnimationFrame(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		return 0;
	}

	String obj;
	String ani;

	if(args[0].type==SCRIPT_STRING_STORED){
		obj=args[0].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		ani=args[1].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}


	Array<ObjectAnimation>* oa;

	if(level->objectKey.find(obj)){
		oa=&level->objectKey[obj]->animations;	
	}else if(level->ikEffectorKey.find(obj)){
		oa=&level->ikEffectorKey[obj]->animations;	
	}else if(level->ikJointKey.find(obj)){
		oa=&level->ikJointKey[obj]->animations;	
	}else if(level->ikRootKey.find(obj)){
		oa=&level->ikRootKey[obj]->animations;	
	}else{

		console().write("state err, obj not found");
		return 0;
	}

	for(int i=0; i<oa->size(); i++){
		if((*oa)[i].name==ani){
			script().result(L,(*oa)[i].frame);
			return 1;
		}
	}


	return 0;
}

static int luaAnimationPause(lua_State *L){

	return 0;
}

static int luaAnimationStartFrame(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		return 0;
	}

	String obj;
	String ani;

	if(args[0].type==SCRIPT_STRING_STORED){
		obj=args[0].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		ani=args[1].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}


	Array<ObjectAnimation>* oa;

	if(level->objectKey.find(obj)){
		oa=&level->objectKey[obj]->animations;	
	}else if(level->ikEffectorKey.find(obj)){
		oa=&level->ikEffectorKey[obj]->animations;	
	}else if(level->ikJointKey.find(obj)){
		oa=&level->ikJointKey[obj]->animations;	
	}else if(level->ikRootKey.find(obj)){
		oa=&level->ikRootKey[obj]->animations;	
	}else{

		console().write("state err, obj not found");
		return 0;
	}

	for(int i=0; i<oa->size(); i++){
		if((*oa)[i].name==ani){
			script().result(L,(*oa)[i].start);
			return 1;
		}
	}


	return 0;
}


static int luaAnimationEndFrame(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		return 0;
	}

	String obj;
	String ani;

	if(args[0].type==SCRIPT_STRING_STORED){
		obj=args[0].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		ani=args[1].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}


	Array<ObjectAnimation>* oa;

	if(level->objectKey.find(obj)){
		oa=&level->objectKey[obj]->animations;	
	}else if(level->ikEffectorKey.find(obj)){
		oa=&level->ikEffectorKey[obj]->animations;	
	}else if(level->ikJointKey.find(obj)){
		oa=&level->ikJointKey[obj]->animations;	
	}else if(level->ikRootKey.find(obj)){
		oa=&level->ikRootKey[obj]->animations;	
	}else{

		console().write("state err, obj not found");
		return 0;
	}

	for(int i=0; i<oa->size(); i++){
		if((*oa)[i].name==ani){
			script().result(L,(*oa)[i].end);
			return 1;
		}
	}


	return 0;
}

static int luaAnimationState(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		return 0;
	}

	String obj;
	String ani;

	if(args[0].type==SCRIPT_STRING_STORED){
		obj=args[0].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		ani=args[1].value.ss;	
	}else{
		console().write("state argument error");
		return 0;
	}


	Array<ObjectAnimation>* oa;

	if(level->objectKey.find(obj)){
		oa=&level->objectKey[obj]->animations;	
	}else if(level->ikEffectorKey.find(obj)){
		oa=&level->ikEffectorKey[obj]->animations;	
	}else if(level->ikJointKey.find(obj)){
		oa=&level->ikJointKey[obj]->animations;	
	}else if(level->ikRootKey.find(obj)){
		oa=&level->ikRootKey[obj]->animations;	
	}else{

		console().write("state err, obj not found");
		return 0;
	}

	for(int i=0; i<oa->size(); i++){
		if((*oa)[i].name==ani){

			if((*oa)[i].play){

				script().result(L,String("playing"));
				return 1;
			}else if((*oa)[i].frame!=0 && (*oa)[i].frame!=(*oa)[i].start){
				script().result(L,String("paused"));
				return 1;
			}else{
				script().result(L,String("stopped"));
				return 1;
			}
		}
	}


	return 0;
}

static int luaAnimationPlayAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()>1){
		console().write("Animation Play All Error: bad arguments");
		return 0;
	}

	String aname="default";

	if(args.size()==1){
		if(SCRIPT_STRING_STORED==args[0].type){
			aname=args[0].value.ss;
		}

	}

	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].play=true;
				level->objects[i]->animations[a].frame=level->objects[i]->animations[a].start;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){

				level->ikRoots[i]->animations[a].play=true;
				level->ikRoots[i]->animations[a].frame=level->ikRoots[i]->animations[a].start;
			}
		}
	}


	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].play=true;
				level->ikJoints[i]->animations[a].frame=level->ikJoints[i]->animations[a].start;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].play=true;
				level->ikEffectors[i]->animations[a].frame=level->ikEffectors[i]->animations[a].start;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].play=true;
				level->nulls[i]->animations[a].frame=level->nulls[i]->animations[a].start;
			}
		}
	}

	return 0;

}


static int luaAnimationPauseAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()>1){
		console().write("Animation Play All Error: bad arguments");
		return 0;
	}

	String aname="default";

	if(args.size()==1){
		if(args[0].type=SCRIPT_STRING_STORED){
			aname=args[0].value.ss;
		}

	}

	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].play=false;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){
				level->ikRoots[i]->animations[a].play=false;
			}
		}
	}


	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].play=false;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].play=false;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].play=false;
			}
		}
	}

	return 0;

}

static int luaAnimationRewind(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Animation Restart Error: bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_STRING_STORED){

		Object* obj=level->objectKey[args[0].value.ss];

		if(obj->flags & OBJECT_STATIC){
			console().write("error, object '"+obj->name+"' is static, so animation cannot be displayed");
			return 0;
		}

		for(int i=0; i<obj->animations.size();i++){
			obj->animations[i].frame=0;
		}

	}

	return 0;
}


static int luaAnimationRewindAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()>1){
		console().write("Animation Restart All Error: bad arguments");
		return 0;
	}

	String aname="default";

	if(args.size()==1){
		if(args[0].type=SCRIPT_STRING_STORED){
			aname=args[0].value.ss;
		}

	}

	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].frame=0;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){
				level->ikRoots[i]->animations[a].frame=0;
			}
		}
	}

	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].frame=0;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].frame=0;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].frame=0;
			}
		}
	}

	return 0;
}


static int luaAnimationFpsAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1 && args.size()!=2){
		console().write("Animation Restart All Error: bad arguments");
		return 0;
	}

	String aname="default";
	float fps=0;

	if(args.size()==1){
		if(args[0].type==SCRIPT_DOUBLE_STORED){
			fps=args[0].value.ds;
		
		}else{
			return 0;
		}
	}


	if(args.size()==2){
		if(args[0].type==SCRIPT_STRING_STORED){
			aname=args[0].value.ss;
		
		}

		if(args[1].type==SCRIPT_DOUBLE_STORED){
			fps=args[1].value.ds;
		
		}
	}


	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].fps=fps;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){
				level->ikRoots[i]->animations[a].fps=fps;
			}
		}
	}

	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].fps=fps;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].fps=fps;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].fps=fps;
			}
		}
	}

	return 0;
}


static int luaAnimationFrameAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1 && args.size()!=2){
		console().write("Animation Restart All Error: bad arguments");
		return 0;
	}

	String aname="default";
	float frame=0;

	if(args.size()==1){
		if(args[0].type==SCRIPT_DOUBLE_STORED){
			frame=args[0].value.ds;
		
		}else{
			return 0;
		}
	}else{

	}


	if(args.size()==2){
		if(args[0].type==SCRIPT_STRING_STORED){
			aname=args[0].value.ss;
		
		}

		if(args[1].type==SCRIPT_DOUBLE_STORED){
			frame=args[1].value.ds;
		
		}
	}


	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].frame=frame;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){
				level->ikRoots[i]->animations[a].frame=frame;
			}
		}
	}

	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].frame=frame;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].frame=frame;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].frame=frame;
			}
		}
	}

	return 0;
}


static int luaAnimationBlendAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1 && args.size()!=2){
		console().write("Animation Restart All Error: bad arguments");
		return 0;
	}

	String aname="default";
	float blend=0;

	if(args.size()==1){
		if(args[0].type==SCRIPT_DOUBLE_STORED){
			blend=args[0].value.ds;
		
		}else{
			return 0;
		}
	}


	if(args.size()==2){
		if(args[0].type==SCRIPT_STRING_STORED){
			aname=args[0].value.ss;
		
		}

		if(args[1].type==SCRIPT_DOUBLE_STORED){
			blend=args[1].value.ds;
		
		}
	}


	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].blend=blend;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){
				level->ikRoots[i]->animations[a].blend=blend;
			}
		}
	}

	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].blend=blend;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].blend=blend;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].blend=blend;
			}
		}
	}

	return 0;
}

static int luaAnimationLoopAll(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1 && args.size()!=2){
		console().write("Animation Restart All Error: bad arguments");
		return 0;
	}

	String aname="default";
	bool loop=false;

	if(args.size()==1){
		if(args[0].type==SCRIPT_BOOL_STORED){
			loop=args[0].value.bs;
		
		}else{
			return 0;
		}
	}


	if(args.size()==2){
		if(args[0].type==SCRIPT_STRING_STORED){
			aname=args[0].value.ss;
		
		}

		if(args[1].type==SCRIPT_BOOL_STORED){
			loop=args[1].value.bs;
		
		}
	}


	for(int i=0; i<level->objects.size(); i++){
		for(int a=0; a<level->objects[i]->animations.size(); a++){
			if(level->objects[i]->animations[a].name==aname){
				level->objects[i]->animations[a].loop=loop;
			}
		}
	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){
			if(level->ikRoots[i]->animations[a].name==aname){
				level->ikRoots[i]->animations[a].loop=loop;
			}
		}
	}

	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){
			if(level->ikJoints[i]->animations[a].name==aname){
				level->ikJoints[i]->animations[a].loop=loop;
			}
		}
	}

	for(int i=0; i<level->ikEffectors.size(); i++){
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){
			if(level->ikEffectors[i]->animations[a].name==aname){
				level->ikEffectors[i]->animations[a].loop=loop;
			}
		}
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){
			if(level->nulls[i]->animations[a].name==aname){
				level->nulls[i]->animations[a].loop=loop;
			}
		}
	}

	return 0;
}

template<class T>
float* getValuePointer(T* obj, String variable){

	if(variable=="pos.x"){
		return &obj->pos.x;
	}else if(variable=="pos.y"){
		return &obj->pos.y;
	}else if(variable=="pos.z"){
		return &obj->pos.z;
	}else if(variable=="rot.x"){
		return &obj->rot.x;
	}else if(variable=="rot.y"){
		return &obj->rot.y;
	}else if(variable=="rot.z"){
		return &obj->rot.z;
	}else if(variable=="scale.x"){
		return &obj->scale.x;
	}else if(variable=="scale.y"){
		return &obj->scale.y;
	}else if(variable=="scale.z"){
		return &obj->scale.z;
	}else{

	}

	return NULL;
}

void interpolate(ObjectAnimation* animation,float* fvalue){
	float v;
	float* value=&v;

	if(animation->type==0){	//constant
		
		if(animation->keys.size()<2){
			*value=animation->keys[0].value;
		}else if(animation->frame < animation->keys[1].frame){
			*value=animation->keys[0].value;
		}else if(animation->frame >= animation->keys[animation->keys.size()-1].frame){
			*value=animation->keys[animation->keys.size()-1].value;
		}else{
			for(int key=1; key<animation->keys.size(); key++){
				if(animation->frame < animation->keys[key].frame && animation->frame > animation->keys[key-1].frame){
					*value=animation->keys[key-1].value;
				}

			}
		}
	}else if(animation->type==1){

		if(animation->keys.size()<2){
			*value=animation->keys[0].value;
		}else if(animation->frame <= animation->keys[0].frame){
			*value=animation->keys[0].value;
		}else if(animation->frame >= animation->keys[animation->keys.size()-1].frame){
			*value=animation->keys[animation->keys.size()-1].value;
		}else{
			for(int key=1; key<animation->keys.size(); key++){

				if(animation->frame <= animation->keys[key].frame){

					float next=animation->keys[key].value;
					float previous=animation->keys[key-1].value;

					float weight=animation->keys[key].frame-animation->keys[key-1].frame;
					weight=(animation->frame-animation->keys[key-1].frame)/weight;

					float final=previous*weight+next*(weight-1);

					*value=final;

					break;
				}

			}
		}
	}else if(animation->type==2){

		if(animation->keys.size()<2){
			*value=animation->keys[0].value;
		}else if(animation->frame <= animation->keys[0].frame){
			*value=animation->keys[0].value;
		}else if(animation->frame >= animation->keys[animation->keys.size()-1].frame){
			*value=animation->keys[animation->keys.size()-1].value;
		}else{
			for(int key=1; key<animation->keys.size(); key++){

				if(animation->frame <= animation->keys[key].frame){
					//solve the parametric equation for a bezier curve

					FloatVector2d p0,p1,p2,p3;
					float final;

					p0.x=animation->keys[key-1].frame;
					p0.y=animation->keys[key-1].value;

					p1.x=animation->keys[key-1].rightTanX+animation->keys[key-1].frame;
					p1.y=animation->keys[key-1].rightTanY+animation->keys[key-1].value;

					p2.x=animation->keys[key].leftTanX+animation->keys[key].frame;
					p2.y=animation->keys[key].leftTanY+animation->keys[key].value;

					p3.x=animation->keys[key].frame;
					p3.y=animation->keys[key].value;

					float t=0;

					//calc t using cardano's method
					float d=p0.x;
						d-=animation->frame;
					float c=3*p1.x-3*p0.x;
					float b=3*p0.x+3*p2.x-6*p1.x;
					float aa=-p0.x+3*p1.x-3*p2.x+p3.x;

					if(aa==0 && b==0){
						t=-d/c;
					}else if(aa==0){
						float det=c*c-4*b*d;
						det=math.sqrt(det);

						t=-b+det;
						t/=2*b;

					}else{


						b/=aa;
						c/=aa;
						d/=aa;

						float q=(float(3)*c-(b*b))/float(9);
						float r=-(float(27)*d) + b*(float(9)*c-float(2)*b*b);
						r/=float(54);

						float newdet=q*q*q+r*r;

						if(newdet>0){
							float s=r+math.sqrt(newdet);

							if(s<0){
								s=-math.pow(-s,(float(1)/float(3)));
							}else{
								s=math.pow(s,(float(1)/float(3)));
							}

							float tt=r-math.sqrt(newdet);

							if(tt<0){
								tt=-math.pow(-tt,(float(1)/float(3)));
							}else{
								tt=math.pow(tt,(float(1)/float(3)));
							}

							t=-(b/float(3))+s+tt;

							final=p0.y*(1-t)*(1-t)*(1-t) + 3*p1.y*t*(1-t)*(1-t) + 3*p2.y*t*t*(1-t) + p3.y*t*t*t;

							*value=final;
							break;
						}

						if(newdet==0){

						}

						// Only option left is that all roots are real and unequal (to get here, q < 0)
						q = -q;
						float term1 = (b/3.0);

						float dum1 = q*q*q;
						dum1 = math.arccosRadians(r/math.sqrt(dum1));
						float r13 = float(2.0)*math.sqrt(q);
						float root1 = -term1 + r13*math.cosRadians(dum1/float(3.0));
						float root2 = -term1 + r13*math.cosRadians((dum1 + float(2.0)*PI)/float(3.0));
						float root3 = -term1 + r13*math.cosRadians((dum1 + float(4.0)*PI)/float(3.0));

						float root1F=p0.x*(1-root1)*(1-root1)*(1-root1)+3*p1.x*root1*(1-root1)*(1-root1)+3*p2.x*root1*root1*(1-root1)+p3.x*root1*root1*root1;
						float root2F=p0.x*(1-root2)*(1-root2)*(1-root2)+3*p1.x*root2*(1-root2)*(1-root2)+3*p2.x*root2*root2*(1-root2)+p3.x*root2*root2*root2;
						float root3F=p0.x*(1-root3)*(1-root3)*(1-root3)+3*p1.x*root3*(1-root3)*(1-root3)+3*p2.x*root3*root3*(1-root3)+p3.x*root3*root3*root3;


						bool tset=false;

						if(root1F >= animation->keys[key-1].frame && root1F <= animation->keys[key].frame){
							tset=true;
							t=root1;
						}

						if(root2F >= animation->keys[key-1].frame && root2F <= animation->keys[key].frame){
							tset=true;
							t=root2;
						}


						if(root3F >= animation->keys[key-1].frame && root3F <= animation->keys[key].frame){
							tset=true;
							t=root3;
						}

						
						
					}

					final=p0.y*(1-t)*(1-t)*(1-t) + 3*p1.y*t*(1-t)*(1-t) + 3*p2.y*t*t*(1-t) + p3.y*t*t*t;

					*value=final;

					break;
				}

			}
		}
	}

	*fvalue+=*value*animation->blend;

	if(animation->play){
		animation->frame=animation->frame+(animation->fps/engineTime.fps);

		if(animation->loop){
			if(animation->frame>animation->end){
				animation->frame=animation->start;
			}
		}
	}

}

void constraints(ObjectAddress thisObject,ObjectAddress* constraintPos, ObjectAddress* constraintRot, ObjectAddress* constraintScale,
							FloatVector3d* pos, FloatVector3d* rot, FloatVector3d* scale){
	if(constraintPos!=NULL){

		FloatVector3d globalPos=level->sceneGraph.globalPos(*constraintPos);

		*pos=level->sceneGraph.localPos(thisObject,globalPos);

	}

	if(constraintRot!=NULL){
		FloatVector3d globalRot=level->sceneGraph.globalRot(*constraintRot);

		*rot=level->sceneGraph.localRot(thisObject,globalRot);

	}

	if(constraintScale!=NULL){

	}
}

void Animation::perframe(){

	for(int i=0; i<level->nonstaticObjects.size(); i++){

		for(int a=0; a<level->nonstaticObjects[i]->animations.size(); a++){

			level->nonstaticObjects[i]->animations[a].perframe();

			if(level->nonstaticObjects[i]->animations[a].play || level->nonstaticObjects[i]->animations[a].dirty){
				float* value=getValuePointer<Object>(level->nonstaticObjects[i],level->nonstaticObjects[i]->animations[a].variable);
				*value=0;
			}
		}
		
		for(int a=0; a<level->nonstaticObjects[i]->animations.size(); a++){
			if(level->nonstaticObjects[i]->animations[a].play || level->nonstaticObjects[i]->animations[a].dirty){

				float* value=getValuePointer<Object>(level->nonstaticObjects[i],level->nonstaticObjects[i]->animations[a].variable);
				
				interpolate(&level->nonstaticObjects[i]->animations[a], value);
				
			}
		}

		ObjectAddress a;
		a.type=OBJECT;
		a.object=level->nonstaticObjects[i];
		constraints(a,level->nonstaticObjects[i]->constraintPos,level->nonstaticObjects[i]->constraintRot,level->nonstaticObjects[i]->constraintScale,
			&level->nonstaticObjects[i]->pos,&level->nonstaticObjects[i]->rot,&level->nonstaticObjects[i]->scale);
	}

	for(int i=0; i<level->nulls.size(); i++){
		for(int a=0; a<level->nulls[i]->animations.size(); a++){

			level->nulls[i]->animations[a].perframe();

			if(level->nulls[i]->animations[a].play || level->nulls[i]->animations[a].dirty){
				float* value=getValuePointer<NullObject>(level->nulls[i],level->nulls[i]->animations[a].variable);
				*value=0;
			}
		}

		for(int a=0; a<level->nulls[i]->animations.size(); a++){

			if(level->nulls[i]->animations[a].play || level->nulls[i]->animations[a].dirty){


				float* value=getValuePointer<NullObject>(level->nulls[i],level->nulls[i]->animations[a].variable);
				interpolate(&level->nulls[i]->animations[a], value);
				
			}
		}

		ObjectAddress a;
		a.type=NULL3D;
		a.null=level->nulls[i];
		constraints(a,level->nulls[i]->constraintPos,level->nulls[i]->constraintRot,level->nulls[i]->constraintScale,
			&level->nulls[i]->pos,&level->nulls[i]->rot,&level->nulls[i]->scale);

	}

	for(int i=0; i<level->ikRoots.size(); i++){
		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){

			level->ikRoots[i]->animations[a].perframe();

			if(level->ikRoots[i]->animations[a].play || level->ikRoots[i]->animations[a].dirty){
				float* value=getValuePointer<IkRoot>(level->ikRoots[i],level->ikRoots[i]->animations[a].variable);
				*value=0;
			}
		}

		for(int a=0; a<level->ikRoots[i]->animations.size(); a++){

			if(level->ikRoots[i]->animations[a].play || level->ikRoots[i]->animations[a].dirty){
				float* value=getValuePointer<IkRoot>(level->ikRoots[i],level->ikRoots[i]->animations[a].variable);
				interpolate(&level->ikRoots[i]->animations[a], value);
		
			}
		}

		ObjectAddress a;
		a.type=IK_ROOT;
		a.ikRoot=level->ikRoots[i];
		constraints(a,level->ikRoots[i]->constraintPos,level->ikRoots[i]->constraintRot,level->ikRoots[i]->constraintScale,
			&level->ikRoots[i]->pos,&level->ikRoots[i]->rot,&level->ikRoots[i]->scale);

	}

	for(int i=0; i<level->ikJoints.size(); i++){
		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){

			level->ikJoints[i]->animations[a].perframe();

			if(level->ikJoints[i]->animations[a].play || level->ikJoints[i]->animations[a].dirty){
				float* value=getValuePointer<IkJoint>(level->ikJoints[i],level->ikJoints[i]->animations[a].variable);
				*value=0;
			}
		}

		for(int a=0; a<level->ikJoints[i]->animations.size(); a++){

			if(level->ikJoints[i]->animations[a].play || level->ikJoints[i]->animations[a].dirty){
				float* value=getValuePointer<IkJoint>(level->ikJoints[i],level->ikJoints[i]->animations[a].variable);
				interpolate(&level->ikJoints[i]->animations[a], value);
			
			}
		}

		ObjectAddress a;
		a.type=IK_JOINT;
		a.ikJoint=level->ikJoints[i];
		constraints(a,level->ikJoints[i]->constraintPos,level->ikJoints[i]->constraintRot,level->ikJoints[i]->constraintScale,
			&level->ikJoints[i]->pos,&level->ikJoints[i]->rot,&level->ikJoints[i]->scale);

	}

	for(int i=0; i<level->ikEffectors.size(); i++){
	
		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){


			level->ikEffectors[i]->animations[a].perframe();
			if(level->ikEffectors[i]->animations[a].play || level->ikEffectors[i]->animations[a].dirty){
				float* value=getValuePointer<IkEffector>(level->ikEffectors[i],level->ikEffectors[i]->animations[a].variable);
				*value=0;
			}
		}

		for(int a=0; a<level->ikEffectors[i]->animations.size(); a++){

			if(level->ikEffectors[i]->animations[a].play || level->ikEffectors[i]->animations[a].dirty){

				float* value=getValuePointer<IkEffector>(level->ikEffectors[i],level->ikEffectors[i]->animations[a].variable);
				interpolate(&level->ikEffectors[i]->animations[a], value);
				
			}
		}

		ObjectAddress a;
		a.type=IK_EFFECTOR;
		a.ikEffector=level->ikEffectors[i];
		constraints(a,level->ikEffectors[i]->constraintPos,level->ikEffectors[i]->constraintRot,level->ikEffectors[i]->constraintScale,
			&level->ikEffectors[i]->pos,&level->ikEffectors[i]->rot,&level->ikEffectors[i]->scale);

	}
}

bool Animation::scriptInit(){
	LuaType* luaa=script().add("animation");
		script().add("play",luaAnimationPlay,luaa);
		script().add("playAll",luaAnimationPlayAll,luaa);
		script().add("rewind",luaAnimationRewind,luaa);
		script().add("rewindAll",luaAnimationRewindAll,luaa);
		script().add("fps",luaAnimationFps,luaa);
		script().add("fpsAll",luaAnimationFpsAll,luaa);
		script().add("loop",luaAnimationLoop,luaa);
		script().add("loopAll",luaAnimationLoopAll,luaa);
		script().add("blend",luaAnimationBlend,luaa);
		script().add("blendAll",luaAnimationBlendAll,luaa);
		script().add("pause",luaAnimationPause,luaa);
		script().add("pauseAll",luaAnimationPauseAll,luaa);
		script().add("frame",luaAnimationFrame,luaa);
		script().add("frameAll",luaAnimationFrameAll,luaa);
		script().add("startFrame",luaAnimationStartFrame,luaa);
		script().add("endFrame",luaAnimationEndFrame,luaa);
		script().add("state",luaAnimationState,luaa);
	return true;
}

void ObjectAnimation::perframe(){
	dirty=false;

	if(oldFrame!=frame){
		dirty=true;
		oldFrame=frame;
	}

	if(oldBlend!=blend){
		dirty=true;
		oldBlend=blend;
	}
}

#endif


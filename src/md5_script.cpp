#include "script.hpp"
#include "md5.h"
#include <iostream>
#include "os.h"

int luamd5 (lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	unsigned char* dat=new unsigned char[args[0].value.ss.size()+1];

	memcpy(dat,args[0].value.ss.c_str(),args[0].value.ss.size()+1);

	MD5 md5;
	md5.update(dat,args[0].value.ss.size());
	md5.finalize();
	char* hex=new char[33];

	hex=md5.hex_digest();

	String o=hex;

	script().result(L,o);

	return 1;
}


int luamd5File(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	int length;
	unsigned char* dat;

	MD5 md5;
	Path pt;
	pt.setRelative(args[0].value.ss);
	std::ifstream in(pt.getAbsolute().c_str(),std::ios::in | std::ios::binary);	//,ios::in | ios::binary);

	if(in==NULL){
		console().write(String("Error, file '")+args[0].value.ss+"' not found");
		return 0;
	}

	// get length of file:
	in.seekg (0, std::ios::end);
	length = in.tellg();
	in.seekg (0, std::ios::beg);

	// allocate memory:
	dat = new unsigned char [length];

	// read data as a block:
	in.read ((char*)dat,length);

	in.close();

	md5.update(dat,length);

	md5.finalize();
	char* hex=new char[33];

	hex=md5.hex_digest();

	String o=hex;

	script().result(L,o);

	return 1;
}

bool md5ScriptInit(){
	script().add("md5",luamd5);
	script().add("md5File",luamd5File);

	return true;
}
#include "os.h"


#include "Helperlib/Log.hpp"

#ifndef XSICONVERT
#include "script.hpp"
#endif

Window::Window(){
	active=false;
	fullscreen=false;
	multisample=false;
	size.x=1024;
	size.y=768;
    depth=32;
    zDepth=32;
}	

Os::Os(){
	
    
    window=new Window;
    
}

bool Os::yesNo(std::string question){
	return true;
}

bool Os::showMouse(){
	return true;
}

bool Os::mkdir(Path dir){
	return false;
}

unsigned int Os::time(){
	return 0;
}

bool Os::fileExists(Path){
	return false;
}

Array<ScreenMode> Os::screenModes(){
	Array<ScreenMode> temp;
	
	return temp;
}

std::string Os::execute(std::string program,std::string arguments,std::string workingDirectory){
	return "error";
}

Array<Path> Os::folderContents(Path){
    Array<Path> temp;
    
    return temp;
}

#ifndef XSICONVERT
static int luaError(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	os().error(args[0].value.ss);

	return 0;

}
static int luaFolderExists(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	script().result(L,os().folderExists(args[0].value.ss));

	return 1;
}

static int luaFileExists(lua_State *L){
	
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}
	
	script().result(L,os().fileExists(args[0].value.ss));

	return 1;
}
static int luaFolderContents(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	Array<Path> fc=os().folderContents(args[0].value.ss);

	script().resultTable(L);

	for(int i=0; i<fc.size(); i++){
		script().result(L,i+1,fc[i].getRelative());
	}

	return 1;
}

static int luaMkdir(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, incorrect arguments");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, incorrect arguments, bad type");
		return 0;
	}

	os().mkdir(args[0].value.ss);
	return 0;
}


static int luaExecute(lua_State *L){
	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=2){
		console().write("os.execute Error, there should be two arguments");
		return 0;
	}


	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("os.execute Error, incorrect arguments, bad type, first argument should be a string");
		return 0;
	}

	if(args[1].type!=SCRIPT_STRING_STORED){
		console().write("os.execute Error, incorrect arguments, bad type, second argument should be a string");
		return 0;
	}

	String lol=os().execute(args[0].value.ss,args[1].value.ss);

	if(lol==""){

	}else{
		console().write("ERROR EXECUTING: "+lol);
	}

	return 0;
}


static int luaExit(lua_State *L){

	exit(0);

	return 1;
}

static int luaTime(lua_State *L){

	double t=os().time();

	script().result(L,t);


	return 1;
}
#endif


#ifdef _WIN32
String osName="windows";
#else
String osName="mac";
#endif

String osVersion;

bool osScriptInit(){
	#ifndef XSICONVERT
	script().run("osClock=os.clock");

	LuaType* w=script().add("os");

		script().add("folderExists",&luaFolderExists,w);
		script().add("fileExists",&luaFileExists,w);
		script().add("mkdir",&luaMkdir,w);
		script().add("folderContents",&luaFolderContents,w);
		script().add("exit",&luaExit,w);
		script().add("execute",&luaExecute,w);
		script().add("time",&luaTime,w);
		script().add("error",&luaError,w);
		script().add("name",&osName,w);
		script().add("version",&osVersion,w);

		#ifdef _WIN32
		OSVERSIONINFO m_osinfo;
		m_osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&m_osinfo);

		DWORD dwMinorVersion = m_osinfo.dwMinorVersion;
		DWORD dwMajorVersion = m_osinfo.dwMajorVersion;

		if(dwMajorVersion==5 && dwMinorVersion==1){
			osVersion="xp";
		}else if(dwMajorVersion==5 && dwMinorVersion==0){
			osVersion="2000";
		}else if(dwMajorVersion==4 && dwMinorVersion==0){
			osVersion="nt";
		}else if(dwMajorVersion==4 && dwMinorVersion==90){
			osVersion="me";
		}else if(dwMajorVersion==4 && dwMinorVersion==10){
			osVersion="98";
		}else if(dwMajorVersion==4 && dwMinorVersion==0){
			osVersion="95";
		}else{
			osVersion=String(dwMajorVersion)+String(".")+String(dwMinorVersion);
		}

		logs().main.write("OS:Windows");
		logs().main.write(String("MAJORVERSION:")+dwMajorVersion);
		logs().main.write(String("MINORVERSION:")+dwMinorVersion);
		logs().main.write("VERSION:"+osVersion);
	#elif LINUX
		logs().main.write("OS:Linux");
	#else
		SInt32 MacVersion;

		logs().main.write("OS:Mac");

		if (Gestalt(gestaltSystemVersion, &MacVersion) == noErr){
		
			if(MacVersion<0x1010){
				osVersion="10.0";
			}else if(MacVersion<0x1020){
				osVersion="10.1";
			}else if(MacVersion<0x1030){
				osVersion="10.2";
			}else if(MacVersion<0x1040){
				osVersion="10.3";
			}else if(MacVersion<0x1050){
				osVersion="10.4";
			}else if(MacVersion<0x1060){
				osVersion="10.5";
			}else if(MacVersion<0x1070){
				osVersion="10.6";
			}
		}
		
		logs().main.write("VERSION:"+osVersion);
	#endif
	
	script().run("os.clock=osClock");
	script().run("osClock=nil");
#endif

	return true;
}

bool Os::copy(Path sourcefile, Path destfile){
	return true;
}




void Path::setRelative(String t){
	path=t;
	format=PATH_RELATIVE;
}

void Path::setAbsolute(String t){
	path=t;
	format=PATH_ABSOLUTE;
}

String Path::getFilename(){

	int i;

	for(i=path.size()-1; i>0; i--){
		if(path[i]=='/' || path[i]=='\\'){
			break;
		}else{
			
		}
	}

	if(i!=0){i++;}

	return path.substrIndex(i,path.size());
}


String Path::getFilenameNoExtension(){
	String fn=getFilename();

	int i;

	for(i=0; i<fn.size(); i++){
		if(fn[i]=='.'){
			break;
		}else{
			
		}
	}

	String din=fn.substrIndex(0,i);

	return din;
}	


Path::Path(const char * t){
	path=t;
	format=PATH_RELATIVE;
}


bool Path::operator !=(const Path& p){
	return p.path!=path;
}

bool Path::operator ==(const Path& p){
	return p.path==path;
}

void Path::clear(){
	path="";
}

int operator<(const Path& left, const Path& right){
	return left.path<right.path;
}

String Path::getRelativeDirectory(){
	if(format==PATH_RELATIVE){
		int i;

		for(i=path.size()-1; i>=0; i--){
			if(path[i]=='/' || path[i]=='\\'){
				break;
			}else{
				
			}
		}
		
		i--;

		return path.substrIndex(0,i);
	}
	
	return "";
}


String Path::getRelativeNoExtension(){
	String rel=getRelative();

	int i;

	for(i=0; i<rel.size(); i++){
		if(rel[i]=='.'){
			break;
		}else{
			
		}
	}

	return rel.substrIndex(0,i);

}


String Path::getExtension(){

	if(!path.contains(".")){
		return "";
	}
	
	int i;

	for(i=path.size()-1; i>0; i--){
		if(path[i]=='.'){
			break;
		}else{
			
		}
	}

	return path.substrIndex(i+1,path.size());
}

bool Path::set(){
	if(path==""){
		return false;
	}	

	return true;
}

Path::Path(){

}

Path::Path(String t){
	path=t;
	format=PATH_RELATIVE;
}


String Path::getRelativeTo(String dir){
	return "";
}

#include "script.hpp"

#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include "gl/extensions.h"
#endif

#include "conf.h"
#include "Log.hpp"
#include "HelperLibMath.hpp"
#include "Text.hpp"
#include "keyboard.h"
#include "os.h"
#include "random.h"
#include "timeing.hpp"
#include "shader.hpp"
#include "script_upload.h"
#include "level.hpp"
#include "rsa.h"
#include "Text.hpp"

extern "C" { 
#include "luasocket.h"
#include "mime.h"
}

int outstandingAllocs=0;

#ifndef OLD_SYNC
Array<LuaType*>* scriptVariableRemap;
#endif

#ifdef DEVELOPER
Map<String,int> trackedExpressions;
String trackedResult;
#endif

void ScriptConsole::draw(){

	if(keyboard.tab_up && level->camera->eye==0){
		visible=!visible;
	}

	if(visible){

		static int cursorOffset=0;


		if(keyboard.current_letter_down=='p'  && keyboard.ctrl){
			if(conf->pause){
				console().write("ENGINE RESUMED (ctrl-p to pause)");
			}else{
				console().write("ENGINE PAUSED (ctrl-p to resume)");
			}
			conf->pause=!conf->pause;
		}

		if(keyboard.current_letter_down=='r'  && keyboard.ctrl){
			console().write("LEVEL RELOADED (ctrl-r)");
			reloadLevel();
		}

		glDisable( GL_CLIP_PLANE0 );
		glDisable(GL_DEPTH_TEST);

		int luaMem=lua_gc(script().L,LUA_GCCOUNT,0);

		float luaMemMegs=float(luaMem)/float(1024);

		//shadow lol
		String debug=String("fps: ")+String(engineTime.fps)+String("\n")+String("  time: ")+String(engineTime.time)+String("\n")+
			String("script: ")+engineTime.getTimer("script")+"\t["+engineTime.getTimerAvg("script")+"] ["+engineTime.getTimerPercent("script")+"%]\n"+
			String("     syncto: ")+engineTime.getTimer("syncto")+"\t["+engineTime.getTimerAvg("syncto")+"] ["+engineTime.getTimerPercent("syncto")+"%]\n"+
			String("     syncfrom: ")+engineTime.getTimer("syncfrom")+"\t["+engineTime.getTimerAvg("syncfrom")+"] ["+engineTime.getTimerPercent("syncfrom")+"%]\n"+
			String("total render: ")+engineTime.getTimer("render")+"\t["+engineTime.getTimerAvg("render")+"] ["+engineTime.getTimerPercent("render")+"%]\n"+
			String("     sort: ")+engineTime.getTimer("sort")+"\t["+engineTime.getTimerAvg("sort")+"] ["+engineTime.getTimerPercent("sort")+"%]\n"+
			String("     glow: ")+engineTime.getTimer("glow")+"\t["+engineTime.getTimerAvg("glow")+"] ["+engineTime.getTimerPercent("glow")+"%]\n"+
			String("overhead: ")+engineTime.getTimer("overhead")+"\t["+engineTime.getTimerAvg("overhead")+"] ["+engineTime.getTimerPercent("overhead")+"%]\n"+
			String("     update: ")+engineTime.getTimer("update")+"\t["+engineTime.getTimerAvg("update")+"] ["+engineTime.getTimerPercent("update")+"%]\n"+
			String("     console: ")+engineTime.getTimer("console")+"\t["+engineTime.getTimerAvg("console")+"] ["+engineTime.getTimerPercent("console")+"%]\n"+
			String("     os: ")+engineTime.getTimer("os")+"\t["+engineTime.getTimerAvg("os")+"] ["+engineTime.getTimerPercent("os")+"%]\n"+
			String("     swap: ")+engineTime.getTimer("swap")+"\t["+engineTime.getTimerAvg("swap")+"] ["+engineTime.getTimerPercent("swap")+"%]\n"+
			String("collide: ")+engineTime.getTimer("collide")+"\t["+engineTime.getTimerAvg("collide")+"] ["+engineTime.getTimerPercent("collide")+"%]\n"+
			String("audio: ")+engineTime.getTimer("audio")+"\t["+engineTime.getTimerAvg("audio")+"] ["+engineTime.getTimerPercent("audio")+"%]\n"+
			String("picking3d: ")+engineTime.getTimer("picking")+"\t["+engineTime.getTimerAvg("picking")+"] ["+engineTime.getTimerPercent("picking")+"%]\n"+
			String("picking2d: ")+engineTime.getTimer("picking2d")+"\t["+engineTime.getTimerAvg("picking2d")+"] ["+engineTime.getTimerPercent("picking2d")+"%]\n"+
			String("Lua Memory Usage: ")+String(luaMemMegs)+"MB\n";

		//glColor4f(0,0,0,0.25);
		//text.draw2d(200+2,-7,debug);

        float riftX = 400.f;
		glColor4f(1,1,1,0.9);
		text.draw2d(riftX,-5,debug);
		
		static float cursorTick=0;

        if(level->camera->eye==0){      
		if(keyboard.enter_down){ 
			writeUser(currentCommand);

			script().run(currentCommand);

			currentCommand="";
			currentCommandBkup="";
			ccIndex=0;
		}else{
			if(keyboard.backspaceDown){
				if(cursorOffset==0){
					currentCommand=currentCommand.substr(0,currentCommand.size()-1);
				}else{
					currentCommand=currentCommand.substr(0,currentCommand.size()-1-cursorOffset)+currentCommand.substr(currentCommand.size()-cursorOffset,currentCommand.size());
				}
			}else if(keyboard.arrow_up_down){

				int ssize=store.size();

				if(ccIndex==0){
					currentCommandBkup=currentCommand;
				}

				for(int i=ccIndex+1; i<ssize; i++){

					if(store[ssize-i].type==0){	
						ccIndex=i;
						
						currentCommand=store[ssize-ccIndex].message;
						break;
					}

				}
			}else if(keyboard.arrow_down_down){

				if(ccIndex==0){

				}else{

					int ssize=store.size();

					int i=0;

					for(i=ccIndex-1; i>0; i--){

						if(store[ssize-i].type==0){	
							ccIndex=i;
							currentCommand=store[ssize-ccIndex].message;
							break;
						}
					}

					if(i==0){
						ccIndex=0;
						currentCommand=currentCommandBkup;
					}

					if(ccIndex==0){
						currentCommand=currentCommandBkup;
					}else{
						currentCommand=store[store.size()-ccIndex].message;
					}
				}
			}else if(keyboard.arrow_left_down){
				cursorOffset++;
				cursorTick=0;
			}else if(keyboard.arrow_right_down){
				cursorOffset--;
				cursorTick=0;
				if(cursorOffset<0){cursorOffset=0;}
			}else{
				if(cursorOffset==0){
					currentCommand+=keyboard.currentStringDown;
				}else{
					currentCommand=currentCommand.substr(0,currentCommand.size()-cursorOffset)+keyboard.currentStringDown+currentCommand.substr(currentCommand.size()-cursorOffset,currentCommand.size());
				}
			}
		}
}

		String cc="> "+currentCommand;

		FloatVector2d size=text.getSize(0,"XYZ");
		float y=-conf->sizeY+size.y+5;

		//shadow lol
		//glColor4f(0,0,0,0.25);
		//text.draw2d(riftX+2,y-2+100,cc);

        float riftY = 450.f;

		glColor4f(1,1,1,0.9);
		text.draw2d(riftX,y+riftY,cc);

		int eTargetSize=40;
		int endsize=0;

		if(store.size()>eTargetSize){
			endsize=store.size()-eTargetSize;
		}

		//cursor
		if(cursorOffset>currentCommand.size()){cursorOffset=currentCommand.size();}

		cursorTick+=engineTime.time*5;

		float al=0;
		if(math.cos(cursorTick)>0){al=1;}

		glColor4f(1,1,1,al);

		FloatVector2d cpx=text.getSize(0,currentCommand.substr(0,currentCommand.size()-cursorOffset));
		cpx.x+=10;

		glBegin(GL_QUADS);
			glVertex3f(cpx.x+10	,y,0);
			glVertex3f(cpx.x+11	,y,0);
			glVertex3f(cpx.x+11	,y-15,0);
			glVertex3f(cpx.x+10	,y-15,0);
		glEnd();

		for(int i=store.size()-1; i>=endsize; i--){

			y+=size.y+5;

			if(store[i].type==0){
				//shadow lol
				//glColor4f(0,0,0,0.25);
				//text.draw2d(riftX+2,y-3+100,"> "+store[i].message);

				glColor4f(0.9,0.9,0.9,0.9);
				text.draw2d(riftX,y+riftY,"> "+store[i].message);
			}else{
				//glColor4f(0,0,0,0.25);
				//text.draw2d(riftX+2,y-3+100,store[i].message);
	
				glColor4f(0.9,0.0,0.0,0.9);
				text.draw2d(riftX,y+riftY,store[i].message);
			}

		}

		
		#ifdef DEVELOPER
			glColor4f(0,0.9,0.0,0.9);
			Array<String> tracked=trackedExpressions.indices();

			y=0;
			for(int i=0; i<tracked.size(); i++){
				String exec="trackEval(returnSerialize("+tracked[i]+"))";

				script().run(exec);

				String str=tracked[i]+"="+trackedResult;
				FloatVector2d size=text.getSize(0,str);

				y+=size.y+2;

				text.draw2d(conf->sizeX-size.x-10,-conf->sizeY+y+7,str);
			}
		#endif
	
	}
}

void ScriptConsole::write(String s){
	
#ifdef HEADLESS
	std::cout << s.c_str() << std::endl;
#else

	if(store.size()>0){
		if(repeatCount==-1){
			if(store[store.size()-1].message==s){
				store[store.size()-1].message=s+" [X2]";
				repeatCount=2;
				return;
			}
		}else{
			if(store[store.size()-1].message== s+" [X"+repeatCount+"]"){
				repeatCount++;
				store[store.size()-1].message=s+" [X"+repeatCount+"]";
				return;
			}else{
				repeatCount=-1;
			}
		}
	}


	logs().script.write(s);

	ConsoleMessage t;
	t.type=1;
	t.message=s;

	store.pushBack(t);
#endif
}


void ScriptConsole::writeUser(String s){
	logs().script.write(s);

#ifdef HEADLESS
	std::cout << s.c_str() << std::endl;
#else
	ConsoleMessage t;
	t.type=0;
	t.message=s;

	store.pushBack(t);
#endif
}

ScriptConsole::ScriptConsole(){
	visible=false;
	ccIndex=0;
	repeatCount=-1;
}

ScriptConsole& console(){
	static ScriptConsole e;
	return e;
}

////////////////////////////////////////////////




LuaType::LuaType(){
	parent=NULL;
	newTable=true;
}

LuaType::~LuaType(){

}


static int luaPrint(lua_State *L){
	String s=luaL_checkstring(L,1);
	console().write(s);
	return 0;
}

static int luaTrack(lua_State *L){

	#ifdef DEVELOPER
		String s=luaL_checkstring(L,1);
		if(!trackedExpressions.find(s)){
			trackedExpressions[s]=1;
			console().write("'"+s+"' is now being tracked");
		}
	#endif
	
	return 0;
}


static int luaTrackEval(lua_State *L){
	#ifdef DEVELOPER
		trackedResult=luaL_checkstring(L,1);
	#endif
	return 0;
}


bool Script::pushOntoStack(LuaType* lt){

	syncTo(NULL);

	lua_pop(L, 1);	

	Array<LuaType*> trace;

	trace.pushBack(lt);

	while(lt->parent!=NULL){
		lt=lt->parent;
		trace.pushBack(lt);
	}

	for(int i=trace.size()-1; i>=0; i--){
		lua_pushstring(L, trace[i]->name.cStr());

		if(i==trace.size()-1){
			lua_gettable(L, LUA_GLOBALSINDEX); //get table[key] 
		}else{
			lua_gettable(L, -2);
		}

		if(lua_istable(L, -1)){

		}else{
			console().write("push onto stack error: bad type on '"+trace[i]->name+"' "+lua_typename(L, -1));
			return false;
		}
		
		if(i!=0){
			lua_pop(L,1);
		}
	}

	return true;
}

bool Script::run(String s){
	engineTime.startTimer("syncto");
 
	#ifdef OLD_SYNC
		syncTo();
	#endif

	engineTime.endTimer("syncto",true);

	int error = luaL_loadbuffer(L, s.cStr(), strlen(s.cStr()), "line"); 

	error|=lua_pcall(L, 0, 0, 0);

	if (error) {
				const char* t=lua_tostring(L, -1);

				if(t==NULL){
					console().write("script error, unknown err (NULL)");
				}else{
					String err=t;
					console().write(err);
				}

				lua_pop(L, 1);  /* pop error message from the stack */
				return false;
			}

	engineTime.startTimer("syncfrom");
	#ifdef OLD_SYNC
		syncFrom();
	#endif


	engineTime.endTimer("syncfrom",true);

	return true;
}

bool Script::runRaw(String s){
	engineTime.startTimer("syncto");

	engineTime.endTimer("syncto",true);

	int error = luaL_loadbuffer(L, s.cStr(), strlen(s.cStr()), "line") || lua_pcall(L, 0, 0, 0);

			if (error) {
				console().write(lua_tostring(L, -1));
				lua_pop(L, 1);  /* pop error message from the stack */
				return false;
			}

	engineTime.startTimer("syncfrom");
	engineTime.endTimer("syncfrom",true);

	return true;
}

#ifdef FEATURE_SOCKETS
bool Script::initLuaSocket(lua_State *L){
	
	lua_pushstring(L,"package");
	lua_gettable(L,LUA_GLOBALSINDEX);
	lua_pushstring(L,"preload");
	lua_gettable(L,-2);
	lua_pushstring(L, "socket.core");
	lua_pushcfunction(L, luaopen_socket_core);
	lua_settable(L, -3);

	lua_pushstring(L,"package");
	lua_gettable(L,LUA_GLOBALSINDEX);
	lua_pushstring(L,"preload");
	lua_gettable(L,-2);
	lua_pushstring(L, "mime.core");
	lua_pushcfunction(L, luaopen_mime_core);
	lua_settable(L, -3);


	runRaw("package.path=package.path..';.\\\\socket\\\\?.lua;?.lua;./socket/?.lua'");
	runRaw("package.path=package.path..';..\\\\tools\\\\socket\\\\?.lua;..\\\\..\\\\tools\\\\socket\\\\?.lua;?.lua;./socket/?.lua'");
	
	#ifndef _WIN32

		if(os().workingDirectory==""){
			console().write("error, working directory not set");
		}
		
		runRaw("package.path=package.path..';"+os().workingDirectory+"socket/?.lua'");

	#endif
	
	runRaw("socket=require('socket')");

	//
	runRaw("http=require('socket.http')");
	runRaw("ftp=require('socket.ftp')");

   return true;
}
#endif

int luaDofileHack(lua_State *L){
	String s=luaL_checkstring(L,1);
	script().runFile(s);
	return 0;
}

int luaPathHack(lua_State *L){
	String s=luaL_checkstring(L,1);
	NSString* ins = [NSString stringWithUTF8String:s.cStr()];
	NSString* res = [ins stringByExpandingTildeInPath];
	lua_pushstring(L, [res cString]);
	return 1;
}

bool Script::init(){

	logs().script.write("Script::init");

	L = lua_open();

	lua_settop(L, 0);
	
	luaL_openlibs(L);	//openall libs

	#ifdef FEATURE_SOCKETS
		initLuaSocket(L);
	#endif

	add("print",luaPrint);
	add("track",luaTrack);
	add("trackEval",luaTrackEval);
	add("dofile",luaDofileHack);
	add("pathhack",luaPathHack);

	#ifdef _WIN32
	
	#else
		runRaw("io_output_hack=io.output");
		runRaw("io.output=function(x) if x:sub(0,1)=='~' then print(pathhack(x));io_output_hack(pathhack(x)) else io_output_hack('"+os().workingDirectory+"'..x) end end");
	#endif
	
#ifndef OLD_SYNC
	lua_pushcfunction(L, cSetFunc);
    lua_setglobal(L, "cSetFunc");
 
	lua_pushcfunction(L, cGetFunc);
    lua_setglobal(L, "cGetFunc");
#endif

	runRaw("function math.round(num, idp) local mult = 10^(idp or 0); return math.floor(num  * mult + 0.5) / mult; end");

	runRaw("function math.sinDegrees(t) return math.sin(t * (math.pi/180)) end");
	runRaw("function math.cosDegrees(t) return math.cos(t * (math.pi/180)) end");
	runRaw("function math.tanDegrees(t) return math.tan(t * (math.pi/180)) end");

	runRaw("function math.multiply(a,b) if type(a)=='number' then if b.x~=nil and b.y~=nil and b.z~=nil then return {x=a*b.x,y=a*b.y,z=a*b.z} elseif b.x~=nil and b.y~=nil then return {x=a*b.x,y=a*b.y} end elseif type(b)=='number' then return {x=b*a.x,y=b*a.y,z=b*a.z} else print('multiply error') end end");
	runRaw("function math.subtract(a,b) if a.x~=nil and a.y~=nil and a.z~=nil then return {x=a.x-b.x,y=a.y-b.y,z=a.z-b.z} elseif a.x~=nil and a.y~=nil then return {x=a.x-b.x,y=a.y-b.y} end end");
	runRaw("function math.add(a,b) return {x=a.x+b.x,y=a.y+b.y,z=a.z+b.z} end");
	runRaw("function math.cross(a,b) return {x=a.y*b.z-a.z*b.y, y=a.z*b.x-a.x*b.z, z=a.x*b.y-a.y*b.x} end");
	runRaw("function math.dot(a,b) return (a.x*b.x+a.y*b.y+a.z*b.z) end");
	runRaw("function math.square(a) return a*a end");
	runRaw("function math.invert(a) return {x=-a.x,y=-a.y,z=-a.z} end");
	runRaw("function math.normalize(a) if a.x~=nil and a.y~=nil and a.z~=nil then local m=1/(math.sqrt(a.x*a.x+a.y*a.y+a.z*a.z));return {x=m*a.x,y=m*a.y,z=m*a.z} elseif a.x~=nil and a.y~=nil then local m=1/(math.sqrt(a.x*a.x+a.y*a.y));return {x=m*a.x,y=m*a.y} end end");
	runRaw("function math.magnitude(a) if a.x~=nil and a.y~=nil and a.z~=nil then return math.sqrt(a.x*a.x+a.y*a.y+a.z*a.z); elseif a.x~=nil and a.y~=nil then return math.sqrt(a.x*a.x+a.y*a.y); end end");

	runRaw("math.e=2.718281828459045235360287471352");
	
	runRaw("rawnext=next");

	runRaw("function next(t,k) local mt = getmetatable( t ); local next_imp = ( mt and mt.__next ) or rawnext; return next_imp( t, k ); end");

	//lua 5.1 needs this next line for some reason
	runRaw("function pairs(t) return next,t end");

	String ser;

ser="buffer='' ";

runRaw(ser);

ser="function swrite(str) "
"	if string.find(str,'\\n') then "
"		buffer = buffer .. str; "
"		buffer=string.sub(buffer,0,-2); "
"		print(buffer); "
"		buffer=''; "
"	else "
"       	buffer = buffer .. str; "
"	end "
"end ";

runRaw(ser);

ser="function serialize (o,level) "
"	if level==nil then level='     ' end "	
"   if type(o) == 'number' then "
"       swrite(o..'\\n') "
"   elseif type(o) == 'string' then "
"       swrite(string.format('%q', o)..' \\n') "  
"	elseif type(o) == 'boolean' then "
"		if o==true then "
"			swrite('true \\n') "
"       else "
"			swrite('false \\n') "
"		end "
"	elseif type(o) == 'function' then "
"		swrite('function \\n') "
"	elseif type(o) == 'table' then "
"       swrite('{\\n') "
"       for k,v in pairs(o) do "
"          		swrite(level .. k .. ' = ') "         		
"          		serialize(v,level..'     ') "
"       end "
"       tlevel=string.sub(level,0,-6) "
"       swrite(tlevel..'}\\n') "
"	else "
"        	swrite('cannot serialize a ' .. type(o)) "
"	end "
"end";

runRaw(ser);

ser="function returnSerialize (o,level) "
"	if o==nil then return 'nil' end "
"	local x='' "
"	if level==nil then level='     ' end "	
"   if type(o) == 'number' then "
"       x=x..o "
"   elseif type(o) == 'string' then "
"       x=x..string.format('%q', o) "  
"	elseif type(o) == 'boolean' then "
"		if o==true then "
"			x=x..'true' "
"       else "
"			x=x..'false' "
"		end "
"	elseif type(o) == 'function' then "
"		x=x..'function' "
"	elseif type(o) == 'table' then "
"       x=x..'{\\n' "
"       for k,v in pairs(o) do "
"			if type(k)=='number' then "
"          		x=x..level .. '[' .. k .. '] = ' "  
"			elseif type(k)=='string' then "
"          		x=x..level .. '[\"' .. k .. '\"] = ' "  
"			else "
"          		x=x..level .. k .. ' = ' "         		
"			end "
"          	x=x..returnSerialize(v,level..'     ')..',\\n' "
"       end "
"       tlevel=string.sub(level,0,-6) "
"       x=x..tlevel..'}' "
"	end "
"	return x "
"end ";

runRaw(ser);

ser="function explode(d,p) "
"	t={} "
"	ll=0 "	
"	while true do "
"		l,ly=string.find(p,d,ll+1,true) "
"		if l~=nil then "
"			table.insert(t, string.sub(p,ll,l-1)) "
"			ll=l+(ly-l)+1 "
"		else "
"			table.insert(t, string.sub(p,ll)) " 
"			break " 
"		end "
"	end "
"	thack=false "
"	if t[1]=='' then "
"		thack=true "
"		table.remove(t,1) "
"	end "
"	if t[2]==nil then "
"		if thack then "
"			return t[1] "
"		else "
"			return p "
"		end "
"	else "
"		return t "
"	end "
"end ";


runRaw(ser);

ser="function fileSerialize(table,file,tablename) "
"        fileSerializeBuffer=tablename..'='..returnSerialize(table) "        
"        io.output(file) "
"        io.write(fileSerializeBuffer) "
"        io.close() "
"end ";

runRaw(ser);

	logs().script.write("Script::init done");

	return true;
}
 
bool Script::runFile(Path file){
	
	syncTo();


	if(os().fileExists(file)){
		//unencrypted

		logs().script.write("loading '"+file.getRelative()+"'");

		//open it and see what kind of file it is
		std::ifstream in(file.getAbsolute().c_str(),std::ios::in|std::ios::binary);

		#ifdef VIEWER

			in.seekg(0,std::ios::end);
			unsigned int pos=in.tellg();
			in.seekg(0,std::ios::beg);

			scriptBytesLoaded[file]=pos;
			 
			unsigned int loadedTotal=0;
			Array<unsigned int> s=scriptBytesLoaded.values();

			for(int i=0; i<s.size(); i++){
				loadedTotal+=s[i];
			}

			if(loadedTotal>MAX_SCRIPT_BYTES){
				os().error("Error: You have exceeded the maximum script size allowed in the Viewer addition. "+String(loadedTotal)+" used, "+String(MAX_SCRIPT_BYTES)+" allowed.");
				return false;
			}

		#endif

		char head[4];

		in.read(head,4);

		if(head[0]=='L' &&
			head[1]=='M' && 
			head[2]=='S' &&
			head[3]=='1'){

			//this is complicated now lol

			//read in our data

				in.seekg(0,std::ios::end);
				int scriptlen=in.tellg();
				scriptlen-=260;

			char* scriptdata=new char[scriptlen];

			in.seekg(260,std::ios::beg);
			in.read(scriptdata,scriptlen);

			in.close();

			#ifdef RSA
				String r=rsa_verify_file(os().path(file).c_str());	

				if(r==""){
					signedFileLoaded=true;
				}else{
					console().write("Sign Error: "+r);
					return false;
				}
			#endif

			int error=luaL_loadbuffer(L,scriptdata,scriptlen,file.getRelative().c_str());

			if(error!=0){
				console().write("READ ERROR");
			}


			int err=lua_pcall(L, 0, 0, 0);

			if(error || err){
				String errr=lua_tostring(L, -1);

				os().error("script error:"+errr);
				return false;
			}

		}else{

			#ifdef RSA
				if(!signedFileLoaded){
					os().error("Script Error: At least one signed script file must be loaded before an unsigned file can be loaded (in the release version)");
				}
			#endif

			if (luaL_loadfile(L, file.getAbsolute().c_str()) || lua_pcall(L, 0, 0, 0)){
				String err=lua_tostring(L, -1);
				os().error("script error:"+err);
				console().write(err);
				return false;
			}
		}
	}else{
		os().error("error, script '"+file.getRelative()+"' doesn't exist!");

		logs().script.write("error, script file '"+file.getRelative()+"' not found");
		return false;
	}

	syncFrom();

	return true;
}


int Script::next(int t,lua_State* LL){
#ifdef OLD_SYNC
	return lua_next(LL, t);
#else
	//p1:table p2:nil/key

	if(lua_getmetatable(LL,t)==0){
		//no metatable, use the default
		return lua_next(LL, t);
	}

	//p1:table p2:nil/key p3:metatable
	lua_pushstring(LL,"__next");
	//p1:table p2:nil/key p3:metatable p4:keystring"__next"

	//lua_gettable(LL, -2);	//where index points to the table. lua_gettable pops a key from the stack and returns (on the stack) the contents of the table at that key. The table is left where it was in the stack.
	lua_rawget(LL,-2);

	//p1:table p2:nil/key p3:metatable p4:function"__next"

	if(lua_type( LL, lua_gettop( LL ) )==LUA_TFUNCTION){

		//The number of results are adjusted to nresults, unless nresults is LUA_MULTRET

		lua_pushvalue(LL,-4);	//push  the table we want to traverse onto the stack (make a copy from earlier)
		//p1:table p2:nil/key p3:metatable p4:function"__next" p5:table

		lua_pushvalue(LL,-4);	//push key
		//p1:table p2:nil/key p3:metatable p4:function"__next" p5:table p6:nil/key
	
		if(lua_pcall(LL,2,2,0)!=0){

			console().write(lua_tostring(LL, -1));
			return 0;
		}

		//p1:table p2:nil/key p3:metatable p4:newkey p5:newvalue
		//NOTE: if this is the last value, newkey+newvalue are both NIL

		//now, -1 should be the value, -2 the key


		lua_remove(LL,-3);
		//p1:table p2:nil/key p3:newkey p4:newvalue

		lua_remove(LL,-3);
		//p1:table p2:newkey p1:newvalue

		if(lua_type(LL, -1)==LUA_TNIL){
			//If there are no more elements, then lua_next returns 0 (and pushes nothing).

			//p1:table p2:newkey p3:newvalue
			lua_remove(LL,-1);
			//p1:table p2:newkey
			lua_remove(LL,-1);

			//p1:table

			return 0;
		}else{
			return 1;
		}
	}else{
		return 0;


	}
#endif

	return 0;
}


void Script::getArgsTable(SafeArray<LuaType>* final,lua_State* LL){

	lua_pushnil(LL);  // first key 

	while (next(-2,LL)!= 0) {
		LuaType* c;
		final->pushBack();
		c=&(*final)[final->size()-1];

		if(lua_type(LL, -2)==LUA_TSTRING){
			
			c->name=lua_tostring( LL, -2 );

			if(lua_type(LL, -1)==LUA_TNUMBER){
				c->type=SCRIPT_DOUBLE_STORED;
				c->value.ds=lua_tonumber( LL, -1 );
			}else if(lua_type(LL, -1)==LUA_TSTRING){
				c->type=SCRIPT_STRING_STORED;
				c->value.ss=lua_tostring( LL, -1 );
			}else if(lua_type(LL, -1)==LUA_TBOOLEAN){
				c->type=SCRIPT_BOOL_STORED;
				c->value.bs=lua_toboolean( LL, -1 );
			}else if(lua_type(LL, -1)==LUA_TTABLE){
				c->type=SCRIPT_TABLE;
				getArgsTable(&c->children,LL);
			}else if(lua_type(LL, -1)==LUA_TFUNCTION){
				c->type=SCRIPT_FUNCTION;
			}
		}else if(lua_type(LL, -2)==LUA_TNUMBER){
			//numeric index
			c->nameint=lua_tonumber( LL, -2 );

			if(lua_type(LL, -1)==LUA_TNUMBER){
				c->type=SCRIPT_DOUBLE_STORED;
				c->value.ds=lua_tonumber( LL, -1 );
			}else if(lua_type(LL, -1)==LUA_TSTRING){
				c->type=SCRIPT_STRING_STORED;
				c->value.ss=lua_tostring( LL, -1 );
			}else if(lua_type(LL, -1)==LUA_TBOOLEAN){
				c->type=SCRIPT_BOOL_STORED;
				c->value.bs=lua_toboolean( LL, -1 );
			}else if(lua_type(LL, -1)==LUA_TTABLE){
				c->type=SCRIPT_TABLE;
				getArgsTable(&c->children,LL);
			}else if(lua_type(LL, -1)==LUA_TFUNCTION){
				c->type=SCRIPT_FUNCTION;
			}

		}else{
			//key other than a string
			
		}
		
		//`key' is at index -2 and `value' at index -1 
		lua_pop(LL, 1);  // removes `value'; keeps `key' for next iteration 
	}

}

Array<LuaType> Script::getArgs(lua_State* LL){

	Array<LuaType> lt;

	int numargs = lua_gettop(LL);

	for(int i=0; i<numargs; i++){
		lt.pushBack();
	}

	for(int i=numargs-1; i>=0; i--){

		if(lua_type( LL, lua_gettop( LL ) )==LUA_TTABLE){
			lt[i].type=SCRIPT_TABLE;
			getArgsTable(&lt[i].children,LL);

			lua_pop(LL, 1);
		}else if(lua_type( LL, lua_gettop( LL ) )==LUA_TSTRING){
			String s = lua_tostring(LL,lua_gettop( LL ) );

			lt[i].type=SCRIPT_STRING_STORED;
			lt[i].value.ss=s;

			lua_pop(LL, 1);

		}else if(lua_type( LL, lua_gettop( LL ) )==LUA_TNUMBER){

			double s = lua_tonumber(LL,lua_gettop( LL ) );

			lt[i].type=SCRIPT_DOUBLE_STORED;
			lt[i].value.ds=s;

			lua_pop(LL, 1);

		}else if(lua_type( LL, lua_gettop( LL ) )==LUA_TBOOLEAN){

			bool s = lua_toboolean(LL,lua_gettop( LL ) );

			lt[i].type=SCRIPT_BOOL_STORED;
			lt[i].value.bs=s;

			lua_pop(LL, 1);
		}else if(lua_type( LL, lua_gettop( LL ) )==LUA_TTHREAD){

		}else{

		}

	}

	lua_settop(LL, 0); //clear the stack

	return lt;
}

void Script::resultTable(lua_State* LL){

	lua_newtable(LL);
}

void Script::result(lua_State* LL,float i){
	lua_pushnumber(LL,(double)i);

}

void Script::result(lua_State* LL,int i){
	lua_pushnumber(LL,(double)i);

}

void Script::result(lua_State* LL,double i){
	lua_pushnumber(LL,i);

}


void Script::result(lua_State* LL,String i){
	lua_pushstring(LL,i.cStr());

}

void Script::result(lua_State* LL,bool i){
	lua_pushboolean(LL,i);

}

void Script::resultStartTable(lua_State* LL, String name){
	lua_pushstring(LL,name.cStr());
	lua_newtable(LL);
}

void Script::resultEndTable(lua_State* LL){
	lua_settable(LL, -3);
}

void Script::result(lua_State* LL, String name, float i){
	lua_pushstring(LL,name.cStr());
	lua_pushnumber(LL,(double)i);
	lua_settable(LL, -3);

}



void Script::result(lua_State* LL,String name,lua_CFunction func){
	lua_pushstring(LL, name.cStr());
	lua_pushcfunction(LL, func);
	lua_settable(LL, -3);
}

void Script::result(lua_State* LL,int name,String str){
	lua_pushnumber(LL, name);
	lua_pushstring(LL, str.cStr());
	lua_settable(LL, -3);
}


void Script::arg(char* arg){
	lua_pushstring(L,arg);

}
void Script::arg(String& arg){
	lua_pushstring(L,arg.cStr());
}

bool Script::call(String function,int args=0, int results=0){
	bool r=true;

	lua_settop(L, 0);

	lua_getglobal(L, function.cStr());  /* function to be called */

	if (lua_pcall(L, args,results, 0) != 0){
		console().write(lua_tostring(L, -1));
		r=false;
	}
	
	return r;
}

bool  Script::searchGlobal(void* f,SafeArray<LuaType>* lt=NULL){
	//check to see if this already exists

	//don't care anymore
	return false;

	if(lt==NULL){
		lt=&global;

		
	}

	for(int i=0; i<lt->size(); i++){
		if((*lt)[i].type==SCRIPT_TABLE){
			if(searchGlobal(f,&(*lt)[i].children)){
				return true;
			}

		}else{
			if((*lt)[i].value.f==f){
				return true;
			}
		}

	}

	return false;
}


LuaType* Script::add(String name,lua_CFunction func){
	#ifdef SCRIPTDEBUG
		console().write("add: cfunction '"+name+"'");
	#endif

	

	lua_pushcfunction(L, func);
    lua_setglobal(L, name.c_str());

	
	return NULL;
}

LuaType* Script::add(String name,lua_CFunction func,LuaType* target){

	#ifdef SCRIPTDEBUG
		console().write("add: cfunction w/target '"+name+"'");
	#endif

	if(target->type=SCRIPT_TABLE){

		LuaType t;
		t.name=name;
		t.value.cfunc=func;
		t.type=SCRIPT_FUNCTION;
		t.parent=target;


		target->children.pushBack(t);

		upload(&target->children[target->children.size()-1]);

		return &target->children[target->children.size()-1];		

	}else{
		console().write("add: error, target '"+target->name+"' isn't a table");
	}

	return NULL;
}


LuaType* Script::add(String name){
	#ifdef SCRIPTDEBUG
		console().write("add: table '"+name+"'");
	#endif


	for(int i=0; i<global.size(); i++){
		if(global[i].name==name){
			return &global[i];
		}
	}

	LuaType t;
	t.name=name;
	t.type=SCRIPT_TABLE;
	t.parent=NULL;


	global.pushBack(t);

	upload(&global[global.size()-1]);

	return &global[global.size()-1];

}

LuaType* Script::add(String name,LuaType* target){
	#ifdef SCRIPTDEBUG
		console().write("add: table (w/target) '"+name+"'");
	#endif

	LuaType t;
	t.name=name;
	t.type=SCRIPT_TABLE;
	t.parent=target;


	target->children.pushBack(t);

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];

}

LuaType* Script::add(int name,LuaType* target){
	#ifdef SCRIPTDEBUG
		console().write("add: table (w/target) '"+name+"'");
	#endif

	LuaType t;
	t.nameint=name;
	t.type=SCRIPT_TABLE_INT;
	t.parent=target;


	target->children.pushBack(t);

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];

}

LuaType* Script::add(String name, float* value){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: float '"+name+"'");
	#endif


	LuaType t;
	t.name=name;
	t.value.f=value;
	t.type=SCRIPT_FLOAT;
	t.parent=NULL;


	global.pushBack(t);

	upload(&global[global.size()-1]);
	return &global[global.size()-1];
}

LuaType* Script::add(String name, float* value,LuaType* target){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: float w/target '"+name+"' value: "+*value);
	#endif


	LuaType t;
	t.name=name;
	t.value.f=value;
	t.type=SCRIPT_FLOAT;
	t.parent=target;


	target->children.pushBack(t);

	

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}


LuaType* Script::add(String name, double* value,LuaType* target){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: double w/target '"+name+"' value: "+*value);
	#endif


	LuaType t;
	t.name=name;
	t.value.d=value;
	t.type=SCRIPT_DOUBLE;
	t.parent=target;


	target->children.pushBack(t);

	

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}



LuaType* Script::add(String name, int* value,LuaType* target){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: int w/target '"+name+"' value: "+*value);
	#endif


	LuaType t;
	t.name=name;
	t.value.i=value;
	t.type=SCRIPT_INT;
	t.parent=target;


	target->children.pushBack(t);

	

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}


LuaType* Script::add(String name, int value,LuaType* target){
	#ifdef SCRIPTDEBUG
		console().write("add: int static w/target '"+name+"' value: "+value);
	#endif

	LuaType t;
	t.name=name;
	t.value.is=value;
	t.type=SCRIPT_INT_STORED;
	t.parent=target;

	target->children.pushBack(t);
	
	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}

LuaType* Script::add(String name, bool value,LuaType* target){
	
	#ifdef SCRIPTDEBUG
		console().write("add: int static w/target '"+name+"' value: "+value);
	#endif

	LuaType t;
	t.name=name;
	t.value.bs=value;
	t.type=SCRIPT_BOOL_STORED;
	t.parent=target;

	target->children.pushBack(t);

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}

LuaType* Script::add(String name, String value,LuaType* target){

	#ifdef SCRIPTDEBUG
		console().write("add: string static w/target '"+name+"' value: "+value);
	#endif

	LuaType t;
	t.name=name;
	t.value.ss=value;
	t.type=SCRIPT_STRING_STORED;
	t.parent=target;


	target->children.pushBack(t);

	
	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}

LuaType* Script::add(String name, const char* value,LuaType* target){

	#ifdef SCRIPTDEBUG
		console().write("add: string static w/target '"+name+"' value: "+value);
	#endif


	LuaType t;
	t.name=name;
	t.value.ss=String(value);
	t.type=SCRIPT_STRING_STORED;
	t.parent=target;


	target->children.pushBack(t);

	
	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}

LuaType* Script::add(String name, unsigned int* value,LuaType* target){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: int w/target '"+name+"' value: "+*value);
	#endif


	LuaType t;
	t.name=name;
	t.value.ui=value;
	t.type=SCRIPT_UNSIGNED_INT;
	t.parent=target;


	target->children.pushBack(t);

	

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}


LuaType* Script::add(String name, unsigned int value,LuaType* target){

	#ifdef SCRIPTDEBUG
		console().write("add: int static w/target '"+name+"' value: "+value);
	#endif


	LuaType t;
	t.name=name;
	t.value.uis=value;
	t.type=SCRIPT_UNSIGNED_INT_STORED;
	t.parent=target;


	target->children.pushBack(t);

	
	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}

LuaType* Script::add(String name, bool* value,LuaType* target){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: bool w/target '"+name+"'");
	#endif


	LuaType t;
	t.name=name;
	t.value.b=value;
	t.type=SCRIPT_BOOL;
	t.parent=target;

	target->children.pushBack(t);

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}



LuaType* Script::add(String name, String* value,LuaType* target){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: bool w/target '"+name+"'");
	#endif

	LuaType t;
	t.name=name;
	t.value.s=value;
	t.type=SCRIPT_STRING;
	t.parent=target;

	
	target->children.pushBack(t);

	upload(&target->children[target->children.size()-1]);
	return &target->children[target->children.size()-1];
}


LuaType* Script::add(String name, String* value){

	if(searchGlobal((void*)value)){
		console().write("Error, address of '"+name+"' already added under another name");
		fatalError=true;
		return NULL;
	}

	#ifdef SCRIPTDEBUG
		console().write("add: bool w/target '"+name+"'");
	#endif

	LuaType t;
	t.name=name;
	t.value.s=value;
	t.type=SCRIPT_STRING;
	t.parent=NULL;
	
	global.pushBack(t);

	upload(&global[global.size()-1]);
	return &global[global.size()-1];
}

bool Script::remove(LuaType* var){

	if(var->parent==NULL){
		for(int i=0; i<global.size(); i++){

			if(&global[i]==var){
				global.erase(i);
				return true;
			}
		}
	}else{

		for(int i=0; i<var->parent->children.size(); i++){

			if(&var->parent->children[i]==var){
				var->parent->children.erase(i);
				return true;
			}
		}
	}

	return false;
}



LuaType* Script::change(LuaType* var, double* newValue){

	
	var->value.d=newValue;
	var->type=SCRIPT_DOUBLE;

	return var;
}


LuaType* Script::change(LuaType* var, float* newValue){

	
	var->value.f=newValue;
	var->type=SCRIPT_FLOAT;

	return var;
}

LuaType* Script::change(LuaType* var, bool* newValue){

	
	var->value.b=newValue;
	var->type=SCRIPT_BOOL;

	return var;
}

void Script::enableSync(){
	doSync=true;
}


void Script::disableSync(){
	doSync=false;
}

Script::Script(){
	fatalError=false;
	L=NULL;
	doSync=true;

	signedFileLoaded=false;

#ifndef OLD_SYNC
	scriptVariableRemap=&remap;
	lastIter.name="";
#else

#endif

	init();
}

Script::~Script(){
	if(L!=NULL){
		lua_close(L);
	}
}

Script& script() {
	static Script e;
	return e;
}


bool Script::reload(){

	if(os().fileExists(conf->script)){
		//unencrypted

		console().write("reloading script '"+conf->script+"'");

		if (luaL_loadfile(L, conf->script.cStr()) || lua_pcall(L, 0, 0, 0)){
			console().write(lua_tostring(L, -1));
			return false;
		}
	}else{
		console().write("error, script file '"+conf->script+"' not found");
		return false;
	}


	return true;
}


LuaType* Script::rename(LuaType* var,String name){

	script().syncFrom(NULL);
		
	if(name==var->name){
		console().write("ERROR: attemping to rename a variable to its original name");
	}

	String s;
	s=name+"={};"+name+"="+var->name+";"+var->name+"={};";
	int error = luaL_loadbuffer(L, s.cStr(), strlen(s.cStr()), "line") || lua_pcall(L, 0, 0, 0);

			if (error) {
				console().write(lua_tostring(L, -1));
				lua_pop(L, 1);  /* pop error message from the stack */
				return false;
			}



	var->name=name;

	script().syncTo(NULL);

	return var;
	
}

bool Script::deleteGlobal(LuaType* item){

	bool found=false;

	for(int i=0; i<global.size(); i++){
		if(&global[i]==item){
			global.erase(i);
			found=true;
			break;
		}

	}

	if(!found){
		return false;
	}

	return true;
}

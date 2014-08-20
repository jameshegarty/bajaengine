#include "script.hpp"

#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

#include "conf.h"


#include "Log.hpp"

#include "Text.hpp"

#include "keyboard.h"

#include "os.h"
#include "random.h"
#include "timeing.hpp"

#include "shader.hpp"

#include "Extensions.h"

#include "script_upload.h"



int cSetFunc(lua_State* L){
#ifndef OLD_SYNC
	int i=luaL_checknumber(L,1);
	
	LuaType* var=(*scriptVariableRemap)[(int)i];

	if(var->type==SCRIPT_FLOAT){
		if(lua_type(L, 2)==LUA_TNUMBER){
			double v=luaL_checknumber(L,2);
			*var->value.f=v;
		}else{
			console().write("Error, tried to set variable '"+var->name+"' to a bad type.");
		}

	}else if(var->type==SCRIPT_INT){
		if(lua_type(L, 2)==LUA_TNUMBER){
			double v=luaL_checknumber(L,2);

			*var->value.i=v;
		}else{
			console().write("Error, tried to set variable '"+var->name+"' to a bad type.");
		}
	}else if(var->type==SCRIPT_UNSIGNED_INT){
		if(lua_type(L, 2)==LUA_TNUMBER){
			double v=luaL_checknumber(L,2);

			*var->value.ui=v;
		}else{
			console().write("Error, tried to set variable '"+var->name+"' to a bad type.");
	
		}
	}else if(var->type==SCRIPT_STRING){
		if(lua_type(L, 2)==LUA_TSTRING){
			String v=luaL_checkstring(L,2);
			*var->value.s=v;
		}else if(lua_type(L, 2)==LUA_TNIL){
			*var->value.s="";
		}
	}else if(var->type==SCRIPT_BOOL){
		bool v=lua_toboolean(L,2);
		*var->value.b=v;
	}else if(var->type==SCRIPT_DOUBLE){
		if(lua_type(L, 2)==LUA_TNUMBER){
			double v=luaL_checknumber(L,2);
			*var->value.d=v;
		}else{
			console().write("Error, tried to set variable '"+var->name+"' to a bad type.");
		}
	}

#endif

	return 0;
}


int cGetFunc(lua_State* L){
#ifndef OLD_SYNC
	int i=luaL_checknumber(L,1);
	LuaType* var=(*scriptVariableRemap)[(int)i];

	if(var->type==SCRIPT_FLOAT){
		
		lua_pushnumber(L,*var->value.f);
		return 1;
	}else if(var->type==SCRIPT_INT){

		lua_pushnumber(L,(double)*var->value.i);
		return 1;
	}else if(var->type==SCRIPT_UNSIGNED_INT){

		lua_pushnumber(L,(double)*var->value.ui);
		return 1;
	}else if(var->type==SCRIPT_BOOL){

		lua_pushboolean(L,*var->value.b);
		return 1;
	}else if(var->type==SCRIPT_STRING){

		lua_pushstring(L,var->value.s->c_str());
		return 1;
	}else if(var->type==SCRIPT_DOUBLE){

		lua_pushnumber(L,*var->value.d);
		return 1;
	}

#endif
	return 0;

}


bool Script::bindVariablesToTable(LuaType* parent,String path){		//path="blah"  path="blah.lol" or path="" for global


	String syncs;
	
	syncs="do "
	"  local meta, glue = {}, {} ";
	
	if(path!=""){
		syncs+="  local old_meta = getmetatable("+path+") ";
	}else{
		syncs+="  local old_meta = getmetatable(getfenv()) ";
	}
	
syncs+="  local old_index, old_newindex "
"  if old_meta then "
"    old_index, old_newindex = old_meta.__index, old_meta.__newindex "
"  end ";
	
	Array<LuaType*> list;

	if(path==""){
		for(int i=0; i<global.size(); i++){
			if(global[i].type!=SCRIPT_TABLE && global[i].type!=SCRIPT_TABLE_INT && global[i].type!=SCRIPT_FUNCTION){
				list.pushBack(&global[i]);
			}
		}
	}else{
		for(int i=0; i<parent->children.size(); i++){
			if(parent->children[i].type!=SCRIPT_TABLE && parent->children[i].type!=SCRIPT_TABLE_INT && parent->children[i].type!=SCRIPT_FUNCTION){
				list.pushBack(&parent->children[i]);
			}
		}
	}


	String lol;

	for(int i=0; i< list.size(); i++){
			lol=list[i]->name;
		
			syncs+=" glue[\"";
			syncs+=lol;
			syncs+="\"]=";
			syncs+=stringFunctions.toString(list[i]->remapIndex);
			syncs+=";";
	}

syncs+="meta = {} "
"  if type(old_index) == 'function' then "
"      function meta.__index(t, k) "
"        if glue[k] "
"          then return cGetFunc(glue[k]) "
"          else return old_index(t, k) "
"        end "
"      end "
"    elseif type(old_index) == 'nil' then "
"      function meta.__index(t, k) "
"        if glue[k] then return cGetFunc(glue[k]) end "
"      end "
"    else "
"      function meta.__index(t, k) "
"        if glue[k] "
"          then return cGetFunc(glue[k]) "
"          else return old_index[k] "
"        end "
"      end "
"  end "
"  if old_newindex "
"    then "
"      function meta.__newindex(t, k, v) "
"        if glue[k] "
"          then cSetFunc(glue[k],v) "
"          else old_newindex(t, k, v) "
"        end "
"      end "
"    else "
"      function meta.__newindex(t, k, v) "
"        if glue[k] "
"          then "
"            cSetFunc(glue[k],v) "
"          else "
"            rawset(t, k, v) "
"          end "
"      end "
"  end ";

syncs+="function meta.__next(t,i) "
"	if glue[i] then "
"		x=rawnext(glue,i) "
"		return x,t[x] "
"	end "
"	if rawnext(t,i)==nil then "
"		x=rawnext(glue) "
"		return x,t[x] "
"	else "
"		return rawnext(t,i) "
"	end "
"end ";

if(path!=""){
	syncs+="  setmetatable("+path+", meta) ";
}else{
	syncs+="  setmetatable(getfenv(), meta) ";
}

syncs+=" end ";

runRaw(syncs);

return true;
}


bool Script::pushTable(LuaType* obj){
	Array<LuaType*> reverse;
	LuaType* c=obj->parent;

	while(c!=NULL){
		reverse.pushBack(c);
		c=c->parent;
	}

	for(int i=reverse.size()-1; i>=0; i--){	

		if(reverse[i]->type==SCRIPT_TABLE){
			lua_pushstring(L, reverse[i]->name.cStr());
		}else if(reverse[i]->type==SCRIPT_TABLE_INT){
			lua_pushnumber(L,reverse[i]->nameint);
		}else{
			//crazy error
			return false;
		}

		if(reverse[i]->newTable){
			return false;
		}else{

			if(i==reverse.size()-1){

				lua_gettable(L, LUA_GLOBALSINDEX);
			}else{

				lua_gettable(L, -2);
			}
		}
	}

	return true;
}	

bool Script::upload(LuaType* obj){

#ifdef OLD_SYNC
	return true;
#else


	remap.pushBack(obj);
	obj->remapIndex=remap.size()-1;

	if(obj->type==SCRIPT_TABLE){
		if(obj->parent==NULL){
			lua_newtable(L); 
			lua_setglobal(L,obj->name.c_str());
			obj->newTable=false;
		}else{
			pushTable(obj);

			lua_pushstring(L, obj->name.c_str());
			lua_newtable(L); 
	
			lua_settable(L, -3);

			obj->newTable=false;

		}

	}else if(obj->type==SCRIPT_TABLE_INT){
		if(obj->parent==NULL){
			//doesn't work with global table
			return false;
		}else{
			pushTable(obj);

			lua_pushnumber(L,obj->nameint);

			lua_newtable(L); 
			
			lua_settable(L, -3);

			obj->newTable=false;
		}

	}else if(obj->type==SCRIPT_INT_STORED){

		if(obj->parent==NULL){
			lua_pushnumber(L,obj->value.is);
			lua_setglobal(L, obj->name.c_str());
		}else{
			pushTable(obj);
			lua_pushstring(L, obj->name.c_str());
			lua_pushnumber(L,obj->value.is);
			lua_settable(L, -3);
		}		

	}else if(obj->type==SCRIPT_UNSIGNED_INT_STORED){

		if(obj->parent==NULL){
			lua_pushnumber(L,obj->value.uis);
			lua_setglobal(L, obj->name.c_str());
		}else{
			pushTable(obj);
			lua_pushstring(L, obj->name.c_str());
			lua_pushnumber(L,obj->value.uis);
			lua_settable(L, -3);
		}		

	}else if(obj->type==SCRIPT_BOOL_STORED){

		if(obj->parent==NULL){
			lua_pushboolean(L,obj->value.bs);
			lua_setglobal(L, obj->name.c_str());
		}else{
			pushTable(obj);
			lua_pushstring(L, obj->name.c_str());
			lua_pushboolean(L,obj->value.bs);
			lua_settable(L, -3);
		}		

	}else if(obj->type==SCRIPT_STRING_STORED){

		if(obj->parent==NULL){
			lua_pushstring(L,obj->value.ss.c_str());
			lua_setglobal(L, obj->name.c_str());
		}else{
			pushTable(obj);
			lua_pushstring(L, obj->name.c_str());
			lua_pushstring(L, obj->value.ss.c_str());
			lua_settable(L, -3);
		}		

	}else if(obj->type==SCRIPT_FLOAT || obj->type==SCRIPT_INT || obj->type==SCRIPT_UNSIGNED_INT || obj->type==SCRIPT_DOUBLE ||obj->type==SCRIPT_STRING || obj->type==SCRIPT_BOOL){
		if(obj->parent==NULL){
			bindVariablesToTable(NULL,"");
		}else{
			String path;

			LuaType* c;

			c=obj->parent;

			while(c!=NULL){
				if(c->parent==NULL){
					path=c->name+path;
				}else{
					path="[\""+c->name+"\"]"+path;
					
				}

				c=c->parent;
			}


			bindVariablesToTable(obj->parent,path);

			

		}
	}else if(obj->type==SCRIPT_FUNCTION){
		if(obj->parent==NULL){
			lua_pushcfunction(L, obj->value.cfunc);
			lua_setglobal(L, obj->name.c_str());
		}else{
			pushTable(obj);
			lua_pushstring(L, obj->name.c_str());
			lua_pushcfunction(L, obj->value.cfunc);
			lua_settable(L, -3);
		}		
	}


	lua_settop(L, 0);
	return true;
#endif
}

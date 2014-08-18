#include "script.hpp"

#ifdef OLD_SYNC

#include <iostream>
#include <fstream>

#include <windows.h>

#include "conf.h"


#include "Log.hpp"

#include "text.hpp"

#include "keyboard.h"

#include "os.h"
#include "random.h"
#include "timeing.hpp"

#include "shader.hpp"

#include "gl/extensions.h"


bool Script::syncFrom(SafeArray<LuaType>* lt){

	#ifndef OLD_SYNC
		return true;
	#endif

	if(!doSync){
		return true;
	}

	if(lt==NULL){
		lt=&global;
	}

	for(int i=0; i<lt->size(); i++){
		if((*lt)[i].type==SCRIPT_TABLE_INT){
			lua_pushnumber(L, (*lt)[i].nameint);
			
		}else{
			lua_pushstring(L, (*lt)[i].name.cStr());
		}

		if(lt==&global){
			lua_gettable(L, LUA_GLOBALSINDEX); //get table[key] 
		}else{
			lua_gettable(L, -2);
		}

		if((*lt)[i].type==SCRIPT_TABLE){
			if(lua_istable(L, -1)){
				syncFrom(&(*lt)[i].children);
			}else{
				if(!(*lt)[i].newTable){
					console().write("type mismatch: '"+(*lt)[i].name+"' needs to a table");
				}
				//lua_pop(L, 1);	//IMPORTANT TO NOT HAVE THIS
			}
		}else if((*lt)[i].type==SCRIPT_TABLE_INT){
			if(lua_istable(L, -1)){
				syncFrom(&(*lt)[i].children);
			}else{
				if(!(*lt)[i].newTable){
				console().write("type mismatch: '"+String((*lt)[i].nameint)+"' needs to a table");
				}
				//lua_pop(L, 1);	//IMPORTANT TO NOT HAVE THIS
			}
		}else if((*lt)[i].type==SCRIPT_INT){
			if(lua_isnumber(L, -1)){
				double t=lua_tonumber(L, -1);
				lua_pop(L, 1);

				if(lt==&global){
					*(*lt)[i].value.i=(int)t;
				}else{
					*(*lt)[i].value.i=(int)t;
				}

			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a number");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}
		}else if((*lt)[i].type==SCRIPT_INT_STORED){
			if(lua_isnumber(L, -1)){
				double t=lua_tonumber(L, -1);
				lua_pop(L, 1);
			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a number");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}
		}else if((*lt)[i].type==SCRIPT_FLOAT){
			if(lua_isnumber(L, -1)){
				double t=lua_tonumber(L, -1);
				lua_pop(L, 1);

				if(lt==&global){
					*(*lt)[i].value.f=(float)t;
				}else{
					*(*lt)[i].value.f=(float)t;
				}

			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a number");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}
		}else if((*lt)[i].type==SCRIPT_DOUBLE){
			if(lua_isnumber(L, -1)){

				double t=lua_tonumber(L, -1);
				lua_pop(L, 1);

				if(lt==&global){
					*(*lt)[i].value.d=t;
				}else{
					*(*lt)[i].value.d=t;
				}

			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a number");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}
		}else if((*lt)[i].type==SCRIPT_BOOL){
			if(lua_isboolean(L, -1)){
				bool t=lua_toboolean(L, -1);
				lua_pop(L, 1);

				if(lt==&global){
					*(*lt)[i].value.b=t;
				}else{
					*(*lt)[i].value.b=t;
				}

			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a boolean");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}
		}else if((*lt)[i].type==SCRIPT_STRING){
			if(lua_isstring(L, -1)){
				String t=lua_tostring(L, -1);
				lua_pop(L, 1);

				if(lt==&global){
					*(*lt)[i].value.s=t;
				}else{
					*(*lt)[i].value.s=t;
				}

			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a string");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}

		}else if((*lt)[i].type==SCRIPT_FUNCTION){
			if(lua_isfunction(L, -1)){
				lua_pop(L, 1);
			}else{
				console().write("type mismatch: '"+(*lt)[i].name+"' needs to a function");
				lua_pop(L, 1);	//IMPORTANT - NEED TO HAVE
			}
		}else{
			console().write("SYNCFROM: type mismatch: '"+(*lt)[i].name+"' bad type");
		}


	}

	lua_pop(L, 1);
	
	return true;
}


bool Script::syncTo(SafeArray<LuaType>* lt){

	#ifndef OLD_SYNC
		return true;
	#endif

	if(!doSync){
		return true;
	}

	if(lt==NULL){
		lt=&global;
	}


	for(int i=0; i<lt->size(); i++){

		if((*lt)[i].type==SCRIPT_TABLE){
			if((*lt)[i].newTable){
				lua_newtable(L); 
				(*lt)[i].newTable=false;
			}else{
				if(lt==&global){
					lua_pushstring(L, (*lt)[i].name.c_str());
					lua_gettable(L, LUA_GLOBALSINDEX); //get table[key] 
				}else{
					lua_pushstring(L, (*lt)[i].name.c_str());
					lua_gettable(L, -2); //get table[key] 
				}
			}

			if(!syncTo(&(*lt)[i].children)){
				return false;
			}

			if(lt==&global){
				lua_setglobal(L,(*lt)[i].name.c_str());
			}else{
				lua_pushstring(L, (*lt)[i].name.c_str());
				lua_insert(L, -2);
				lua_settable(L, -3);
			}
			
		}else if((*lt)[i].type==SCRIPT_TABLE_INT){

			if((*lt)[i].newTable){
				lua_newtable(L); 
				(*lt)[i].newTable=false;
			}else{
				if(lt==&global){

					console().write("error, numerical indices can't be for global tables");
					return false;
				}else{
					lua_pushnumber(L,(*lt)[i].nameint);
					lua_gettable(L, -2); //get table[key] 
				}
			}	
			
				if(!syncTo(&(*lt)[i].children)){
					return false;
				}

				if(lt==&global){
					console().write("error, numerical indices can't be for global tables");
					return false;
				}else{
					lua_pushnumber(L,(*lt)[i].nameint);
					lua_insert(L, -2);
					lua_settable(L, -3);
				}
			
		}else if((*lt)[i].type==SCRIPT_FLOAT){

			if(lt==&global){
				lua_pushnumber(L,(double)*(*lt)[i].value.f);
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());

				float f=*(*lt)[i].value.f;

				lua_pushnumber(L,(double)*(*lt)[i].value.f);
				lua_settable(L, -3);
			}

		}else if((*lt)[i].type==SCRIPT_DOUBLE){
	
			if(lt==&global){
				lua_pushnumber(L,*(*lt)[i].value.d);
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());
				lua_pushnumber(L,*(*lt)[i].value.d);
				lua_settable(L, -3);
			}

		}else if((*lt)[i].type==SCRIPT_INT){
	
			if(lt==&global){
				lua_pushnumber(L,(double)*(*lt)[i].value.i);
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());

				float f=*(*lt)[i].value.i;

				lua_pushnumber(L,(double)*(*lt)[i].value.i);
				lua_settable(L, -3);
			}
		}else if((*lt)[i].type==SCRIPT_INT_STORED){
	
			if(lt==&global){
				lua_pushnumber(L,(double)(*lt)[i].value.is);
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());

				lua_pushnumber(L,(double)(*lt)[i].value.is);
				lua_settable(L, -3);
			}
		}else if((*lt)[i].type==SCRIPT_BOOL){
			if(lt==&global){
				lua_pushboolean(L,*(*lt)[i].value.b);
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());

				lua_pushboolean(L,*(*lt)[i].value.b);
				lua_settable(L, -3);
			}

		}else if((*lt)[i].type==SCRIPT_STRING){
			if(lt==&global){
				lua_pushstring(L,(*(*lt)[i].value.s).cStr());
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());

				lua_pushstring(L,(*(*lt)[i].value.s).cStr());
				lua_settable(L, -3);
			}
		}else if((*lt)[i].type==SCRIPT_FUNCTION){
			if(lt==&global){
				lua_pushcfunction(L, (*lt)[i].value.cfunc);
				lua_setglobal(L,(*lt)[i].name.cStr());
			}else{
				lua_pushstring(L, (*lt)[i].name.cStr());
				lua_pushcfunction(L, (*lt)[i].value.cfunc);

				lua_settable(L, -3);
			}

		}else{	
			console().write("Error: '"+(*lt)[i].name+"' when adding");
			return false;
		}
		
	}

	if(lt==&global){
		lua_settop(L, 0);
	}

	return true;
}

#else
bool Script::syncFrom(SafeArray<LuaType>* lt){
	return true;
}

bool Script::syncTo(SafeArray<LuaType>* lt){
	return true;
}

#endif

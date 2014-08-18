#include "material.hpp"
#include "script.hpp"
#include "level.hpp"

MaterialControl material;

bool MaterialControl::add(String name){


	ObjectAddress o=level->add(name,"material");


	return true;
}


bool MaterialControl::deleteMaterial(String name){

	if(level->materialKey.find(name)){
		script().run("mat[\""+name+"\"]=nil");
	}else{
		console().write("material delete error, material '"+name+"' doesn't exist");
		return false;
	}

	Material* mat=level->materialKey[name];

	if(mat->nonstaticRefs>0){
		console().write("error, material '"+name+"' is still referenced by an object so it can't be deleted");

		for(int i=0; i<mat->nonstaticRefList.size(); i++){
			console().write(mat->nonstaticRefList[i]->name);
		}

		return false;
	}

	level->materialIndexKey.remove(name);
	level->materialKey.remove(name);

	int ind=-1;

	for(int i=0; i<level->materials.size(); i++){
		if(level->materials[i].name==name){
			level->materials.erase(i);
			ind=i;
			break;
		}
	}

	level->materialCount--;

	for(int i=0; i<level->objects.size(); i++){

		if(level->objects[i]->material>ind){
			level->objects[i]->material--;
		}
	}

	return true;
}

int luaAddMaterial(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("add material bad args");
		return 0;
	}

	if(args[0].type==SCRIPT_STRING_STORED){
		material.add(args[0].value.ss);

	}else{
		console().write("add material bad args");
		return 0;
	}

	return 0;
}


int luaDeleteMaterial(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("delete material bad args");
		return 0;
	}

	if(args[0].type==SCRIPT_STRING_STORED){
		material.deleteMaterial(args[0].value.ss);

	}else{
		console().write("delete material bad args");
		return 0;
	}

	return 0;
}

bool MaterialControl::scriptInit(){
	LuaType* m=script().add("material");

	script().add("add",luaAddMaterial,m);
	script().add("delete",luaDeleteMaterial,m);

	return true;
}
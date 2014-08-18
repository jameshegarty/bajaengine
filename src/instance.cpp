#include "instance.hpp"
#include "level.hpp"

Instance::Instance(){
	object=NULL;
}

Override::Override(){
	overridden=false;
}

void Instance::link(String name){
	if(level->objectKey.find(name)){

		object=new ObjectAddress;
		object->type=OBJECT;
		object->object=level->objectKey[name];

		Override* or1=new Override;
		or1->floatVector3dX=0;
		or1->floatVector3dY=0;
		or1->floatVector3dZ=0;
		or1->pointerFloatVector3dX=&object->object->pos.x;
		or1->pointerFloatVector3dY=&object->object->pos.y;
		or1->pointerFloatVector3dZ=&object->object->pos.z;
		or1->type=COMMONTYPE_FLOATVECTOR3D;
	
		overrides["pos"]=or1;
		LuaType* lp=script().add("pos",luaObj);
			script().add("x",&or1->floatVector3dX,lp);
			script().add("y",&or1->floatVector3dY,lp);
			script().add("z",&or1->floatVector3dZ,lp);
		script().add("posOverride",&or1->overridden,luaObj);

		Override* or2=new Override;
		or2->floatVector3dX=0;
		or2->floatVector3dY=0;
		or2->floatVector3dZ=0;
		or2->pointerFloatVector3dX=&object->object->rot.x;
		or2->pointerFloatVector3dY=&object->object->rot.y;
		or2->pointerFloatVector3dZ=&object->object->rot.z;
		or2->type=COMMONTYPE_FLOATVECTOR3D;
	
		overrides["rot"]=or2;
		LuaType* lpr=script().add("rot",luaObj);
			script().add("x",&or2->floatVector3dX,lpr);
			script().add("y",&or2->floatVector3dY,lpr);
			script().add("z",&or2->floatVector3dZ,lpr);
		script().add("rotOverride",&or2->overridden,luaObj);

		Override* or3=new Override;
		or3->floatVector3dX=0;
		or3->floatVector3dY=0;
		or3->floatVector3dZ=0;
		or3->pointerFloatVector3dX=&object->object->scale.x;
		or3->pointerFloatVector3dY=&object->object->scale.y;
		or3->pointerFloatVector3dZ=&object->object->scale.z;
		or3->type=COMMONTYPE_FLOATVECTOR3D;
	
		overrides["scale"]=or3;
		LuaType* lps=script().add("scale",luaObj);
			script().add("x",&or3->floatVector3dX,lps);
			script().add("y",&or3->floatVector3dY,lps);
			script().add("z",&or3->floatVector3dZ,lps);
		script().add("scaleOverride",&or3->overridden,luaObj);

	}else{
		console().write("error, unable to link because object '"+name+"' not found.");
	}

}

void Instance::pre(){

	Array<Override*> ovr=overrides.values();

	for(int i=0; i<ovr.size(); i++){
		if(ovr[i]->overridden){
			if(COMMONTYPE_FLOATVECTOR3D==ovr[i]->type){
				ovr[i]->pastFloatVector3dX=*ovr[i]->pointerFloatVector3dX;
				ovr[i]->pastFloatVector3dY=*ovr[i]->pointerFloatVector3dY;
				ovr[i]->pastFloatVector3dZ=*ovr[i]->pointerFloatVector3dZ;

				*ovr[i]->pointerFloatVector3dX=ovr[i]->floatVector3dX;
				*ovr[i]->pointerFloatVector3dY=ovr[i]->floatVector3dY;
				*ovr[i]->pointerFloatVector3dZ=ovr[i]->floatVector3dZ;
			}

		}
	}

}

void Instance::post(){
	Array<Override*> ovr=overrides.values();

	for(int i=0; i<ovr.size(); i++){
		if(ovr[i]->overridden){
			if(COMMONTYPE_FLOATVECTOR3D==ovr[i]->type){
				*ovr[i]->pointerFloatVector3dX=ovr[i]->pastFloatVector3dX;
				*ovr[i]->pointerFloatVector3dY=ovr[i]->pastFloatVector3dY;
				*ovr[i]->pointerFloatVector3dZ=ovr[i]->pastFloatVector3dZ;
			}
		}
	}
}

int luaInstanceLink(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("link error, bad args");
		return 0;
	}

	Instance* inst;
	String name;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				inst=level->instanceKey[args[0].children[i].value.ss];
			}
		}

	}

	if(args[1].type==SCRIPT_STRING_STORED){
		name=args[1].value.ss;
	}

	
	inst->link(name);

	return 0;
}

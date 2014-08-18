#include "level.hpp"

#include <iostream>
#include <fstream>

#include "GlHeader.hpp"

#include "Log.hpp"
#include "HelperLibMath.hpp"

#include "os.h"
#include "FileIo.hpp"
#include "Bmp.hpp"
#include "texture.hpp"
#include "camera.hpp"

#include "conf.h"

#include "draw.hpp"

using namespace std;

bool Level::luaRemove(Object* o){
	script().run("obj[\""+o->name+"\"]=nil;");

	return true;
}

bool Level::luaRemove(Panel* o){
	script().run("obj[\""+o->name+"\"]=nil;");

	return true;
}

bool Level::luaRemove(Instance* o){
	script().run("obj[\""+o->name+"\"]=nil;");

	return true;
}


static int luaDeleteTexture(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("add texture error, bad arguments");
		return 0;
	}

	String matname;
	String type;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				matname=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		type=args[1].value.ss;
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}


	Material* mat=level->materialKey[matname];

	mat->deleteTexture(type);

	return 0;
}

static int luaAddTexture(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("add texture error, bad arguments");
		return 0;
	}

	String matname;
	String type;
	String file;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				matname=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_STRING_STORED){
		type=args[1].value.ss;
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}

	if(args[2].type==SCRIPT_STRING_STORED){
		file=args[2].value.ss;
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}

	if(matname==""){
		console().write("eror, material name bad");
		return 0;
	}

	if(!level->materialKey.find(matname)){
		console().write("eror, material not found");
		return 0;
	}

	Material* mat=level->materialKey[matname];

	mat->addTexture(type,file);

	return 0;
}


static int luaMaterialShader(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("shader set error, bad arguments");
		return 0;
	}

	String matname;
	String type;
	String data;

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				matname=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}


	if(args[1].type==SCRIPT_STRING_STORED){
		type=args[1].value.ss;
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}

	if(args[2].type==SCRIPT_STRING_STORED){
		data=args[2].value.ss;
	}else{
		console().write("add texture error, bad arguments");
		return 0;
	}

	if(!level->materialKey.find(matname)){
		console().write("eror, material not found");
		return 0;
	}

	Material* mat=level->materialKey[matname];

	if(type=="builtin"){

		if(data=="multitexturing"){
			mat->shader=MATERIAL_SHADER_MIXER;
		}
	}

	return 0;
}

static int luaDeleteLightmap(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("shader set error, bad arguments");
		return 0;
	}

	String name;

	

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("lightmap error, bad arguments");
		return 0;
	}

	Object* obj=level->objectKey[name];

	obj->lightmap=-1;

	return 0;
}

static int luaLightmap(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("shader set error, bad arguments");
		return 0;
	}

	String name;
	String file;
	

	if(args[0].type==SCRIPT_TABLE){
		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}
	}else{
		console().write("lightmap error, bad arguments");
		return 0;
	}


	if(args[1].type==SCRIPT_STRING_STORED){
		file=args[1].value.ss;
	}else{
		console().write("lightmap error, bad arguments");
		return 0;
	}

	if(!level->objectKey.find(name)){
		console().write("eror, object not found");
		return 0;
	}

	Object* obj=level->objectKey[name];

	TextureImage l;
	l.file=file;
	l.ref=true;

	String lname;

	Array<String> a=file.explode(".");

	for(int i=0; i<a.size()-1; i++){
		lname+=a[i];
	}

	Array<String> b=lname.explode("/");

	lname=b[b.size()-1];

	l.name=lname;

	level->lightmapCount++;
	level->lightmaps.pushBack(l);

	obj->lightmap=level->lightmapCount-1;

	return 0;
}

bool Level::luaUpload(Material* o){
	//if you don't want material control, you can just uncomment the next line
	//return true;

	if(o->name==""){
		return false;
	}

	LuaType* foo=script().add(o->name,luaMat);

		script().add("addTexture",&luaAddTexture,foo);	//type=diff/spec/etc file=lol.bmp
		script().add("deleteTexture",&luaDeleteTexture,foo);
		script().add("shader",&luaMaterialShader,foo);

		script().add("shiny",&o->shiny,foo);
		script().add("name",&o->name,foo);

		LuaType* foop=script().add("diffuse",foo);
			script().add("r",&o->diffuse.x,foop);
			script().add("g",&o->diffuse.y,foop);
			script().add("b",&o->diffuse.z,foop);
			script().add("a",&o->diffuse.w,foop);

		LuaType* fooa=script().add("ambient",foo);
			script().add("r",&o->ambient.x,fooa);
			script().add("g",&o->ambient.y,fooa);
			script().add("b",&o->ambient.z,fooa);
			script().add("a",&o->ambient.w,fooa);

		LuaType* foos=script().add("specular",foo);
			script().add("r",&o->specular.x,foos);
			script().add("g",&o->specular.y,foos);
			script().add("b",&o->specular.z,foos);
			script().add("a",&o->specular.w,foos);

		LuaType* foopr=script().add("reflectivity",foo);
			script().add("r",&o->reflectivity.x,foopr);
			script().add("g",&o->reflectivity.y,foopr);
			script().add("b",&o->reflectivity.z,foopr);
			script().add("a",&o->reflectivity.w,foopr);

		LuaType* foouv0=script().add("uvLightmap",foo);
			LuaType* foouv0p=script().add("pos",foouv0);
			script().add("x",&o->texLightmapPos.x,foouv0p);
			script().add("y",&o->texLightmapPos.y,foouv0p);

			LuaType* foouv0s=script().add("scale",foouv0);
			script().add("x",&o->texLightmapScale.x,foouv0s);
			script().add("y",&o->texLightmapScale.y,foouv0s);

		LuaType* foouvlm=script().add("uv0",foo);
			LuaType* foouvlmp=script().add("pos",foouvlm);
			script().add("x",&o->tex0pos.x,foouvlmp);
			script().add("y",&o->tex0pos.y,foouvlmp);

			LuaType* foouvlms=script().add("scale",foouvlm);
			script().add("x",&o->tex0scale.x,foouvlms);
			script().add("y",&o->tex0scale.y,foouvlms);

		LuaType* foouv1=script().add("uv1",foo);
			LuaType* foouv1p=script().add("pos",foouv1);
			script().add("x",&o->tex1pos.x,foouv1p);
			script().add("y",&o->tex1pos.y,foouv1p);

			LuaType* foouv1s=script().add("scale",foouv1);
			script().add("x",&o->tex1scale.x,foouv1s);
			script().add("y",&o->tex1scale.y,foouv1s);

		LuaType* foouv2=script().add("uv2",foo);
			LuaType* foouv2p=script().add("pos",foouv2);
			script().add("x",&o->tex2pos.x,foouv2p);
			script().add("y",&o->tex2pos.y,foouv2p);

			LuaType* foouv2s=script().add("scale",foouv2);
			script().add("x",&o->tex2scale.x,foouv2s);
			script().add("y",&o->tex2scale.y,foouv2s);

		LuaType* foouv3=script().add("uv3",foo);
			LuaType* foouv3p=script().add("pos",foouv3);
			script().add("x",&o->tex3pos.x,foouv3p);
			script().add("y",&o->tex3pos.y,foouv3p);

			LuaType* foouv3s=script().add("scale",foouv3);
			script().add("x",&o->tex3scale.x,foouv3s);
			script().add("y",&o->tex3scale.y,foouv3s);

		LuaType* foouv4=script().add("uv4",foo);
			LuaType* foouv4p=script().add("pos",foouv4);
			script().add("x",&o->tex4pos.x,foouv4p);
			script().add("y",&o->tex4pos.y,foouv4p);

			LuaType* foouv4s=script().add("scale",foouv4);
			script().add("x",&o->tex4scale.x,foouv4s);
			script().add("y",&o->tex4scale.y,foouv4s);

		LuaType* foouv5=script().add("uv5",foo);
			LuaType* foouv5p=script().add("pos",foouv5);
			script().add("x",&o->tex5pos.x,foouv5p);
			script().add("y",&o->tex5pos.y,foouv5p);

			LuaType* foouv5s=script().add("scale",foouv5);
			script().add("x",&o->tex5scale.x,foouv5s);
			script().add("y",&o->tex5scale.y,foouv5s);

	return true;
}


bool Level::luaUpload(TextureImage* o){
	//if you don't want material control, you can just uncomment the next line
	//return true;

	if(o->name==""){
		return false;
	}

	LuaType* img=script().add(o->name,luaImage);
		script().add("name",o->name,img);
		LuaType* size=script().add("size",img);
			script().add("x",o->x,size);
			script().add("y",o->y,size);
		script().add("bytesPerPixel",o->bytesPP,img);
		script().add("getValue",luaGetImageValue,img);
		script().add("setValue",luaSetImageValue,img);
		script().add("save",luaSaveImage,img);

	return true;
}

static int luaGetVertexCount(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	String name;
	

	if(args.size()==1){
		if(args[0].type!=SCRIPT_TABLE){
			return 0;
		}

		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

	}else{
		console().write("ERROR: must be accessed as a class");
		return 0;
	}


	script().result(L,level->objectKey[name]->getVertexCount());
	return 1;
}

static int luaGetVertex(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	String name;
	int vid;
	

	if(args.size()==2){
		if(args[0].type!=SCRIPT_TABLE){
			return 0;
		}

		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

		if(args[1].type!=SCRIPT_DOUBLE_STORED){
			return 0;
		}

		vid=args[1].value.ds;
	}else{
		console().write("ERROR: must be accessed as a class");
		return 0;
	}
	Object* o=level->objectKey[name];

	if(o->getVertexCount()<vid){
		console().write("ERROR: bad vertex id.");
		return 0;
	}

	if(vid<1){
		console().write("ERROR: bad vertex id.");
		return 0;
	}

	Vertex* ver=&o->getVertexRaw(vid-1);

	script().resultTable(L);
	script().resultStartTable(L,"pos");
	script().result(L,"x",ver->pos.x);
	script().result(L,"y",ver->pos.y);
	script().result(L,"z",ver->pos.z);
	script().resultEndTable(L);
	return 1;
}


static int luaSetVertex(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	String name;
	int vid;
	
	FloatVector3d pos;

	if(args.size()==3){
		if(args[0].type!=SCRIPT_TABLE){
			console().write("ERROR: bad arguments.  must be accessed as a class");
			return 0;
		}

		for(int i=0; i<args[0].children.size(); i++){
			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}
		}

		if(args[1].type!=SCRIPT_DOUBLE_STORED){
			console().write("ERROR: bad arguments. no vertex id");
			return 0;
		}

		vid=args[1].value.ds;

		if(args[2].type!=SCRIPT_TABLE){
			console().write("ERROR: bad arguments. third argument should be a table.");
			return 0;
		}

		for(int i=0; i<args[2].children.size(); i++){
			if(args[2].children[i].name=="pos"){
				if(args[2].children[i].type==SCRIPT_TABLE){
					if(args[2].children[i].children.size()==3){
						for(int j=0; j<3; j++){
							if(args[2].children[i].children[j].name=="x"){
								pos.x=args[2].children[i].children[j].value.ds;
							}else if(args[2].children[i].children[j].name=="y"){
								pos.y=args[2].children[i].children[j].value.ds;
							}else if(args[2].children[i].children[j].name=="z"){
								pos.z=args[2].children[i].children[j].value.ds;
							}
						}
					}else{
						console().write("ERROR: bad arguments. pos does not have 3 elements");
						return 0;
					}

				}else{
					console().write("ERROR: bad arguments. pos is not a table.");
					return 0;
				}
			}
		}
	}else{
		console().write("ERROR: bad arguments.  must be accessed as a class");
		return 0;
	}
	Object* o=level->objectKey[name];

	if(o->getVertexCount()<vid){
		console().write("ERROR: bad vertex id.");
		return 0;
	}

	if(vid<1){
		console().write("ERROR: bad vertex id.");
		return 0;
	}

	Vertex* ver=&o->getVertexRaw(vid-1);

	ver->pos=pos;

	o->listBuilt=false;
	return 0;
}

static int luaAddPolygon(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	String name;

	Vertex v[3];
	

	if(args.size()!=2){
		console().write("add polygon should be accessed as a class!");
		return 0;
	}

	for(int i=0; i<args[0].children.size(); i++){

		if(args[0].children[i].name=="name"){
			name=args[0].children[i].value.ss;
		}

	}

	if(name==""){
		console().write("error, name not specified!");
		return 0;
	}

	if(args[1].children.size()==3){
		for(int vindex=0; vindex<3; vindex++){

			int vertex=args[1].children[vindex].nameint-1;

			if(vertex<0 || vertex>2){
				console().write("vertex table index error, indices should be 1,2, and 3");
				return 0;
			}

			for(int posindex=0; posindex<args[1].children[vindex].children.size(); posindex++){

				if(args[1].children[vindex].children[posindex].name=="pos"){

					for(int xindex=0; xindex<args[1].children[vindex].children[posindex].children.size(); xindex++){
						if(args[1].children[vindex].children[posindex].children[xindex].name=="x"){
							v[vertex].pos.x=args[1].children[vindex].children[posindex].children[xindex].value.ds;
						}
					}

					for(int yindex=0; yindex<args[1].children[vindex].children[posindex].children.size(); yindex++){
						if(args[1].children[vindex].children[posindex].children[yindex].name=="y"){
							v[vertex].pos.y=args[1].children[vindex].children[posindex].children[yindex].value.ds;
						}
					}

					for(int zindex=0; zindex<args[1].children[vindex].children[posindex].children.size(); zindex++){
						if(args[1].children[vindex].children[posindex].children[zindex].name=="z"){
							v[vertex].pos.z=args[1].children[vindex].children[posindex].children[zindex].value.ds;
						}
					}
				}
			}

			for(int normalindex=0; normalindex<args[1].children[vindex].children.size(); normalindex++){
				if(args[1].children[vindex].children[normalindex].name=="normal"){


					for(int xindex=0; xindex<args[1].children[vindex].children[normalindex].children.size(); xindex++){
						if(args[1].children[vindex].children[normalindex].children[xindex].name=="x"){
							v[vertex].normal.x=args[1].children[vindex].children[normalindex].children[xindex].value.ds;
						}
					}

					for(int yindex=0; yindex<args[1].children[vindex].children[normalindex].children.size(); yindex++){
						if(args[1].children[vindex].children[normalindex].children[yindex].name=="y"){
							v[vertex].normal.y=args[1].children[vindex].children[normalindex].children[yindex].value.ds;
						}
					}

					for(int zindex=0; zindex<args[1].children[vindex].children[normalindex].children.size(); zindex++){
						if(args[1].children[vindex].children[normalindex].children[zindex].name=="z"){
							v[vertex].normal.z=args[1].children[vindex].children[normalindex].children[zindex].value.ds;
						}
					}
				}
			}

			for(int uvindex=0; uvindex<args[1].children[vindex].children.size(); uvindex++){
				if(args[1].children[vindex].children[uvindex].name=="uv"){


					for(int xindex=0; xindex<args[1].children[vindex].children[uvindex].children.size(); xindex++){
						if(args[1].children[vindex].children[uvindex].children[xindex].name=="x"){
							v[vertex].uvLightmap.x=args[1].children[vindex].children[uvindex].children[xindex].value.ds;
							v[vertex].uv0.x=args[1].children[vindex].children[uvindex].children[xindex].value.ds;
						}
					}

					for(int yindex=0; yindex<args[1].children[vindex].children[uvindex].children.size(); yindex++){
						if(args[1].children[vindex].children[uvindex].children[yindex].name=="y"){
							v[vertex].uvLightmap.y=args[1].children[vindex].children[uvindex].children[yindex].value.ds;
							v[vertex].uv0.y=args[1].children[vindex].children[uvindex].children[yindex].value.ds;
						}
					}

				}
			}
			
		}
	}else{
		console().write("error, should be a triangle");
		return 0;
	}


	//calculate tangent+binormal
	{
		FloatVector3d tt;

		const FloatVector3d& v1 = v[0].pos;
		const FloatVector3d& v2 = v[1].pos;
		const FloatVector3d& v3 = v[2].pos;
		
		const FloatVector2d& w1 = v[0].uv0;
		const FloatVector2d& w2 = v[1].uv0;
		const FloatVector2d& w3 = v[2].uv0;
		
		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;
		
		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;
		

		float r = 1.0F / (s1 * t2 - s2 * t1);
		FloatVector3d sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				(t2 * z1 - t1 * z2) * r);
		FloatVector3d tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				(s1 * z2 - s2 * z1) * r);
		


		tt = (sdir - v[0].normal * (v[0].normal * sdir)).normalize();
		v[0].tangent=tt;
		tt = (sdir - v[1].normal * (v[1].normal * sdir)).normalize();
		v[1].tangent=tt;
		tt = (sdir - v[2].normal * (v[2].normal * sdir)).normalize();
		v[2].tangent=tt;
		
		float hand=1;

		hand=(v[0].normal % sdir * tdir < 0.0F) ? -1.0F : 1.0F;
		v[0].binormal=math.crossProduct(v[0].normal,v[0].tangent)*hand;
		hand=(v[1].normal % sdir * tdir < 0.0F) ? -1.0F : 1.0F;
		v[1].binormal=math.crossProduct(v[1].normal,v[1].tangent)*hand;
		hand=(v[2].normal % sdir * tdir < 0.0F) ? -1.0F : 1.0F;
		v[2].binormal=math.crossProduct(v[2].normal,v[2].tangent)*hand;

	}

	Object* t=level->objectKey[name];

	if(t->getVertexCount()==0){
		v[0].end=START_TRI;
		v[2].end=END_TRI;
	}else if(t->getVertexRaw(t->getVertexCount()-1).end==END_TRI){
		t->getVertexRaw(t->getVertexCount()-1).end=0;
		v[2].end=END_TRI;
	}

	t->addVertex(v[0]);
	t->addVertex(v[1]);
	t->addVertex(v[2]);
	t->setVertexCount(t->getVertexCount()+3);

	t->type=0;	//0==polymesh
	
	return 0;
}

bool Level::applyMaterial(String objectname, String mat){

	Object* t=level->objectKey[objectname];

	if(t->material!=-1){
		level->materials[t->material].nonstaticRefs--;
		level->materials[t->material].nonstaticRefList.pushBack(t);
	}

	t->material=level->materialIndexKey[mat];
	level->materials[t->material].nonstaticRefs++;
	level->materials[t->material].nonstaticRefList.pushBack(t);

	return true;
}

static int luaApplyMaterial(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	String name;
	String mat;

	if(args.size()==2){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

		if(args[1].type==SCRIPT_STRING_STORED){
			mat=args[1].value.ss;
		}
	}

	if(!level->materialKey.find(mat)){

		console().write("Error, material '"+mat+"' not found");
		return 0;
	}

	

	level->applyMaterial(name,mat);

	return 0;
}


static int luaMaterial(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	String name;
	String mat;

	if(args.size()==2){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

		if(args[1].type==SCRIPT_STRING_STORED){
			mat=args[1].value.ss;
		}

	}else if(args.size()==1){
		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

		Object* t=level->objectKey[name];

		script().result(L,level->materials[t->material].name);
		return 1;
	}else{
		console().write("Error, material() bad arguments, must be accessed as a class");
		return 0;
	}


	if(!level->materialKey.find(mat)){

		console().write("Error, material '"+mat+"' not found");
		return 0;
	}

	Object* t=level->objectKey[name];

	if(t->material!=-1){
		level->materials[t->material].nonstaticRefs--;
		level->materials[t->material].nonstaticRefList.pushBack(t);
	}

	t->material=level->materialIndexKey[mat];
	level->materials[t->material].nonstaticRefs++;
	level->materials[t->material].nonstaticRefList.pushBack(t);

	return 0;
}


bool Level::luaUpload(Object* o){
	LuaType* foo=script().add(o->name,luaObj);

		if(o->flags & OBJECT_INSIDEBB){
			script().add("insidebb",&o->insidebb,foo);
		}

		script().add("name",&o->name,foo);
		script().add("visible",&o->visible,foo);
		script().add("collide",&o->collide,foo);
		script().add("getVertexCount",&luaGetVertexCount,foo);
		script().add("getVertex",&luaGetVertex,foo);
		script().add("setVertex",&luaSetVertex,foo);
		script().add("addPolygon",&luaAddPolygon,foo);
		script().add("material",&luaMaterial,foo);
		script().add("lightmap",&luaLightmap,foo);
		script().add("deleteLightmap",&luaDeleteLightmap,foo);
		script().add("rotMode",&o->rotMode,foo);

		#ifdef ODE
			script().add("mass",&o->mass,foo);
			script().add("friction",&o->friction,foo);
			script().add("bounce",&o->bounce,foo);
			script().add("bounceThreshold",&o->bounceThreshold,foo);
			script().add("dynamic",&o->dynamic,foo);
			script().add("collideType",&o->collideType,foo);
		#endif

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foor=script().add("rot",foo);
			script().add("x",&o->rot.x,foor);
			script().add("y",&o->rot.y,foor);
			script().add("z",&o->rot.z,foor);

		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);
			script().add("z",&o->scale.z,foos);


	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}

bool Level::luaUpload(NullObject* o){

	LuaType* foo=script().add(o->name,luaObj);

		script().add("visible",&o->visible,foo);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foor=script().add("rot",foo);
			script().add("x",&o->rot.x,foor);
			script().add("y",&o->rot.y,foor);
			script().add("z",&o->rot.z,foor);

		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);
			script().add("z",&o->scale.z,foos);

	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}

bool Level::luaUpload(Disk* o){
	
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		script().add("visible",&o->visible,foo);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foopu=script().add("detail",foo);
			script().add("u",&o->detail.x,foopu);
			script().add("v",&o->detail.y,foopu);

		LuaType* foopue=script().add("extent",foo);
			script().add("theta",&o->extent.x,foopue);
			script().add("phi",&o->extent.y,foopue);

		LuaType* foor=script().add("rot",foo);
			script().add("x",&o->rot.x,foor);
			script().add("y",&o->rot.y,foor);
			script().add("z",&o->rot.z,foor);

		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);
			script().add("z",&o->scale.z,foos);


	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}


bool Level::luaUpload(Instance* o){
	
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		script().add("link",&luaInstanceLink,foo);

	o->luaObj=foo;

	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}


bool Level::luaUpload(Sphere* o){

	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		script().add("visible",&o->visible,foo);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foopu=script().add("detail",foo);
			script().add("u",&o->detail.x,foopu);
			script().add("v",&o->detail.y,foopu);

		LuaType* foopue=script().add("extent",foo);
			script().add("theta",&o->extent.x,foopue);
			script().add("phi",&o->extent.y,foopue);

		LuaType* foor=script().add("rot",foo);
			script().add("x",&o->rot.x,foor);
			script().add("y",&o->rot.y,foor);
			script().add("z",&o->rot.z,foor);

		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);
			script().add("z",&o->scale.z,foos);


	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}

bool Level::luaUpload(Curve* o){

	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		script().add("visible",&o->visible,foo);
		script().add("getPos",&luaGetCurvePos,foo);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foor=script().add("rot",foo);
			script().add("x",&o->rot.x,foor);
			script().add("y",&o->rot.y,foor);
			script().add("z",&o->rot.z,foor);

		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);
			script().add("z",&o->scale.z,foos);


	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}





bool Level::luaUpload(Line3d* o){
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		
		script().add("thickness",&o->thickness,foo);
		script().add("visible",&o->visible,foo);
		
		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foops=script().add("scale",foo);
			script().add("x",&o->scale.x,foops);
			script().add("y",&o->scale.y,foops);
			script().add("z",&o->scale.z,foops);
			
		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);


		
	return true;
}


bool Level::luaUpload(Line2d* o){
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		
		script().add("thickness",&o->thickness,foo);
		script().add("visible",&o->visible,foo);
		script().add("percent",&o->percent,foo);
		script().add("keepAspectRatio",&o->keepAspectRatio,foo);
		
		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foops=script().add("scale",foo);
			script().add("x",&o->scale.x,foops);
			script().add("y",&o->scale.y,foops);
			script().add("z",&o->scale.z,foops);
			
		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);
		
	return true;
}


bool Level::luaUpload(Box* o){
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		
		script().add("visible",&o->visible,foo);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foops=script().add("scale",foo);
			script().add("x",&o->scale.x,foops);
			script().add("y",&o->scale.y,foops);
			script().add("z",&o->scale.z,foops);
			
		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);


		
	return true;
}

bool Level::luaUpload(Circle* o){
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		
		script().add("thickness",&o->thickness,foo);
		script().add("visible",&o->visible,foo);
		script().add("percent",&o->percent,foo);
		script().add("keepAspectRatio",&o->keepAspectRatio,foo);
		
		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foops=script().add("scale",foo);
			script().add("x",&o->scale.x,foops);
			script().add("y",&o->scale.y,foops);
			
		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);
		
	return true;
}

bool Level::luaUpload(Point3d* o){
	LuaType* foo=script().add(o->name,luaObj);

		script().add("name",&o->name,foo);
		
		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		script().add("scale",&o->scale,foo);
		script().add("visible",&o->visible,foo);

		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);


		
	return true;
}

bool Level::luaUpload(Camera* o){
	return false;
}


bool Level::luaUpload(Light* o){

	LuaType* foo=script().add(o->name,luaObj);
	
	script().add("name",&o->name,foo);
	script().add("shadow",&o->shadow,foo);
	script().add("shadowQuality",&o->shadowQuality,foo);
	script().add("umbra",&o->umbra,foo);
	script().add("intensity",&o->intensity,foo);
	
	
	if(o->type==LIGHT_SPOT){
		script().add("cone",&o->coneAngle,foo);
		script().add("spread",&o->coneSpread,foo);
	}


		LuaType* foof=script().add("falloff",foo);
			script().add("start",&o->falloffStart,foof);
			script().add("stop",&o->falloffEnd,foof);	//can't be end, that's a lua keyword

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foor=script().add("rot",foo);
			script().add("x",&o->rot.x,foor);
			script().add("y",&o->rot.y,foor);
			script().add("z",&o->rot.z,foor);


		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);

		return true;
}

bool Level::luaUpload(Panel* o){

	LuaType* foo=script().add(o->name,luaObj);
	
	script().add("load",&luaPanelLoad,foo);
	script().add("loadMouseover",&luaPanelLoadMouseover,foo);
	script().add("enableMouseover",&luaPanelEnableMouseover,foo);
	script().add("disableMouseover",&luaPanelDisableMouseover,foo);
	script().add("scissor",&luaPanelScissor,foo);
	script().add("scissorOff",&luaPanelScissorOff,foo);
	script().add("name",&o->name,foo);
	script().add("mouseoverFade",&o->mouseoverFade,foo);
	script().add("mouseover",&o->mouseover,foo);
	script().add("percent",&o->percent,foo);	
	script().add("center",&o->center,foo);	
	script().add("keepAspectRatio",&o->keepAspectRatio,foo);	
	script().add("visible",&o->visible,foo);
	script().add("clickable",&o->clickable,foo);
	
	script().add("smartCenter",&o->smartCenter,foo);

		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);

		LuaType* foosp=script().add("pivot",foo);
			script().add("x",&o->pivot.x,foosp);
			script().add("y",&o->pivot.y,foosp);


		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

		LuaType* foor=script().add("rot",foo);
			script().add("z",&o->rot.z,foor);

	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}



bool Level::luaUpload(TextPanel* o){

	LuaType* foo=script().add(o->name,luaObj);

		script().add("font",&luaTextPanelFont,foo);
		script().add("scissor",&luaTextPanelScissor,foo);
		script().add("name",&o->name,foo);
		script().add("text",&luaTextPanelText,foo);
		script().add("percent",&o->percent,foo);	
		script().add("center",&o->center,foo);	
		script().add("mouseover",&o->mouseover,foo);	
		script().add("keepAspectRatio",&o->keepAspectRatio,foo);	
		script().add("smartCenter",&o->smartCenter,foo);	
		script().add("visible",&o->visible,foo);
		script().add("clickable",&o->clickable,foo);
		script().add("align",&o->align,foo);	

		LuaType* fooc=script().add("color",foo);
			script().add("r",&o->color.x,fooc);
			script().add("g",&o->color.y,fooc);
			script().add("b",&o->color.z,fooc);
			script().add("a",&o->color.w,fooc);

		LuaType* foos=script().add("scale",foo);
			script().add("x",&o->scale.x,foos);
			script().add("y",&o->scale.y,foos);

		LuaType* foop=script().add("pos",foo);
			script().add("x",&o->pos.x,foop);
			script().add("y",&o->pos.y,foop);
			script().add("z",&o->pos.z,foop);

	script().syncFrom(NULL);
	script().syncTo(NULL);

	return true;
}


int luaLoad(lua_State *L){

	String files=luaL_checkstring(L,1);

	Path file;
	file.setRelative(files);

	if(!level->load(file)){
		os().error(String("Error: could not load level '"+file.getRelative()+"'").cStr());
		return false;
	}

	return 0;
}


int luaSave(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=2){
		console().write("level.save error, needs two arguments");
		return 0;
	}

	String files=args[0].value.ss;
	bool developer=args[1].value.bs;

	Path file;
	file.setRelative(files);

	if(!level->save(file, developer)){
		os().error(String("Error: could not load level '"+file.getRelative()+"'").cStr());
		return false;
	}

	return 0;
}

int luaLevelNew(lua_State *L){
	newLevel();
	return 0;
}

int luaLevelDelete(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		console().write("Error, bad arguments to levelManager.delete()");
		return 0;
	}

	if(args[0].type!=SCRIPT_STRING_STORED){
		console().write("Error, bad arguments to levelManager.delete()");
		return 0;
	}

	if(args[0].value.ss==level->name){
		console().write("Error, can not delete the level '"+level->name+"' because it is the current level.");
		return 0;
	}

	deleteLevel(args[0].value.ss);

	return 0;
}

int luaLevelList(lua_State *L){
	
	if(loadedLevels.size()==0){
		return 0;
	}

	script().resultTable(L);

	for(int i=0; i<loadedLevels.size(); i++){
		script().result(L,i,loadedLevels[i]->name);
	}

	return 1;
}


int luaLevelHide(lua_State *L){
	String name=luaL_checkstring(L,1);

	hideLevel(name);

	return 0;
}


int luaLevelSwap(lua_State *L){

	String level1=luaL_checkstring(L,1);
	String level2=luaL_checkstring(L,2);

	int index1=-1;
	int index2=-1;

	for(int i=0; i<loadedLevels.size(); i++){
		if(loadedLevels[i]->name==level1){
			index1=i;
		}
	}

	for(int i=0; i<loadedLevels.size(); i++){
		if(loadedLevels[i]->name==level2){
			index2=i;
		}
	}

	Level *tmp=loadedLevels[index1];
	loadedLevels[index1]=loadedLevels[index2];
	loadedLevels[index2]=tmp;

	return 0;
}

int luaLevelShow(lua_State *L){
	String name=luaL_checkstring(L,1);
	showLevel(name);
	return 0;
}




int luaMask(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()==1){
		if(args[0].type==SCRIPT_STRING_STORED){

			level->maskLevel=args[0].value.ss;
		}
	}

	return 0;
}

int luaAdd(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	String name;
	String type;

	if(args.size()==1){

		if(args[0].type==SCRIPT_TABLE_INT || args[0].type==SCRIPT_TABLE){
			if(args[0].children.size()==2){

				if(args[0].children[0].type==SCRIPT_STRING_STORED){
					if(args[0].children[0].name=="name"){
						name=args[0].children[0].value.ss;
					}else if(args[0].children[0].name=="type"){
						type=args[0].children[0].value.ss;
					}
				}

				if(args[0].children[1].type==SCRIPT_STRING_STORED){
					if(args[0].children[1].name=="name"){
						name=args[0].children[1].value.ss;
					}else if(args[0].children[1].name=="type"){
						type=args[0].children[1].value.ss;
					}
				}
			}

		}

	}else if(args.size()==2){
		if(args[0].type==SCRIPT_STRING_STORED && args[1].type==SCRIPT_STRING_STORED){
			name=args[0].value.ss;
			type=args[1].value.ss;
		}else{
			console().write("add error, bad arguments");
			return 0;
		}
		
	}else{
		console().write("add error, bad arguments");
		return 0;
	}

	level->add(name,type);

	return 0;
}

int luaCopy(lua_State *L){
	String obj1=luaL_checkstring(L,1);	
	String obj2=luaL_checkstring(L,2);

	level->copy(obj1,obj2);
	
	return 0;
}

int luaDelete(lua_State *L){
	String name=luaL_checkstring(L,1);
	level->remove(name);
	return 0;
}

int luaRename(lua_State *L){
	String namefrom=luaL_checkstring(L,1);
	String nameto=luaL_checkstring(L,2);

	level->rename(namefrom,nameto);

	return 0;
}

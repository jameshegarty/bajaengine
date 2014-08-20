 #include "level.hpp"


#include <iostream>
#include <fstream>

#include "GlHeader.hpp"

#include "Helperlib/Log.hpp"
#include "Helperlib/HelperLibMath.hpp"

#include "FileIo.hpp"
#include "bmp.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "light.h"
#include "filters.h"
#include "conf.h"

#include "foam.hpp"
#include "draw.hpp"

#include "os.h"
#include "timeing.hpp"

using namespace std;

/* NOTES:
* changing the format can cause weird stuff to happen becuase its anal
*
*/

#ifndef LOAD_PROGRESSIVE

bool Level::load(String infile){

	engineTime.startTimerSingle("TOTALLOAD");

	water=NULL;

	//FastStream in (infile.c_str(), ios::in|ios::binary);
	ifstream in (infile.c_str(), ios::in|ios::binary);
  	
	if(in==NULL){


		logs().main << "Could not open file: '" << infile << "'" << endLine;
		return false;
	}

	file=infile;
	//in.cache();

	//unsigned long numObjects;
	unsigned long startTextures;
	unsigned long startMaterials;
	unsigned long startLightmaps;
	unsigned long startObjects;

	//check header
	in.seekg (0, ios::beg);

	char* header=new char[4];
	in.read (header, 4);
	


	loadingScreen();

	if(header[0]!='L' ||
		header[1]!='M' ||
		header[2]!='L' ||
		header[3]!='1' ||
		file.substr(file.size()-3,3)!="lml"){
		logs().main << String("unknown level format :") << String(header)  << endLine;

		//logs().main << String("unknown level format :") << file.substr(file.size()-3,3)  << endLine;
		return false;
	}

	//in.seekg (8);

	in.read ((char*)&textureCount, 4);
	textures=new TextureImage[textureCount];
	logs().main << "textureCount:" << textureCount << endLine;
	
	in.read ((char*)&materialCount, 4);
	//materials=new Material[materialCount];
	materials.reserve(materialCount);
	for(int i=0; i<materialCount; i++){
		materials.pushBack();
	}
	logs().main << "materialcount:" << materialCount << endLine;

	in.read ((char*)&lightmapCount, 4);
	lightmaps=new TextureImage[lightmapCount];
	logs().main << "lightmapCount:" << lightmapCount << endLine;

	in.read ((char*)&objectCount, 4);
	//objects=new Object[objectCount];
	logs().main << "objectcount:" << objectCount << endLine;

	in.read ((char*)&startTextures, 4);
	in.read ((char*)&startMaterials, 4);
	in.read ((char*)&startLightmaps, 4);
	in.read ((char*)&startObjects, 4);

	/////////////////////////////////////////////////////////////////////////////////////////////////start reading textures
	engineTime.startTimerSingle("TEXTURES");
	in.seekg (startTextures, ios::beg);

	unsigned long* texIndices=new unsigned long[textureCount];

	for(unsigned int i=0; i<textureCount; i++){
		//indices[i];
		
		in.read ((char*)&texIndices[i], 4);
		
	}

	for(unsigned int i=0; i<textureCount; i++){
		in.seekg (texIndices[i]);
		
		//in.read ((char*)&objectSize, 4);	//size

		in.read ((char*)&textures[i].ref,sizeof(textures[i].ref));	

		if(textures[i].ref){
			
			in.read ((char*)&textures[i].flags,sizeof(textures[i].flags));	
			//in.read ((char*)&textures[i].flags,sizeof(unsigned int));	
			textures[i].file.read(&in);


			textures[i].fileRelative.read(&in);
			//in.read ((char*)&textures[i].flags,sizeof(textures[i].flags));	
			

			String fi=textures[i].file;
			Path f;
			f.setRelative(fi);
			
			if(!os().fileExists(f)){
			//if(true){
				logs().renderer.write("texture file doesn't exist:"+f.getAbsolute());

			//	if(true){
				f=textures[i].fileRelative;

				String tr=infile;
				tr.replace("/","\\");
				Array<String> source=tr.explode("\\");
				Array<String> dest=String(os().executableFile).explode("\\");

				if(source[0]!=dest[0]){
					//infile probably just relative
					tr=os().executableDirectory+tr;
					source=tr.explode("\\");
				}

				while(source[0]==dest[0]){
					source.erase(0);
					dest.erase(0);
				}

				for(int i=0; i<dest.size()-1; i++){
					f="..\\"+f;
				}

				for(int i=0; i<source.size()-1; i++){
					f=f.substrIndex(3,f.size());
				}

				//logs().main.write("TRYPATH:"+f);
				f.replace("\\","/");

				logs().renderer.write("try to load similar file:"+f);
			}

			if(!os().fileExists(f)){
				logs().renderer.write("similar file doesn't exist:"+f);
				return false;
			}

			ImageData imagedata=loadImage(f);
			
			if(imagedata.loadSuccess){
					textures[i].x=imagedata.width;
					textures[i].y=imagedata.height;
					textures[i].data=imagedata.data;
					textures[i].bytesPP=imagedata.bytesPP;

					if(!checkSize(textures[i].x) || !checkSize(textures[i].y)){
						String s1="Error, non power of 2 texture '"+textures[i].file + "' x:" + String(textures[i].x ) + " y:" + String(textures[i].y);
						String s2="Make sure image is rgb and not grayscale";

						logs().main.write(s1);
						logs().main.write(s2);

						os().error("Non power of 2 texture error, see log");

						return false;
					}

					if(textures[i].flags & TEXTURE_NORMAL){ 
						

						ImageData t;

						t.width=textures[i].x;
						t.height=textures[i].y;
						t.data=textures[i].data;
						t.bytesPP=textures[i].bytesPP;
						t.bitsPP=textures[i].bytesPP*8;
						t.size=textures[i].bytesPP*textures[i].x*textures[i].y;

						ImageData f=sobel(t);
						textures[i].data=f.data;
					}
			}else{

					String s1="could not load texture file '"+textures[i].file+ "'";
					String s2="Error: "+imagedata.error;

					logs().main.write(s1);
					logs().main.write(s2);
					return false;
			}
		}else{
			in.read ((char*)&textures[i].x,sizeof(textures[i].x));	
			in.read ((char*)&textures[i].y,sizeof(textures[i].y));	
			in.read ((char*)&textures[i].bytesPP,sizeof(textures[i].bytesPP));	

			unsigned long tempsize=textures[i].x*textures[i].y*textures[i].bytesPP;

			textures[i].data=new unsigned char[tempsize];

			in.read ((char*)textures[i].data,tempsize);	
		}
	}

	logs().main.write("TEXTURES:"+String(engineTime.getTimerSingle("TEXTURES")));

	/////////////////////////////////////////////////////////////////////////////////////////////////start reading materials
	engineTime.startTimerSingle("MATERIALS");
	in.seekg (startMaterials, ios::beg);

	unsigned long* matIndices=new unsigned long[materialCount];

	for(unsigned int i=0; i<materialCount; i++){
		in.read ((char*)&matIndices[i], 4);
	}

	for(unsigned int i=0; i<materialCount; i++){
		in.seekg (matIndices[i]);

		materials[i].name.read(&in);

		in.read ((char*)&materials[i].shader,sizeof(materials[i].shader));	

		in.read ((char*)&materials[i].diffuse,sizeof(materials[i].diffuse));	
		in.read ((char*)&materials[i].specular,sizeof(materials[i].specular));
		in.read ((char*)&materials[i].ambient,sizeof(materials[i].ambient));	
		in.read ((char*)&materials[i].reflectivity,sizeof(materials[i].reflectivity));	

		in.read ((char*)&materials[i].tex0pos,sizeof(materials[i].tex0pos));	
		in.read ((char*)&materials[i].tex0scale,sizeof(materials[i].tex0scale));

		in.read ((char*)&materials[i].tex1pos,sizeof(materials[i].tex1pos));	
		in.read ((char*)&materials[i].tex1scale,sizeof(materials[i].tex1scale));

		in.read ((char*)&materials[i].tex2pos,sizeof(materials[i].tex2pos));	
		in.read ((char*)&materials[i].tex2scale,sizeof(materials[i].tex2scale));

		in.read ((char*)&materials[i].tex3pos,sizeof(materials[i].tex3pos));	
		in.read ((char*)&materials[i].tex3scale,sizeof(materials[i].tex3scale));

		in.read ((char*)&materials[i].tex4pos,sizeof(materials[i].tex4pos));	
		in.read ((char*)&materials[i].tex4scale,sizeof(materials[i].tex4scale));

		in.read ((char*)&materials[i].tex5pos,sizeof(materials[i].tex5pos));	
		in.read ((char*)&materials[i].tex5scale,sizeof(materials[i].tex5scale));

		in.read ((char*)&materials[i].tex6pos,sizeof(materials[i].tex6pos));	
		in.read ((char*)&materials[i].tex6scale,sizeof(materials[i].tex6scale));

		in.read ((char*)&materials[i].shiny,sizeof(materials[i].shiny));	
		in.read ((char*)&materials[i].textureDiffuse,sizeof(materials[i].textureDiffuse));
		in.read ((char*)&materials[i].textureDisplacement,sizeof(materials[i].textureDisplacement));
		in.read ((char*)&materials[i].textureSpecular,sizeof(materials[i].textureSpecular));
		in.read ((char*)&materials[i].textureBump,sizeof(materials[i].textureBump));

		in.read ((char*)&materials[i].textureEnvironmentFront,sizeof(materials[i].textureEnvironmentFront));
		in.read ((char*)&materials[i].textureEnvironmentBack,sizeof(materials[i].textureEnvironmentBack));
		in.read ((char*)&materials[i].textureEnvironmentLeft,sizeof(materials[i].textureEnvironmentLeft));
		in.read ((char*)&materials[i].textureEnvironmentRight,sizeof(materials[i].textureEnvironmentRight));
		in.read ((char*)&materials[i].textureEnvironmentTop,sizeof(materials[i].textureEnvironmentTop));
		in.read ((char*)&materials[i].textureEnvironmentBottom,sizeof(materials[i].textureEnvironmentBottom));


		if(materials[i].textureDiffuse>(int)textureCount ||
			materials[i].textureDisplacement>(int)textureCount ||
			materials[i].textureSpecular>(int)textureCount ||
			materials[i].textureBump>(int)textureCount ||
			materials[i].textureEnvironmentFront>(int)textureCount ||
			materials[i].textureEnvironmentBack>(int)textureCount ||
			materials[i].textureEnvironmentLeft>(int)textureCount ||
			materials[i].textureEnvironmentRight>(int)textureCount ||
			materials[i].textureEnvironmentTop>(int)textureCount ||
			materials[i].textureEnvironmentBottom>(int)textureCount){


				logs().main.write("Material ["+String(i)+" - "+materials[i].name+"]points to bad texture.  There must be an error in the file.  Try rebuilding the level (maybe it's in an old format or something)");

			return false;
		}

		//set up the cube maps (if there are any)

		if(materials[i].textureEnvironmentFront!=-1 &&
			materials[i].textureEnvironmentBack!=-1 &&
			materials[i].textureEnvironmentLeft!=-1 &&
			materials[i].textureEnvironmentRight!=-1 &&
			materials[i].textureEnvironmentTop!=-1 &&
			materials[i].textureEnvironmentBottom!=-1 &&
			!(materials[i].shader & MATERIAL_SHADER_MIXER)){


			unsigned int gid=0;
			glGenTextures(1, &gid);

			textures[materials[i].textureEnvironmentFront].flags|=TEXTURE_CUBE_FRONT;
			textures[materials[i].textureEnvironmentBack].flags|=TEXTURE_CUBE_BACK;
			textures[materials[i].textureEnvironmentLeft].flags|=TEXTURE_CUBE_LEFT;
			textures[materials[i].textureEnvironmentRight].flags|=TEXTURE_CUBE_RIGHT;
			textures[materials[i].textureEnvironmentTop].flags|=TEXTURE_CUBE_TOP;
			textures[materials[i].textureEnvironmentBottom].flags|=TEXTURE_CUBE_BOTTOM;

			textures[materials[i].textureEnvironmentFront].glName(gid);
			textures[materials[i].textureEnvironmentBack].glName(gid);
			textures[materials[i].textureEnvironmentLeft].glName(gid);
			textures[materials[i].textureEnvironmentRight].glName(gid);
			textures[materials[i].textureEnvironmentTop].glName(gid);
			textures[materials[i].textureEnvironmentBottom].glName(gid);
		}
	}

	logs().main.write("MATERIALS:"+String(engineTime.getTimerSingle("MATERIALS")));

	/////////////////////////////////////////////////////////////////////////////////////////////////start reading lightmaps
	engineTime.startTimerSingle("LIGHTMAPS");
	in.seekg (startLightmaps, ios::beg);

	unsigned long* lightmapIndices=new unsigned long[lightmapCount];

	for(unsigned int i=0; i<lightmapCount; i++){
		in.read ((char*)&lightmapIndices[i], 4);
	}

	for(unsigned int i=0; i<lightmapCount; i++){
		in.seekg (lightmapIndices[i]);

		in.read ((char*)&lightmaps[i].ref,sizeof(lightmaps[i].ref));

		if(lightmaps[i].ref){
			in.read ((char*)&lightmaps[i].flags,sizeof(lightmaps[i].flags));
			lightmaps[i].file.read(&in);
			lightmaps[i].fileRelative.read(&in);
			String f=lightmaps[i].file;
			if(true){
				logs().renderer.write("lightmap file doesn't exist:"+f);
				f=lightmaps[i].fileRelative;

				String tr=infile;	//the position of the lml file

				if(f.contains("\\")==false && f.contains("/")==false){
					//it's in the same directory as the lml

					tr.replace("/","\\");
					Array<String> source=tr.explode("\\");

					String final;

					for(int i=0; i<source.size()-1; i++){
						final=source[i]+"/";
					}

					f=final+f;

				}else{

					tr.replace("/","\\");
					Array<String> source=tr.explode("\\");
					Array<String> dest=String(os().executableFile).explode("\\");

					if(source[0]!=dest[0]){
						//infile probably just relative
						tr=os().executableDirectory+tr;
						source=tr.explode("\\");
					}

					while(source[0]==dest[0]){
						source.erase(0);
						dest.erase(0);
					}

					for(int i=0; i<dest.size()-1; i++){
						f="..\\"+f;
					}

					for(int i=0; i<source.size()-1; i++){
						f=f.substrIndex(3,f.size());
					}

					f.replace("\\","/");
				}

				logs().renderer.write("try to load similar file:"+f);
			}

			if(!os().fileExists(f)){
				logs().renderer.write("similar file doesn't exist:"+f);
				return false;
			}

			ImageData imagedata=loadImage(f);
			//e
			if(imagedata.loadSuccess){
					lightmaps[i].x=imagedata.width;
					lightmaps[i].y=imagedata.height;
					lightmaps[i].data=imagedata.data;
					lightmaps[i].bytesPP=imagedata.bytesPP;

					if(!checkSize(lightmaps[i].x) || !checkSize(lightmaps[i].y)){
						String s1="Error, non power of 2 lightmap '"+lightmaps[i].file + "' x:" + String(lightmaps[i].x ) + " y:" + String(lightmaps[i].y);
						String s2="Make sure image is rgb and not grayscale";

						logs().main.write(s1);
						logs().main.write(s2);
						return false;
					}

			}else{

					String s1="could not load lightmap file '"+lightmaps[i].file+ "'";
					String s2="Error: "+imagedata.error;

					logs().main.write(s1);
						logs().main.write(s2);
					return false;
			}

			if(lightmaps[i].x>2048 || lightmaps[i].y>2048 || lightmaps[i].bytesPP>4 || lightmaps[i].bytesPP<3){

				logs().main.write("bad lightmap size");
				return false;
			}

		}else{
			in.read ((char*)&lightmaps[i].x,sizeof(lightmaps[i].x));	
			in.read ((char*)&lightmaps[i].y,sizeof(lightmaps[i].y));	
			in.read ((char*)&lightmaps[i].bytesPP,sizeof(lightmaps[i].bytesPP));

			
			if(lightmaps[i].x>2048 || lightmaps[i].y>2048 || lightmaps[i].bytesPP>4 || lightmaps[i].bytesPP<3){

				logs().main.write("bad lightmap size");
				return false;
			}

			unsigned long tempsize=lightmaps[i].x*lightmaps[i].y*lightmaps[i].bytesPP;

			lightmaps[i].data=new unsigned char[tempsize];

			in.read ((char*)lightmaps[i].data,tempsize);	

		}

		if(!texture.load(&lightmaps[i])){
			logs().main.write("error loading lightmap texture "+(String)i);
			return false;
		}

		delete[] lightmaps[i].data;

	}
	logs().main.write("LIGHTMAPS:"+String(engineTime.getTimerSingle("LIGHTMAPS")));

	///////////////////////////////////////////////////////////////////////////////////////////////start reading objects
	engineTime.startTimerSingle("OBJECTS");
	in.seekg (startObjects, ios::beg);

	
	//read object positions
	unsigned long* indices=new unsigned long[objectCount];

	for(int i=0; i<objectCount; i++){
		in.read ((char*)&indices[i], 4);
	}

	//get objects

	Object* currentObject=NULL;
	NullObject* currentNull=NULL;
	Light* currentLight=NULL;
	
	for(unsigned int i=0; i<objectCount; i++){
		currentObject=new Object;

		in.seekg (indices[i]);
		
		in.read ((char*)&currentObject->type,sizeof(currentObject->type));	//type
		in.read ((char*)&currentObject->flags,sizeof(currentObject->flags));	//flags
		in.read ((char*)&currentObject->material,sizeof(currentObject->material));	//flags
		in.read ((char*)&currentObject->lightmap,sizeof(currentObject->lightmap));	//flags
		
		

		currentObject->name.read(&in);
		currentObject->action.read(&in);

		if(currentObject->type==0){
			in.read ((char*)&currentObject->pos,sizeof(currentObject->pos));	
			in.read ((char*)&currentObject->rot,sizeof(currentObject->rot));	
			in.read ((char*)&currentObject->scale,sizeof(currentObject->scale));	

			int vc;
			in.read ((char*)&vc,sizeof(vc));

			currentObject->setVertexCount(vc);

			//read vertices
			if(currentObject->getVertexCount()>0){
				currentObject->reserveVertices(currentObject->getVertexCount());
				for(int i=0; i<currentObject->getVertexCount(); i++){
					currentObject->addVertex();
				}
			}

			for(unsigned int v=0; v<currentObject->getVertexCount(); v++){
				in.read ((char*)&currentObject->getVertexRaw(v).pos,sizeof(FloatVector3d));
				in.read ((char*)&currentObject->getVertexRaw(v).normal,sizeof(FloatVector3d));
				in.read ((char*)&currentObject->getVertexRaw(v).binormal,sizeof(FloatVector3d));
				in.read ((char*)&currentObject->getVertexRaw(v).tangent,sizeof(FloatVector3d));
				in.read ((char*)&currentObject->getVertexRaw(v).uv0,sizeof(FloatVector2d));
				currentObject->getVertexRaw(v).uvLightmap=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv1=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv2=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv3=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv4=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv5=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv6=currentObject->getVertexRaw(v).uv0;

				in.read ((char*)&currentObject->getVertexRaw(v).end,sizeof(unsigned int));
			}

			objects.pushBack(currentObject);

		}else if(currentObject->type==1 || currentObject->type==2){	//point light
			//point light
			currentLight=new Light;

			if(currentObject->type==1){
				currentLight->type=LIGHT_POINT;
			}else{
				currentLight->type=LIGHT_SPOT;
			}

			currentLight->name=currentObject->name;
			currentLight->flags=currentObject->flags;

			in.read ((char*)&currentLight->pos,sizeof(currentLight->pos));	
			
			in.read ((char*)&currentLight->rot,sizeof(currentLight->rot));	
			in.read ((char*)&currentLight->color,sizeof(currentLight->color));	//TEMP STORE

			currentLight->falloffStart=currentLight->color.x;
			currentLight->falloffEnd=currentLight->color.y;
			currentLight->coneAngle=currentLight->color.z;

			//color is stored in the first vertex.  Hacky!
			unsigned long vertexcount;
			in.read ((char*)&vertexcount,sizeof(vertexcount));	
			in.read ((char*)&currentLight->color,sizeof(currentLight->color));	//REAL COLOR
			FloatVector3d ts;
			in.read ((char*)&ts,sizeof(ts));	//misc parameters

			currentLight->intensity=ts.x;
			currentLight->umbra=ts.y;

			if(ts.z==1){currentLight->shadow=true;}else{currentLight->shadow=false;}

			FloatVector2d ts2;
			in.read ((char*)&ts2,sizeof(ts2));	//misc parameters
			currentLight->coneSpread=ts2.x;

			lights.pushBack(currentLight);
		}else if(currentObject->type==3){
			NullObject* cNull=new NullObject();
			cNull->name=currentObject->name;
			cNull->flags=currentObject->flags;
			cNull->type=3;
			
			in.read ((char*)&cNull->pos,sizeof(cNull->pos));	
			in.read ((char*)&cNull->rot,sizeof(cNull->rot));
			in.read ((char*)&cNull->scale,sizeof(cNull->scale));

			nulls.pushBack(cNull);

			currentNull=cNull;
		}else if(currentObject->type==4){
			Camera* cCam=new Camera;

			in.read ((char*)&cCam->pos,sizeof(cCam->pos));	
			in.read ((char*)&cCam->rot,sizeof(cCam->rot));

			FloatVector3d scale;
			in.read ((char*)&scale,sizeof(scale));	

			cCam->fov=scale.x;
			cCam->clipNear=scale.y;
			cCam->clipFar=scale.z;
	
			cameras.pushBack(cCam);

		}

		//SORT
		if(currentObject->type==0){

			//all insidebb should be nonstatic (or else that flag is useless)
			if(currentObject->flags & OBJECT_INSIDEBB){
				if(currentObject->flags & OBJECT_STATIC){
					currentObject->flags^=OBJECT_STATIC;
				}
			}

			if(currentObject->flags & OBJECT_COLLIDE){
				currentObject->collide=true;
				colliders.pushBack(currentObject);
			}else{
				currentObject->collide=false;
			}

			if(currentObject->flags & OBJECT_NOFOG){
				currentObject->nofog=true;
			}

			if(currentObject->flags & OBJECT_NOCLIP){
				currentObject->noclip=true;
			}

			if(currentObject->material<0){
				logs().main.write("error, object "+currentObject->name+" doesn't have a material");
				return false;
			}

			bool transhack=false;

			if(currentObject->material<materialCount && currentObject->material>=0){
				//in case this was saved as a dev build, in which case it can't know if a texture is transparent...
				//transparent objects don't get put in either static or nonstatic

				if(materials[currentObject->material].textureDiffuse<textureCount && materials[currentObject->material].textureDiffuse>=0){

					if(textures[materials[currentObject->material].textureDiffuse].bytesPP==4){
						//transparentObjects.pushBack(currentObject);

						transhack=true;
					}
				}
			}

			if(materials[currentObject->material].textureEnvironmentFront!=-1 &&
				materials[currentObject->material].textureEnvironmentBack!=-1 &&
				materials[currentObject->material].textureEnvironmentLeft!=-1 &&
				materials[currentObject->material].textureEnvironmentRight!=-1 &&
				materials[currentObject->material].textureEnvironmentTop!=-1 &&
				materials[currentObject->material].textureEnvironmentBottom!=-1 &&
				!(currentObject->flags & OBJECT_TRANSPARENT) &&
				(materials[currentObject->material].shader & MATERIAL_SHADER_ENVIRONMENT) &&
				!transhack){
				

				//environmentMapObjects.pushBack();
				nonstaticObjects.pushBack(currentObject);
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);

				//int lol=0;
			}else if(!(currentObject->flags & OBJECT_TRANSPARENT) &&
				!(currentObject->flags & OBJECT_STATIC) &&
				(currentObject->noclip) &&
				!transhack){
				

				//noclip objs need to be nonstatic
				nonstaticObjects.pushBack(currentObject);
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);
			else if( (materials[currentObject->material].reflectivity.x>0 || 
				materials[currentObject->material].reflectivity.y>0 ||
				materials[currentObject->material].reflectivity.z>0 ||
				materials[currentObject->material].reflectivity.w>0) &&
				!(currentObject->flags & OBJECT_WATER) &&
				!(currentObject->flags & OBJECT_TRANSPARENT) &&
				!(materials[currentObject->material].shader & MATERIAL_SHADER_ENVIRONMENT) &&
				! transhack){

				reflectiveObjects.pushBack(currentObject);

			}else if((transhack || currentObject->flags & OBJECT_TRANSPARENT) && !(currentObject->flags & OBJECT_WATER)){

				//transparent objects don't get put in either static or nonstatic
				transparentObjects.pushBack(currentObject);
				if(!(currentObject->flags & OBJECT_STATIC)){
					materials[currentObject->material].nonstaticRefs++;
					materials[currentObject->material].nonstaticRefList.pushBack(currentObject);

					//int lol=0;
				}
			}else if(currentObject->flags & OBJECT_STATIC && !(currentObject->flags & OBJECT_WATER)){

				staticObjects.pushBack(currentObject);

			}else if(!(currentObject->flags & OBJECT_STATIC  && !(currentObject->flags & OBJECT_WATER))){

				nonstaticObjects.pushBack(currentObject);
				objectKey[currentObject->name]=currentObject;
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);

				//int lol=0;
			}

			
		}

		if(currentObject->name!=""){
			if(currentObject->type==0){
				if(currentObject->flags & OBJECT_VISIBLE){
					currentObject->visible=true;
				}else{
					currentObject->visible=false;
				}

				if(!(currentObject->flags & OBJECT_STATIC)){
					luaUpload(currentObject);
				}

			}else if( (currentObject->type==1||currentObject->type==2) && currentLight!=NULL){
				if(!(currentObject->flags & OBJECT_STATIC)){
					luaUpload(currentLight);
				}
			}else if(currentObject->type==3 && currentNull!=NULL){
				if(!(currentObject->flags & OBJECT_STATIC)){
					luaUpload(currentNull);
				}
			}

			if(currentObject->flags & OBJECT_WATER){


				water=currentObject;
			}
		}

		

		currentNull=NULL;

	}

	logs().main.write("OBJECTS:"+String(engineTime.getTimerSingle("OBJECTS")));

	engineTime.startTimerSingle("ENDFUNCTS");

	//call create functions
	for(int i=0; i<objects.size(); i++){

		if(objects[i]->action!=""){
			script().run(level->objects[i]->action+"(\"create\")");
		}
	}

	//set camera
	if(cameras.size()>0){


		camera=cameras[0];
		cameraToLua();

	}

	/*upload textures to OGL
	have to do this down here at the end to deal with cube maps
	cube maps all have to be bound to the same target, so we tag them when making materials
	the face info is stored in materials, not textures
	*/

	for(unsigned int i=0; i<textureCount; i++){

		if(!texture.load(&textures[i])){
			return false;
		}

		delete[] textures[i].data;

	}


	//update all
	for(unsigned int i=0; i<objects.size(); i++){
		objects[i]->update();
	}

	for(unsigned int i=0; i<materials.size(); i++){
		materials[i].update();
	}

	conf->forceTextureLevelChanged();
	updateTextureLevel();

	engineTime.skipNextFrame();

	logs().main.write("ENDFUNCTS:"+String(engineTime.getTimerSingle("ENDFUNCTS")));

	logs().main.write("TOTALLOAD:"+String(engineTime.getTimerSingle("TOTALLOAD")));
	return true;
}

#endif
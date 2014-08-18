#include "GlHeader.hpp"

#include "level.hpp"
#include "script.hpp"

#include <iostream> 
#include <fstream>

#include "Log.hpp"
#include "HelperLibMath.hpp"

#include "FileIo.hpp"
#include "Bmp.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "light.h"
#include "Filters.h"
#include "conf.h"
#include "draw.hpp"
#include "os.h"
#include "timeing.hpp"
#include "script.hpp"
#include "endian.hpp"
#include "SceneGraph.hpp"
#include "leaks.h"

#ifdef _WIN32
	#include "NeHeGl.h"
#endif

#ifdef LOAD_PROGRESSIVE

using namespace std;

int totalPackets=0;
int currentPacket=0;
bool currentlyLoading=false;
int skip=4;
int currentSkip=0;

void updateProgressBar(){
#ifdef LOOP_DEBUG
	logs().main.write("updateProgressBar STARTa");
#endif

	if(currentSkip<skip){
		currentSkip++;
		currentPacket++;
		return;
	}else{
		currentSkip=0;
		currentPacket++;
	}

	if(!conf->loadProgressive){return;}

#ifdef LOOP_DEBUG
	logs().main.write("glClearColor "+String((unsigned int)&glClearColor));
#endif

	glClearColor(0,0,0,1);

#ifdef LOOP_DEBUG
	logs().main.write("glClear");
#endif

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float percent=float(currentPacket)/float(totalPackets);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: updateProgressBar script().run");
#endif

	script().run("if loadProgress~=nil and type(loadProgress)=='function' then loadProgress("+String(percent)+") end");

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: updateProgressBar drawPanels");
#endif

	drawPanels();

#ifdef _WIN32
	#ifdef LOOP_DEBUG
		logs().main.write("updateProgressBar::SWAP START "+String((int)window.hDC));
	#endif
		
	SwapBuffers (window.hDC);
	#ifdef LOOP_DEBUG
		logs().main.write("updateProgressBar::SWAP END");
	#endif
#elif LINUX

#else
	glutSwapBuffers();
#endif
	
#ifdef LOOP_DEBUG
	logs().main.write("updateProgressBar ENDa");
#endif
}


bool Level::load(Path infile){	
	
	currentlyLoading=true;
	totalPackets=0;
	currentPacket=0;

	engineTime.startTimerSingle("TOTALLOAD");

	String lol=infile.getExtension();

	if(infile.getExtension()=="obj"){
		return loadObj(infile);
	}

	#ifdef RSA
		String rvf=rsa_verify_file(infile.c_str());

		if(rvf==""){

		}else{
			logs().main.write("Level Verify Error: "+rvf);
			return false;
		}
	#endif

	String infileName=infile.getFilenameNoExtension();

	ifstream in (infile.getAbsolute().c_str(), ios::in|ios::binary);
  	
	if(in==NULL){
		logs().main << "Could not open file: '" << infile.getAbsolute() << "'" << endLine;
		return false;
	}

	file=infile;
	unsigned long startTextures;
	unsigned long startMaterials;
	unsigned long startLightmaps;
	unsigned long startObjects;

	//check header
	in.seekg (0, ios::beg);

	char header[4];
	in.read (header, 4);

	loadingScreen();

	if(header[0]!='L' ||
		header[1]!='M' ||
		header[2]!='L' ||
		(header[3]!='1' && header[3]!='2') ||
		file.getExtension()!="lml"){
		logs().main << String("unknown level format :") << String(header)  << endLine;
		return false;
	}

	int version=1;

	if(header[3]=='2'){
		version=2;
		logs().main.write("VERSION:2");
	}else{
		logs().main.write("VERSION:1");
	}

	//seek over the sig
	in.seekg(256,ios::cur);

	unsigned long localTextureCount,localMaterialCount,localLightmapCount,localObjectCount;

	in.read ((char*)&localTextureCount, 4);
	localTextureCount=endian(localTextureCount);
	textureCount+=localTextureCount;
	unsigned int baseTexture=textures.size();
	for(int i=0; i<localTextureCount; i++){textures.pushBack();}

	logs().main << "textureCount:" << localTextureCount << endLine;
	
	in.read ((char*)&localMaterialCount, 4);
	localMaterialCount=endian(localMaterialCount);
	materialCount+=localMaterialCount;

	unsigned int baseMat=materials.size();

	for(int i=0; i<localMaterialCount; i++){materials.pushBack();}

	logs().main << "materialcount:" << localMaterialCount << endLine;

	in.read ((char*)&localLightmapCount, 4);
	localLightmapCount=endian(localLightmapCount);
	lightmapCount+=localLightmapCount;

	unsigned int baseLightmap=lightmaps.size();

	for(int i=0; i<localLightmapCount; i++){lightmaps.pushBack();}

	logs().main << "lightmapCount:" << localLightmapCount << endLine;

	in.read ((char*)&localObjectCount, 4);
	localObjectCount=endian(localObjectCount);
	objectCount+=localObjectCount;

	logs().main << "objectcount:" << localObjectCount << endLine;

	in.read ((char*)&startTextures, 4);
	in.read ((char*)&startMaterials, 4);
	in.read ((char*)&startLightmaps, 4);
	in.read ((char*)&startObjects, 4);
	startTextures=endian(startTextures);
	startMaterials=endian(startMaterials);
	startLightmaps=endian(startLightmaps);
	startObjects=endian(startObjects);
	
	////////////////////////////

	totalPackets=localTextureCount+localTextureCount+localMaterialCount+localLightmapCount+localObjectCount;
	totalPackets+=1;
	totalPackets+=float(localTextureCount);
	totalPackets+=float(localMaterialCount)*float(0.2);
	totalPackets+=float(localObjectCount)*float(0.2);
#ifdef LOOP_DEBUG
	logs().main.write("CALL updateProgressBar START");
#endif
	updateProgressBar();
#ifdef LOOP_DEBUG
	logs().main.write("CALL updateProgressBar END");
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////start reading textures
	engineTime.startTimerSingle("TEXTURES");
	in.seekg (startTextures, ios::beg);

	unsigned long* texIndices=new unsigned long[localTextureCount];

#ifdef LOOP_DEBUG
	logs().main.write("CALL load::texture loop1 START");
#endif
	for(unsigned int i=0; i<localTextureCount; i++){		
		in.read ((char*)&texIndices[i], 4);
		texIndices[i]=endian(texIndices[i]);
		
	}
#ifdef LOOP_DEBUG
	logs().main.write("CALL load::texture loop1 END");
#endif

#ifdef LOOP_DEBUG
	logs().main.write("CALL load::texture loop2 START");
#endif
	for(unsigned int i=0; i<localTextureCount; i++){
		in.seekg (texIndices[i]);
		
		in.read ((char*)&textures[baseTexture+i].ref,1);	
		
		if(textures[baseTexture+i].ref){
			
			in.read ((char*)&textures[baseTexture+i].flags,sizeof(textures[baseTexture+i].flags));	
			textures[baseTexture+i].file.read(&in);

			textures[baseTexture+i].fileRelative.read(&in);
			
			bool prefound=false;

			//check for duplicates
			for(int t=0; t<baseTexture; t++){
				if(textures[t].file==textures[baseTexture+i].file){

					if(t!=baseTexture+i){
						prefound=true;
						textures[baseTexture+i].x=textures[t].x;
						textures[baseTexture+i].y=textures[t].y;
						textures[baseTexture+i].bytesPP=textures[t].bytesPP;
						textures[baseTexture+i].file=textures[t].file;
						textures[baseTexture+i].fileRelative=textures[t].fileRelative;
						textures[baseTexture+i].name=textures[t].name+String(baseTexture+i);
						textures[baseTexture+i].dataLoaded=textures[t].dataLoaded;

						luaUpload(&textures[baseTexture+i]);
					}
				}
			}

			if(!prefound){
				
				
				Path ftpath;
				ftpath.setAbsolute(textures[baseTexture+i].file);

				if(!os().fileExists(ftpath)){
					String f=textures[baseTexture+i].file;

					logs().renderer.write("texture file doesn't exist:"+f);

					if(textures[baseTexture+i].fileRelative==""){
						logs().renderer.write("Error, no relative path stored, so can't search for file");
						return false;
					}else{
						f=textures[baseTexture+i].fileRelative;

						String tr=infile.getRelative();
						tr.replace("/","\\");
						Array<String> source=tr.explode("\\");
						String wd=os().workingDirectory;
						wd.replace("/","\\");
						Array<String> dest=String(wd).explode("\\");

						if(source[0]!=dest[0]){
							//infile probably just relative
							tr=os().workingDirectory+tr;
							source=tr.explode("\\");
						}

						bool kill=false;

						while(!kill){

							if(source.size()>0 && dest.size()>0){
								if(source[0]!=dest[0]){
									kill=true;
									break;
								}
							}else{
								kill=true;
								break;
							}

							source.erase(0);
							dest.erase(0);
						}

						for(int i=0; i<dest.size()-1; i++){
							f="..\\"+f;
						}

						for(int i=0; i<source.size()-1; i++){
							if(f.substrIndex(0,3)=="..\\" || f.substrIndex(0,3)=="../"){
								f=f.substrIndex(3,f.size());
							}
						}

						f.replace("\\","/");

						ftpath.setRelative(f);
						logs().renderer.write("try to load similar file:"+ftpath.getAbsolute());
					}
				}

				if(!os().fileExists(ftpath)){

#ifdef DEVELOPER
					if(editor.projectList.size()>0){
						if(os().fileExists(editor.projectList[editor.loadedProject]+"/"+ftpath.getRelative())){
							ftpath.setRelative(editor.projectList[editor.loadedProject]+"/"+ftpath.getRelative());
						}else{
							logs().renderer.write("similar file doesn't exist:"+ftpath.getAbsolute());
							return false;
						}
					}else
#endif
					{
						logs().renderer.write("similar file doesn't exist:"+ftpath.getAbsolute());
						return false;
					}
				}
				
#ifdef LOOP_DEBUG
	logs().main.write("CALL load loadImage START");
#endif

				ImageData imagedata=loadImage(ftpath);
#ifdef LOOP_DEBUG
	logs().main.write("CALL load loadImage END");
#endif	
				if(imagedata.loadSuccess){
						textures[baseTexture+i].x=imagedata.width;
						textures[baseTexture+i].y=imagedata.height;
						textures[baseTexture+i].data=imagedata.data;
						textures[baseTexture+i].bytesPP=imagedata.bytesPP;
						textures[baseTexture+i].name=imagedata.name;
						textures[baseTexture+i].file=imagedata.file;
						textures[baseTexture+i].fileRelative=imagedata.fileRelative;
						textures[baseTexture+i].dataLoaded=imagedata.dataLoaded;

						if(!checkSize(textures[baseTexture+i].x) || !checkSize(textures[baseTexture+i].y)){

							String s1="Error, non power of 2 texture '"+textures[baseTexture+i].file + "' x:" + String(textures[baseTexture+i].x ) + " y:" + String(textures[baseTexture+i].y);
							String s2="Make sure image is rgb and not grayscale";
							logs().main.write(s1);
							logs().main.write(s2);

							os().error("Non power of 2 texture error, see log");

							return false;
						}

						if(textures[baseTexture+i].flags & TEXTURE_NORMAL){ 
							

							ImageData t;

							t.width=textures[baseTexture+i].x;
							t.height=textures[baseTexture+i].y;
							t.data=textures[baseTexture+i].data;
							t.bytesPP=textures[baseTexture+i].bytesPP;
							t.bitsPP=textures[baseTexture+i].bytesPP*8;
							t.size=textures[baseTexture+i].bytesPP*textures[baseTexture+i].x*textures[baseTexture+i].y;

							ImageData f=sobel(t);
							textures[baseTexture+i].data=f.data;
						}
				}else{

						String s1="could not load texture file '"+ftpath.getAbsolute()+ "'";
						String s2="Error: "+imagedata.error;

						logs().main.write(s1);
						logs().main.write(s2);
						return false;
				}
			}
		}else{
			in.read ((char*)&textures[baseTexture+i].x,sizeof(textures[baseTexture+i].x));
			textures[baseTexture+i].x=endian(textures[baseTexture+i].x);	
			in.read ((char*)&textures[baseTexture+i].y,sizeof(textures[baseTexture+i].y));	
			textures[baseTexture+i].y=endian(textures[baseTexture+i].y);
			in.read ((char*)&textures[baseTexture+i].bytesPP,sizeof(textures[baseTexture+i].bytesPP));	
			textures[baseTexture+i].bytesPP=endian(textures[baseTexture+i].bytesPP);
			
			if(textures[baseTexture+i].x>2048 || textures[baseTexture+i].y>2048 || textures[baseTexture+i].bytesPP>4 || textures[baseTexture+i].bytesPP<3){

				logs().main.write("bad texture size "+String(textures[baseTexture+i].x)+" "+String(textures[baseTexture+i].y)+" "+String(textures[baseTexture+i].bytesPP));
				return false;
			}
			
			unsigned long tempsize=textures[baseTexture+i].x*textures[baseTexture+i].y*textures[baseTexture+i].bytesPP;

			textures[baseTexture+i].data=new unsigned char[tempsize];
			textures[baseTexture+i].dataLoaded=new bool;
			*textures[baseTexture+i].dataLoaded=true;
			textures[baseTexture+i].filePos=in.tellg();
			textures[baseTexture+i].file=file.getRelative();
			textures[baseTexture+i].fileRelative=file.getRelative();

			leaks.alloc("TextureImage::data");
			leaks.alloc("TextureImage::dataLoaded");

			textures[baseTexture+i].name=infileName+String(baseTexture+i);

			luaUpload(&textures[baseTexture+i]);

			in.read ((char*)textures[baseTexture+i].data,tempsize);	
			
			//textures[baseTexture+i].x/=2;
			//textures[baseTexture+i].y/=2;
		}


		updateProgressBar();

	}
#ifdef LOOP_DEBUG
	logs().main.write("CALL load::texture loop2 END");
#endif

	delete[] texIndices;

	logs().main.write("TEXTURES:"+String(engineTime.getTimerSingle("TEXTURES")));

	/////////////////////////////////////////////////////////////////////////////////////////////////start reading materials
	engineTime.startTimerSingle("MATERIALS");
	in.seekg (startMaterials, ios::beg);

	unsigned long* matIndices=new unsigned long[localMaterialCount];

	
	for(unsigned int i=0; i<localMaterialCount; i++){		
		in.read ((char*)&matIndices[i], 4);
		matIndices[i]=endian(matIndices[i]);
	}

	for(unsigned int i=0; i<localMaterialCount; i++){
		in.seekg (matIndices[i]);
		
		materials[baseMat+i].name.read(&in);

		//prevent duplicate material names
		if(materialKey.find(materials[baseMat+i].name)){
			int cn=0;

			while(materialKey.find(materials[baseMat+i].name+String(cn))){cn++;}

			materials[baseMat+i].name=materials[baseMat+i].name+String(cn);
		}

		in.read ((char*)&materials[baseMat+i].shader,sizeof(materials[baseMat+i].shader));	
		materials[baseMat+i].shader=endian(materials[baseMat+i].shader);
		
		in.seekg (16,ios_base::cur);
		in.read ((char*)&materials[baseMat+i].diffuse.x,sizeof(materials[baseMat+i].diffuse.x));
		in.read ((char*)&materials[baseMat+i].diffuse.y,sizeof(materials[baseMat+i].diffuse.y));
		in.read ((char*)&materials[baseMat+i].diffuse.z,sizeof(materials[baseMat+i].diffuse.z));
		in.read ((char*)&materials[baseMat+i].diffuse.w,sizeof(materials[baseMat+i].diffuse.w));
		materials[baseMat+i].diffuse.x=endian(materials[baseMat+i].diffuse.x);	
		materials[baseMat+i].diffuse.y=endian(materials[baseMat+i].diffuse.y);	
		materials[baseMat+i].diffuse.z=endian(materials[baseMat+i].diffuse.z);	
		materials[baseMat+i].diffuse.w=endian(materials[baseMat+i].diffuse.w);	

		in.seekg (16,ios_base::cur);
		in.read ((char*)&materials[baseMat+i].specular.x,sizeof(materials[baseMat+i].specular.x));
		in.read ((char*)&materials[baseMat+i].specular.y,sizeof(materials[baseMat+i].specular.y));
		in.read ((char*)&materials[baseMat+i].specular.z,sizeof(materials[baseMat+i].specular.z));
		in.read ((char*)&materials[baseMat+i].specular.w,sizeof(materials[baseMat+i].specular.w));
		materials[baseMat+i].specular.x=endian(materials[baseMat+i].specular.x);
		materials[baseMat+i].specular.y=endian(materials[baseMat+i].specular.y);
		materials[baseMat+i].specular.z=endian(materials[baseMat+i].specular.z);
		materials[baseMat+i].specular.w=endian(materials[baseMat+i].specular.w);
		
		in.seekg (16,ios_base::cur);
		in.read ((char*)&materials[baseMat+i].ambient.x,sizeof(materials[baseMat+i].ambient.x));	
		in.read ((char*)&materials[baseMat+i].ambient.y,sizeof(materials[baseMat+i].ambient.y));	
		in.read ((char*)&materials[baseMat+i].ambient.z,sizeof(materials[baseMat+i].ambient.z));	
		in.read ((char*)&materials[baseMat+i].ambient.w,sizeof(materials[baseMat+i].ambient.w));	
		materials[baseMat+i].ambient.x=endian(materials[baseMat+i].ambient.x);
		materials[baseMat+i].ambient.y=endian(materials[baseMat+i].ambient.y);
		materials[baseMat+i].ambient.z=endian(materials[baseMat+i].ambient.z);
		materials[baseMat+i].ambient.w=endian(materials[baseMat+i].ambient.w);

		in.seekg (16,ios_base::cur);
		in.read ((char*)&materials[baseMat+i].reflectivity.x,sizeof(materials[baseMat+i].reflectivity.x));	
		in.read ((char*)&materials[baseMat+i].reflectivity.y,sizeof(materials[baseMat+i].reflectivity.y));	
		in.read ((char*)&materials[baseMat+i].reflectivity.z,sizeof(materials[baseMat+i].reflectivity.z));	
		in.read ((char*)&materials[baseMat+i].reflectivity.w,sizeof(materials[baseMat+i].reflectivity.w));	
		materials[baseMat+i].reflectivity.x=endian(materials[baseMat+i].reflectivity.x);
		materials[baseMat+i].reflectivity.y=endian(materials[baseMat+i].reflectivity.y);
		materials[baseMat+i].reflectivity.z=endian(materials[baseMat+i].reflectivity.z);
		materials[baseMat+i].reflectivity.w=endian(materials[baseMat+i].reflectivity.w);
		
		in.read ((char*)&materials[baseMat+i].tex0pos.x,sizeof(materials[baseMat+i].tex0pos.x));	
		materials[baseMat+i].tex0pos.x=endian(materials[baseMat+i].tex0pos.x);
		in.read ((char*)&materials[baseMat+i].tex0pos.y,sizeof(materials[baseMat+i].tex0pos.y));
		materials[baseMat+i].tex0pos.y=endian(materials[baseMat+i].tex0pos.y);	
		in.read ((char*)&materials[baseMat+i].tex0scale.x,sizeof(materials[baseMat+i].tex0scale.x));
		materials[baseMat+i].tex0scale.x=endian(materials[baseMat+i].tex0scale.x);
		in.read ((char*)&materials[baseMat+i].tex0scale.y,sizeof(materials[baseMat+i].tex0scale.y));
		materials[baseMat+i].tex0scale.y=endian(materials[baseMat+i].tex0scale.y);
		
		in.read ((char*)&materials[baseMat+i].tex1pos.x,sizeof(materials[baseMat+i].tex1pos.x));	
		materials[baseMat+i].tex1pos.x=endian(materials[baseMat+i].tex1pos.x);
		in.read ((char*)&materials[baseMat+i].tex1pos.y,sizeof(materials[baseMat+i].tex1pos.y));
		materials[baseMat+i].tex1pos.y=endian(materials[baseMat+i].tex1pos.y);
		in.read ((char*)&materials[baseMat+i].tex1scale.x,sizeof(materials[baseMat+i].tex1scale.x));
		materials[baseMat+i].tex1scale.x=endian(materials[baseMat+i].tex1scale.x);
		in.read ((char*)&materials[baseMat+i].tex1scale.y,sizeof(materials[baseMat+i].tex1scale.y));
		materials[baseMat+i].tex1scale.y=endian(materials[baseMat+i].tex1scale.y);

		in.read ((char*)&materials[baseMat+i].tex2pos.x,sizeof(materials[baseMat+i].tex2pos.x));	
		materials[baseMat+i].tex2pos.x=endian(materials[baseMat+i].tex2pos.x);
		in.read ((char*)&materials[baseMat+i].tex2pos.y,sizeof(materials[baseMat+i].tex2pos.y));
		materials[baseMat+i].tex2pos.y=endian(materials[baseMat+i].tex2pos.y);
		in.read ((char*)&materials[baseMat+i].tex2scale.x,sizeof(materials[baseMat+i].tex2scale.x));
		materials[baseMat+i].tex2scale.x=endian(materials[baseMat+i].tex2scale.x);
		in.read ((char*)&materials[baseMat+i].tex2scale.y,sizeof(materials[baseMat+i].tex2scale.y));
		materials[baseMat+i].tex2scale.y=endian(materials[baseMat+i].tex2scale.y);

		in.read ((char*)&materials[baseMat+i].tex3pos.x,sizeof(materials[baseMat+i].tex3pos.x));	
		materials[baseMat+i].tex3pos.x=endian(materials[baseMat+i].tex3pos.x);
		in.read ((char*)&materials[baseMat+i].tex3pos.y,sizeof(materials[baseMat+i].tex3pos.y));
		materials[baseMat+i].tex3pos.y=endian(materials[baseMat+i].tex3pos.y);
		in.read ((char*)&materials[baseMat+i].tex3scale.x,sizeof(materials[baseMat+i].tex3scale.x));
		materials[baseMat+i].tex3scale.x=endian(materials[baseMat+i].tex3scale.x);
		in.read ((char*)&materials[baseMat+i].tex3scale.y,sizeof(materials[baseMat+i].tex3scale.y));
		materials[baseMat+i].tex3scale.y=endian(materials[baseMat+i].tex3scale.y);
		
		in.read ((char*)&materials[baseMat+i].tex4pos.x,sizeof(materials[baseMat+i].tex4pos.x));	
		materials[baseMat+i].tex4pos.x=endian(materials[baseMat+i].tex4pos.x);
		in.read ((char*)&materials[baseMat+i].tex4pos.y,sizeof(materials[baseMat+i].tex4pos.y));
		materials[baseMat+i].tex4pos.y=endian(materials[baseMat+i].tex4pos.y);
		in.read ((char*)&materials[baseMat+i].tex4scale.x,sizeof(materials[baseMat+i].tex4scale.x));
		materials[baseMat+i].tex4scale.x=endian(materials[baseMat+i].tex4scale.x);
		in.read ((char*)&materials[baseMat+i].tex4scale.y,sizeof(materials[baseMat+i].tex4scale.y));
		materials[baseMat+i].tex4scale.y=endian(materials[baseMat+i].tex4scale.y);
		
		in.read ((char*)&materials[baseMat+i].tex5pos.x,sizeof(materials[baseMat+i].tex5pos.x));	
		materials[baseMat+i].tex5pos.x=endian(materials[baseMat+i].tex5pos.x);
		in.read ((char*)&materials[baseMat+i].tex5pos.y,sizeof(materials[baseMat+i].tex5pos.y));
		materials[baseMat+i].tex5pos.y=endian(materials[baseMat+i].tex5pos.y);
		in.read ((char*)&materials[baseMat+i].tex5scale.x,sizeof(materials[baseMat+i].tex5scale.x));
		materials[baseMat+i].tex5scale.x=endian(materials[baseMat+i].tex5scale.x);
		in.read ((char*)&materials[baseMat+i].tex5scale.y,sizeof(materials[baseMat+i].tex5scale.y));
		materials[baseMat+i].tex5scale.y=endian(materials[baseMat+i].tex5scale.y);
		
		in.read ((char*)&materials[baseMat+i].tex6pos.x,sizeof(materials[baseMat+i].tex6pos.x));	
		materials[baseMat+i].tex6pos.x=endian(materials[baseMat+i].tex6pos.x);
		in.read ((char*)&materials[baseMat+i].tex6pos.y,sizeof(materials[baseMat+i].tex6pos.y));
		materials[baseMat+i].tex6pos.y=endian(materials[baseMat+i].tex6pos.y);
		in.read ((char*)&materials[baseMat+i].tex6scale.x,sizeof(materials[baseMat+i].tex6scale.x));
		materials[baseMat+i].tex6scale.x=endian(materials[baseMat+i].tex6scale.x);
		in.read ((char*)&materials[baseMat+i].tex6scale.y,sizeof(materials[baseMat+i].tex6scale.y));
		materials[baseMat+i].tex6scale.y=endian(materials[baseMat+i].tex6scale.y);
		
		
		in.read ((char*)&materials[baseMat+i].shiny,sizeof(materials[baseMat+i].shiny));
		materials[baseMat+i].shiny=endian(materials[baseMat+i].shiny);
		
		in.read ((char*)&materials[baseMat+i].textureDiffuse,sizeof(materials[baseMat+i].textureDiffuse));
		materials[baseMat+i].textureDiffuse=endian(materials[baseMat+i].textureDiffuse);
		if(materials[baseMat+i].textureDiffuse!=-1){materials[baseMat+i].textureDiffuse+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureDisplacement,sizeof(materials[baseMat+i].textureDisplacement));
		materials[baseMat+i].textureDisplacement=endian(materials[baseMat+i].textureDisplacement);
		if(materials[baseMat+i].textureDisplacement!=-1){materials[baseMat+i].textureDisplacement+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureSpecular,sizeof(materials[baseMat+i].textureSpecular));
		materials[baseMat+i].textureSpecular=endian(materials[baseMat+i].textureSpecular);
		if(materials[baseMat+i].textureSpecular!=-1){materials[baseMat+i].textureSpecular+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureBump,sizeof(materials[baseMat+i].textureBump));
		materials[baseMat+i].textureBump=endian(materials[baseMat+i].textureBump);
		if(materials[baseMat+i].textureBump!=-1){materials[baseMat+i].textureBump+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureEnvironmentFront,sizeof(materials[baseMat+i].textureEnvironmentFront));
		materials[baseMat+i].textureEnvironmentFront=endian(materials[baseMat+i].textureEnvironmentFront);
		if(materials[baseMat+i].textureEnvironmentFront!=-1){materials[baseMat+i].textureEnvironmentFront+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureEnvironmentBack,sizeof(materials[baseMat+i].textureEnvironmentBack));
		materials[baseMat+i].textureEnvironmentBack=endian(materials[baseMat+i].textureEnvironmentBack);
		if(materials[baseMat+i].textureEnvironmentBack!=-1){materials[baseMat+i].textureEnvironmentBack+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureEnvironmentLeft,sizeof(materials[baseMat+i].textureEnvironmentLeft));
		materials[baseMat+i].textureEnvironmentLeft=endian(materials[baseMat+i].textureEnvironmentLeft);
		if(materials[baseMat+i].textureEnvironmentLeft!=-1){materials[baseMat+i].textureEnvironmentLeft+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureEnvironmentRight,sizeof(materials[baseMat+i].textureEnvironmentRight));
		materials[baseMat+i].textureEnvironmentRight=endian(materials[baseMat+i].textureEnvironmentRight);
		if(materials[baseMat+i].textureEnvironmentRight!=-1){materials[baseMat+i].textureEnvironmentRight+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureEnvironmentTop,sizeof(materials[baseMat+i].textureEnvironmentTop));
		materials[baseMat+i].textureEnvironmentTop=endian(materials[baseMat+i].textureEnvironmentTop);
		if(materials[baseMat+i].textureEnvironmentTop!=-1){materials[baseMat+i].textureEnvironmentTop+=baseTexture;}

		in.read ((char*)&materials[baseMat+i].textureEnvironmentBottom,sizeof(materials[baseMat+i].textureEnvironmentBottom));
		materials[baseMat+i].textureEnvironmentBottom=endian(materials[baseMat+i].textureEnvironmentBottom);
		if(materials[baseMat+i].textureEnvironmentBottom!=-1){materials[baseMat+i].textureEnvironmentBottom+=baseTexture;}


		if(materials[baseMat+i].textureDiffuse>(int)textureCount ||
			materials[baseMat+i].textureDisplacement>(int)textureCount ||
			materials[baseMat+i].textureSpecular>(int)textureCount ||
			materials[baseMat+i].textureBump>(int)textureCount ||
			materials[baseMat+i].textureEnvironmentFront>(int)textureCount ||
			materials[baseMat+i].textureEnvironmentBack>(int)textureCount ||
			materials[baseMat+i].textureEnvironmentLeft>(int)textureCount ||
			materials[baseMat+i].textureEnvironmentRight>(int)textureCount ||
			materials[baseMat+i].textureEnvironmentTop>(int)textureCount ||
			materials[baseMat+i].textureEnvironmentBottom>(int)textureCount){


				logs().main.write("Material ["+String(i)+" - "+materials[baseMat+i].name+"]points to bad texture.  There must be an error in the file.  Try rebuilding the level (maybe it's in an old format or something)");
				logs().main.write(String(materials[baseMat+i].textureDiffuse)+" "+String(materials[baseMat+i].textureDisplacement)+" "+String(materials[baseMat+i].textureSpecular)+" "+String(materials[baseMat+i].textureBump)+" "+String(materials[baseMat+i].textureEnvironmentFront)+" "+String(materials[baseMat+i].textureEnvironmentBack)+" ");
				logs().main.write(materials[baseMat+i].textureEnvironmentLeft);
				logs().main.write(materials[baseMat+i].textureEnvironmentRight);
				logs().main.write(materials[baseMat+i].textureEnvironmentTop);
				logs().main.write(materials[baseMat+i].textureEnvironmentBottom);
				
				logs().main.write("TEXCNT:"+String((int)textureCount));
			return false;
		}

		//set up the cube maps (if there are any)

		if(materials[baseMat+i].textureEnvironmentFront!=-1 &&
			materials[baseMat+i].textureEnvironmentBack!=-1 &&
			materials[baseMat+i].textureEnvironmentLeft!=-1 &&
			materials[baseMat+i].textureEnvironmentRight!=-1 &&
			materials[baseMat+i].textureEnvironmentTop!=-1 &&
			materials[baseMat+i].textureEnvironmentBottom!=-1 &&
			!(materials[baseMat+i].shader & MATERIAL_SHADER_MIXER)){

			GLuint gid=0;
			glGenTextures(1, &gid);

			textures[materials[baseMat+i].textureEnvironmentFront].flags|=TEXTURE_CUBE_FRONT;
			textures[materials[baseMat+i].textureEnvironmentBack].flags|=TEXTURE_CUBE_BACK;
			textures[materials[baseMat+i].textureEnvironmentLeft].flags|=TEXTURE_CUBE_LEFT;
			textures[materials[baseMat+i].textureEnvironmentRight].flags|=TEXTURE_CUBE_RIGHT;
			textures[materials[baseMat+i].textureEnvironmentTop].flags|=TEXTURE_CUBE_TOP;
			textures[materials[baseMat+i].textureEnvironmentBottom].flags|=TEXTURE_CUBE_BOTTOM;

			textures[materials[baseMat+i].textureEnvironmentFront].glName(gid);
			textures[materials[baseMat+i].textureEnvironmentBack].glName(gid);
			textures[materials[baseMat+i].textureEnvironmentLeft].glName(gid);
			textures[materials[baseMat+i].textureEnvironmentRight].glName(gid);
			textures[materials[baseMat+i].textureEnvironmentTop].glName(gid);
			textures[materials[baseMat+i].textureEnvironmentBottom].glName(gid);
		}

		materialKey[materials[baseMat+i].name]=&materials[baseMat+i];
		materialIndexKey[materials[baseMat+i].name]=baseMat+i;
		updateProgressBar();
	}

	delete[] matIndices;

	logs().main.write("MATERIALS:"+String(engineTime.getTimerSingle("MATERIALS")));

	/////////////////////////////////////////////////////////////////////////////////////////////////start reading lightmaps
	engineTime.startTimerSingle("LIGHTMAPS");
	in.seekg (startLightmaps, ios::beg);

	unsigned long* lightmapIndices=new unsigned long[lightmapCount];

	
	for(unsigned int i=0; i<localLightmapCount; i++){
		in.read ((char*)&lightmapIndices[i], 4);
		lightmapIndices[i]=endian(lightmapIndices[i]);
	}

	for(unsigned int i=0; i<localLightmapCount; i++){
		in.seekg (lightmapIndices[i]);
		
		in.read ((char*)&lightmaps[baseLightmap+i].ref,1);

		ImageData imagedata;
		bool idload=false;

		if(lightmaps[baseLightmap+i].ref){
			in.read ((char*)&lightmaps[baseLightmap+i].flags,sizeof(lightmaps[baseLightmap+i].flags));
			lightmaps[baseLightmap+i].file.read(&in);
			lightmaps[baseLightmap+i].fileRelative.read(&in);
			

			Path flpath;
			flpath.setAbsolute(lightmaps[baseLightmap+i].file);

			if(!os().fileExists(flpath)){
				String f=lightmaps[baseLightmap+i].file;

				logs().renderer.write("lightmap file doesn't exist:"+f);

				f=lightmaps[baseLightmap+i].fileRelative;

				String tr=infile.getRelative();	//the position of the lml file

				if(f.contains("\\")==false && f.contains("/")==false){
					//it's in the same directory as the lml

					tr.replace("/","\\");
					Array<String> source=tr.explode("\\");

					String final;

					for(int i=0; i<source.size()-1; i++){
						final=source[i]+"/";
					}

					f=final+f;

					logs().renderer.write("try to load similar file:"+f);
				}else{

					f=lightmaps[baseLightmap+i].fileRelative;

					String tr=infile.getRelative();
					tr.replace("/","\\");
					Array<String> source=tr.explode("\\");
					String wd=os().workingDirectory;
					wd.replace("/","\\");
					Array<String> dest=String(wd).explode("\\");

					if(source[0]!=dest[0]){
						//infile probably just relative
						tr=os().workingDirectory+tr;
						source=tr.explode("\\");
					}

					bool kill=false;

					while(!kill){

						if(source.size()>0 && dest.size()>0){
							if(source[0]!=dest[0]){
								kill=true;
								break;
							}
						}else{
							kill=true;
							break;
						}

						source.erase(0);
						dest.erase(0);
					}

					for(int i=0; i<dest.size()-1; i++){
						f="..\\"+f;
					}

					for(int i=0; i<source.size()-1; i++){
						if(f.substrIndex(0,3)=="..\\" || f.substrIndex(0,3)=="../"){
							f=f.substrIndex(3,f.size());
						}
					}

					f.replace("\\","/");

					flpath.setRelative(f);
					logs().renderer.write("try to load similar file:"+flpath.getAbsolute());
				}

				
			}

			if(!os().fileExists(flpath)){
#ifdef DEVELOPER
				if(editor.projectList.size()>0){
					if(os().fileExists(editor.projectList[editor.loadedProject]+"/"+flpath.getRelative())){
						flpath.setRelative(editor.projectList[editor.loadedProject]+"/"+flpath.getRelative());
					}else{
						logs().renderer.write("similar file doesn't exist:"+flpath.getAbsolute());
						return false;
					}
				}else
#endif
				{
					logs().renderer.write("similar file doesn't exist:"+flpath.getAbsolute());
					return false;
				}
			}

			imagedata=loadImage(flpath);
			idload=true;

			if(imagedata.loadSuccess){
					lightmaps[baseLightmap+i].x=imagedata.width;
					lightmaps[baseLightmap+i].y=imagedata.height;
					lightmaps[baseLightmap+i].data=imagedata.data;
					lightmaps[baseLightmap+i].bytesPP=imagedata.bytesPP;
					lightmaps[baseLightmap+i].name=imagedata.name;
					lightmaps[baseLightmap+i].file=imagedata.file;
					lightmaps[baseLightmap+i].fileRelative=imagedata.fileRelative;
					lightmaps[baseLightmap+i].dataLoaded=imagedata.dataLoaded;


					if(!checkSize(lightmaps[baseLightmap+i].x) || !checkSize(lightmaps[baseLightmap+i].y)){

						String s1="Error, non power of 2 lightmap '"+lightmaps[baseLightmap+i].file + "' x:" + String(lightmaps[baseLightmap+i].x ) + " y:" + String(lightmaps[baseLightmap+i].y);
						String s2="Make sure image is rgb and not grayscale";
						logs().main.write(s1);
						logs().main.write(s2);
						return false;
					}

			}else{

					String s1="could not load lightmap file '"+lightmaps[baseLightmap+i].file+ "'";
					String s2="Error: "+imagedata.error;

					logs().main.write(s1);
						logs().main.write(s2);
					return false;
			}

			if(lightmaps[baseLightmap+i].x>2048 || lightmaps[baseLightmap+i].y>2048 || lightmaps[baseLightmap+i].bytesPP>4 || lightmaps[baseLightmap+i].bytesPP<3){

				logs().main.write("bad lightmap size");
				return false;
			}

		}else{
			in.read ((char*)&lightmaps[baseLightmap+i].x,sizeof(lightmaps[baseLightmap+i].x));	
			lightmaps[baseLightmap+i].x=endian(lightmaps[baseLightmap+i].x);
			in.read ((char*)&lightmaps[baseLightmap+i].y,sizeof(lightmaps[baseLightmap+i].y));	
			lightmaps[baseLightmap+i].y=endian(lightmaps[baseLightmap+i].y);
			in.read ((char*)&lightmaps[baseLightmap+i].bytesPP,sizeof(lightmaps[baseLightmap+i].bytesPP));
			lightmaps[baseLightmap+i].bytesPP=endian(lightmaps[baseLightmap+i].bytesPP);
						
			if(lightmaps[baseLightmap+i].x>2048 || lightmaps[baseLightmap+i].y>2048 || lightmaps[baseLightmap+i].bytesPP>4 || lightmaps[baseLightmap+i].bytesPP<3){

				logs().main.write("bad lightmap size "+String(lightmaps[baseLightmap+i].x)+" "+String(lightmaps[baseLightmap+i].y)+" "+String(lightmaps[baseLightmap+i].bytesPP));
				return false;
			}

			unsigned long tempsize=lightmaps[baseLightmap+i].x*lightmaps[baseLightmap+i].y*lightmaps[baseLightmap+i].bytesPP;

			lightmaps[baseLightmap+i].data=new unsigned char[tempsize];

			lightmaps[baseLightmap+i].name=infileName+String("LM")+String(baseLightmap+i);

			lightmaps[baseLightmap+i].dataLoaded=new bool;
			*lightmaps[baseLightmap+i].dataLoaded=true;

			lightmaps[baseLightmap+i].filePos=in.tellg();
			lightmaps[baseLightmap+i].file=file.getRelative();

			luaUpload(&lightmaps[baseLightmap+i]);

			in.read ((char*)lightmaps[baseLightmap+i].data,tempsize);	

			//lightmaps[baseLightmap+i].x/=2;
			//lightmaps[baseLightmap+i].y/=2;
		}

		if(!texture.load(&lightmaps[baseLightmap+i])){
			logs().main.write("error loading lightmap texture "+(String)i);
			return false;
		}

		if(idload){
			*imagedata.dataLoaded=false;
		}

		delete[] lightmaps[baseLightmap+i].data;
		*lightmaps[baseLightmap+i].dataLoaded=false;

		updateProgressBar();
	}

	delete[] lightmapIndices;

	logs().main.write("LIGHTMAPS:"+String(engineTime.getTimerSingle("LIGHTMAPS")));

	///////////////////////////////////////////////////////////////////////////////////////////////start reading objects
	
	engineTime.startTimerSingle("OBJECTS");
	in.seekg (startObjects, ios::beg);

	Map<String,String> renameRemap;

	//read object positions
	unsigned long* indices=new unsigned long[localObjectCount];

	for(int i=0; i<localObjectCount; i++){
		
		in.read ((char*)&indices[i], 4);
		indices[i]=endian(indices[i]);
	}

	//get objects
	
	Object* currentObject=NULL;
	NullObject* currentNull=NULL;
	Light* currentLight=NULL;
	Curve* currentCurve=NULL;
	Camera* cCam=NULL;

	SceneGraphNode* currentNode=&sceneGraph.root;
	unsigned int currentLevel=0;

	for(unsigned int i=0; i<localObjectCount; i++){
		currentObject=new Object;

		in.seekg (indices[i]);
		
		in.read ((char*)&currentObject->type,sizeof(currentObject->type));	//type
		currentObject->type=endian(currentObject->type);

		in.read ((char*)&currentObject->flags,sizeof(currentObject->flags));	//flags
		currentObject->flags=endian(currentObject->flags);
		
		in.read ((char*)&currentObject->material,sizeof(currentObject->material));	//flags
		currentObject->material=endian(currentObject->material);
		currentObject->material+=baseMat;

		in.read ((char*)&currentObject->lightmap,sizeof(currentObject->lightmap));	//flags
		currentObject->lightmap=endian(currentObject->lightmap);
		
		if(currentObject->lightmap!=-1){
			currentObject->lightmap+=baseLightmap;
		}

		if(version==2){
			in.read ((char*)&currentObject->sceneGraphLevel,sizeof(currentObject->sceneGraphLevel));	//flags
			currentObject->sceneGraphLevel=endian(currentObject->sceneGraphLevel);
		
		}
		

		currentObject->name.read(&in);

		//prevent duplicate names
		if(objectExists(currentObject->name)){
			String old=currentObject->name;

			int cn=0;

			while(objectExists(currentObject->name+String(cn))){cn++;}

			currentObject->name=currentObject->name+String(cn);
			renameRemap[old]=currentObject->name;
		}

		
		currentObject->action.read(&in);

		if(version==2){
			currentObject->constraintPosName.read(&in);

			if(renameRemap.find(currentObject->constraintPosName)){currentObject->constraintPosName=renameRemap[currentObject->constraintPosName];}

			in.read ((char*)&currentObject->constraintPosType,4);
			currentObject->constraintPosType=endian(currentObject->constraintPosType);
			
			currentObject->constraintRotName.read(&in);

			if(renameRemap.find(currentObject->constraintRotName)){currentObject->constraintRotName=renameRemap[currentObject->constraintRotName];}

			in.read ((char*)&currentObject->constraintRotType,4);
			currentObject->constraintRotType=endian(currentObject->constraintRotType);
			
			currentObject->constraintScaleName.read(&in);

			if(renameRemap.find(currentObject->constraintScaleName)){currentObject->constraintScaleName=renameRemap[currentObject->constraintScaleName];}

			in.read ((char*)&currentObject->constraintScaleType,4);
			currentObject->constraintScaleType=endian(currentObject->constraintScaleType);
		}

		if(currentObject->type==0){
			in.read ((char*)&currentObject->pos.x,sizeof(currentObject->pos.x));	
			in.read ((char*)&currentObject->pos.y,sizeof(currentObject->pos.y));	
			in.read ((char*)&currentObject->pos.z,sizeof(currentObject->pos.z));	
			currentObject->pos.x=endian(currentObject->pos.x);
			currentObject->pos.y=endian(currentObject->pos.y);
			currentObject->pos.z=endian(currentObject->pos.z);
			
			in.read ((char*)&currentObject->rot.x,sizeof(currentObject->rot.x));	
			in.read ((char*)&currentObject->rot.y,sizeof(currentObject->rot.y));	
			in.read ((char*)&currentObject->rot.z,sizeof(currentObject->rot.z));	
			currentObject->rot.x=endian(currentObject->rot.x);
			currentObject->rot.y=endian(currentObject->rot.y);
			currentObject->rot.z=endian(currentObject->rot.z);
			
			in.read ((char*)&currentObject->scale.x,sizeof(currentObject->scale.x));	
			in.read ((char*)&currentObject->scale.y,sizeof(currentObject->scale.y));	
			in.read ((char*)&currentObject->scale.z,sizeof(currentObject->scale.z));	
			currentObject->scale.x=endian(currentObject->scale.x);
			currentObject->scale.y=endian(currentObject->scale.y);
			currentObject->scale.z=endian(currentObject->scale.z);

			int vc;
			in.read ((char*)&vc,sizeof(vc));
			vc=endian(vc);
			
			currentObject->setVertexCount(vc);

			//read vertices
			if(currentObject->getVertexCount()>0){
				currentObject->reserveVertices(currentObject->getVertexCount());
				for(int i=0; i<currentObject->getVertexCount(); i++){
					currentObject->addVertex();
				}
			}


			for(unsigned int v=0; v<currentObject->getVertexCount(); v++){
				in.read ((char*)&currentObject->getVertexRaw(v).pos.x,4);
				in.read ((char*)&currentObject->getVertexRaw(v).pos.y,4);
				in.read ((char*)&currentObject->getVertexRaw(v).pos.z,4);
				currentObject->getVertexRaw(v).pos.x=endian(currentObject->getVertexRaw(v).pos.x);
				currentObject->getVertexRaw(v).pos.y=endian(currentObject->getVertexRaw(v).pos.y);
				currentObject->getVertexRaw(v).pos.z=endian(currentObject->getVertexRaw(v).pos.z);

				in.read ((char*)&currentObject->getVertexRaw(v).normal.x,4);
				in.read ((char*)&currentObject->getVertexRaw(v).normal.y,4);
				in.read ((char*)&currentObject->getVertexRaw(v).normal.z,4);
				currentObject->getVertexRaw(v).normal.x=endian(currentObject->getVertexRaw(v).normal.x);
				currentObject->getVertexRaw(v).normal.y=endian(currentObject->getVertexRaw(v).normal.y);
				currentObject->getVertexRaw(v).normal.z=endian(currentObject->getVertexRaw(v).normal.z);
				
				in.read ((char*)&currentObject->getVertexRaw(v).binormal.x,4);
				in.read ((char*)&currentObject->getVertexRaw(v).binormal.y,4);
				in.read ((char*)&currentObject->getVertexRaw(v).binormal.z,4);
				currentObject->getVertexRaw(v).binormal.x=endian(currentObject->getVertexRaw(v).binormal.x);
				currentObject->getVertexRaw(v).binormal.y=endian(currentObject->getVertexRaw(v).binormal.y);
				currentObject->getVertexRaw(v).binormal.z=endian(currentObject->getVertexRaw(v).binormal.z);
				
				in.read ((char*)&currentObject->getVertexRaw(v).tangent.x,4);
				in.read ((char*)&currentObject->getVertexRaw(v).tangent.y,4);
				in.read ((char*)&currentObject->getVertexRaw(v).tangent.z,4);
				currentObject->getVertexRaw(v).tangent.x=endian(currentObject->getVertexRaw(v).tangent.x);
				currentObject->getVertexRaw(v).tangent.y=endian(currentObject->getVertexRaw(v).tangent.y);
				currentObject->getVertexRaw(v).tangent.z=endian(currentObject->getVertexRaw(v).tangent.z);
				
				in.read ((char*)&currentObject->getVertexRaw(v).uv0.x,4);
				in.read ((char*)&currentObject->getVertexRaw(v).uv0.y,4);
				currentObject->getVertexRaw(v).uv0.x=endian(currentObject->getVertexRaw(v).uv0.x);
				currentObject->getVertexRaw(v).uv0.y=endian(currentObject->getVertexRaw(v).uv0.y);

				currentObject->getVertexRaw(v).uvLightmap=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv1=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv2=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv3=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv4=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv5=currentObject->getVertexRaw(v).uv0;
				currentObject->getVertexRaw(v).uv6=currentObject->getVertexRaw(v).uv0;

				in.read ((char*)&currentObject->getVertexRaw(v).end,4);
				currentObject->getVertexRaw(v).end=endian(currentObject->getVertexRaw(v).end);

				if(version==2){
					in.read ((char*)&currentObject->getVertexRaw(v).envelopeIndex,4);
					currentObject->getVertexRaw(v).envelopeIndex=endian(currentObject->getVertexRaw(v).envelopeIndex);
				}
			}

			
			objectKey[currentObject->name]=currentObject;
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

			in.read ((char*)&currentLight->pos.x,sizeof(currentLight->pos.x));	
			in.read ((char*)&currentLight->pos.y,sizeof(currentLight->pos.y));	
			in.read ((char*)&currentLight->pos.z,sizeof(currentLight->pos.z));	
			currentLight->pos.x=endian(currentLight->pos.x);
			currentLight->pos.y=endian(currentLight->pos.y);
			currentLight->pos.z=endian(currentLight->pos.z);
			
			in.read ((char*)&currentLight->rot.x,sizeof(currentLight->rot.x));	
			in.read ((char*)&currentLight->rot.y,sizeof(currentLight->rot.y));
			in.read ((char*)&currentLight->rot.z,sizeof(currentLight->rot.z));
			currentLight->rot.x=endian(currentLight->rot.x);
			currentLight->rot.y=endian(currentLight->rot.y);
			currentLight->rot.z=endian(currentLight->rot.z);
			
			in.read ((char*)&currentLight->color.x,sizeof(currentLight->color.x));	//TEMP STORE
			in.read ((char*)&currentLight->color.y,sizeof(currentLight->color.y));
			in.read ((char*)&currentLight->color.z,sizeof(currentLight->color.z));
			currentLight->color.x=endian(currentLight->color.x);
			currentLight->color.y=endian(currentLight->color.y);
			currentLight->color.z=endian(currentLight->color.z);
			
			currentLight->falloffStart=currentLight->color.x;
			currentLight->falloffEnd=currentLight->color.y;
			currentLight->coneAngle=currentLight->color.z;

			//color is stored in the first vertex.  Hacky!
			unsigned long vertexcount;
			in.read ((char*)&vertexcount,sizeof(vertexcount));	
			vertexcount=endian(vertexcount);
			in.read ((char*)&currentLight->color.x,sizeof(currentLight->color.x));	//REAL COLOR
			in.read ((char*)&currentLight->color.y,sizeof(currentLight->color.y));	//REAL COLOR
			in.read ((char*)&currentLight->color.z,sizeof(currentLight->color.z));	//REAL COLOR
			currentLight->color.x=endian(currentLight->color.x);
			currentLight->color.y=endian(currentLight->color.y);
			currentLight->color.z=endian(currentLight->color.z);
			
			FloatVector3d ts;
			in.read ((char*)&ts.x,sizeof(ts.x));	//misc parameters
			in.read ((char*)&ts.y,sizeof(ts.y));
			in.read ((char*)&ts.z,sizeof(ts.z));
			ts.x=endian(ts.x);
			ts.y=endian(ts.y);
			ts.z=endian(ts.z);
			
			currentLight->intensity=ts.x;
			currentLight->umbra=ts.y;

			if(ts.z==1){currentLight->shadow=true;}else{currentLight->shadow=false;}

			FloatVector2d ts2;
			in.read ((char*)&ts2.x,sizeof(ts2.x));	//misc parameters
			in.read ((char*)&ts2.y,sizeof(ts2.y));
			ts2.x=endian(ts2.x);
			ts2.y=endian(ts2.y);
			currentLight->coneSpread=ts2.x;

			lights.pushBack(currentLight);
			lightKey[currentLight->name]=currentLight;
		}else if(currentObject->type==3){
			//null

			NullObject* cNull=new NullObject();
			cNull->name=currentObject->name;
			cNull->flags=currentObject->flags;
			cNull->type=3;
			
			in.read ((char*)&cNull->pos.x,sizeof(cNull->pos.x));
			in.read ((char*)&cNull->pos.y,sizeof(cNull->pos.y));
			in.read ((char*)&cNull->pos.z,sizeof(cNull->pos.z));
			cNull->pos.x=endian(cNull->pos.x);
			cNull->pos.y=endian(cNull->pos.y);
			cNull->pos.z=endian(cNull->pos.z);
			
			in.read ((char*)&cNull->rot.x,sizeof(cNull->rot.x));
			in.read ((char*)&cNull->rot.y,sizeof(cNull->rot.y));
			in.read ((char*)&cNull->rot.z,sizeof(cNull->rot.z));
			cNull->rot.x=endian(cNull->rot.x);
			cNull->rot.y=endian(cNull->rot.y);
			cNull->rot.z=endian(cNull->rot.z);
			
			in.read ((char*)&cNull->scale.x,sizeof(cNull->scale.x));
			in.read ((char*)&cNull->scale.y,sizeof(cNull->scale.y));
			in.read ((char*)&cNull->scale.z,sizeof(cNull->scale.z));
			cNull->scale.x=endian(cNull->scale.x);
			cNull->scale.y=endian(cNull->scale.y);
			cNull->scale.z=endian(cNull->scale.z);
			
			unsigned long vertexcount;
			in.read ((char*)&vertexcount,sizeof(vertexcount));	
			vertexcount=endian(vertexcount);

			cNull->constraintPosName=currentObject->constraintPosName;
			cNull->constraintPosType=currentObject->constraintPosType;
			cNull->constraintRotName=currentObject->constraintRotName;
			cNull->constraintRotType=currentObject->constraintRotType;
			cNull->constraintScaleName=currentObject->constraintScaleName;
			cNull->constraintScaleType=currentObject->constraintScaleType;

			nulls.pushBack(cNull);
			nullKey[currentObject->name]=cNull;

			currentNull=cNull;
		}else if(currentObject->type==4){
			cCam=new Camera;

			FloatVector3d pos,rot,scale;
			unsigned int verCount;

			in.read ((char*)&pos.x,sizeof(pos.x));
			pos.x=endian(pos.x);	
			in.read ((char*)&pos.y,sizeof(pos.y));	
			pos.y=endian(pos.y);
			in.read ((char*)&pos.z,sizeof(pos.z));	
			pos.z=endian(pos.z);
			
			in.read ((char*)&rot.x,4);	
			rot.x=endian(rot.x);
			in.read ((char*)&rot.y,4);	
			rot.y=endian(rot.y);
			in.read ((char*)&rot.z,4);	
			rot.z=endian(rot.z);
			
			in.read ((char*)&scale.x,4);	
			scale.x=endian(scale.x);
			in.read ((char*)&scale.y,4);
			scale.y=endian(scale.y);	
			in.read ((char*)&scale.z,4);	
			scale.z=endian(scale.z);
			
			in.read ((char*)&verCount,4);
			verCount=endian(verCount);
			
			cCam->pos=pos;
			cCam->rot=rot;
			cCam->scale=scale;

		}else if(currentObject->type==6){

			FloatVector3d pos,rot,scale;
			unsigned int verCount;

			in.read ((char*)&pos.x,sizeof(pos.x));
			pos.x=endian(pos.x);	
			in.read ((char*)&pos.y,sizeof(pos.y));	
			pos.y=endian(pos.y);
			in.read ((char*)&pos.z,sizeof(pos.z));	
			pos.z=endian(pos.z);
			
			in.read ((char*)&rot.x,4);	
			rot.x=endian(rot.x);
			in.read ((char*)&rot.y,4);	
			rot.y=endian(rot.y);
			in.read ((char*)&rot.z,4);	
			rot.z=endian(rot.z);
			
			in.read ((char*)&scale.x,4);	
			scale.x=endian(scale.x);
			in.read ((char*)&scale.y,4);
			scale.y=endian(scale.y);	
			in.read ((char*)&scale.z,4);	
			scale.z=endian(scale.z);
			
			in.read ((char*)&verCount,4);
			verCount=endian(verCount);
			
		}else if(currentObject->type==9){

			FloatVector3d pos,rot,scale;
			unsigned int verCount;

			Curve* cCurve=new Curve;

			cCurve->name=currentObject->name;

			in.read ((char*)&pos.x,sizeof(pos.x));
			cCurve->pos.x=endian(pos.x);	
			in.read ((char*)&pos.y,sizeof(pos.y));	
			cCurve->pos.y=endian(pos.y);
			in.read ((char*)&pos.z,sizeof(pos.z));	
			cCurve->pos.z=endian(pos.z);
			
			in.read ((char*)&rot.x,4);	
			cCurve->rot.x=endian(rot.x);
			in.read ((char*)&rot.y,4);	
			cCurve->rot.y=endian(rot.y);
			in.read ((char*)&rot.z,4);	
			cCurve->rot.z=endian(rot.z);
			
			in.read ((char*)&scale.x,4);	
			cCurve->scale.x=endian(scale.x);
			in.read ((char*)&scale.y,4);
			cCurve->scale.y=endian(scale.y);	
			in.read ((char*)&scale.z,4);	
			cCurve->scale.z=endian(scale.z);
			
			in.read ((char*)&verCount,4);
			verCount=endian(verCount);

			unsigned int degree=0;
			unsigned int closed=0;

			in.read((char*)&degree,4);
			degree=endian(degree);
			in.read((char*)&closed,4);
			closed=endian(closed);

			cCurve->degree=degree;
			if(closed==1){cCurve->closed=true;}else{cCurve->closed=false;}

			for(int i=0; i<verCount; i++){
				FloatVector3d t;
				in.read((char*)&t.x,4);
				t.x=endian(t.x);
				in.read((char*)&t.y,4);
				t.y=endian(t.y);
				in.read((char*)&t.z,4);
				t.z=endian(t.z);

				cCurve->controlPoints.pushBack(t);
			}

			currentCurve=cCurve;
			curves.pushBack(currentCurve);
			curveKey[currentCurve->name]=currentCurve;
		}

		if(version==2){
			//read animations

			unsigned int anicount=0;
			in.read ((char*)&anicount,4);
			anicount=endian(anicount);
      assert(anicount==0);
		}

		if(version==2){
			//read envelopes

			unsigned int envcount=0;
			in.read ((char*)&envcount,4);
			envcount=endian(envcount);
      assert(envcount==0);
    }
		
		
		if(version==2){

			ObjectAddress oa;
			
			if(currentObject->type==0){
				oa.type=OBJECT;
				oa.object=currentObject;
			}else if(currentObject->type==1 || currentObject->type==2){
				oa.type=LIGHT;
				oa.light=currentLight;
			}else if(currentObject->type==3){
				oa.type=NULL3D;
				oa.null=currentNull;
			}else if(currentObject->type==4){
				oa.type=CAMERA;
				oa.camera=cCam;
			}else if(currentObject->type==9){
				oa.type=CURVE;
				oa.curve=currentCurve;
			}else{

			}

			if(currentObject->sceneGraphLevel==(currentLevel+1)){

				SceneGraphNode newnode;

				newnode.object=oa;
				newnode.parent=currentNode;

				currentLevel++;
				currentNode->children.pushBack(newnode);
				currentNode=&currentNode->children[currentNode->children.size()-1];
				sceneGraph.nameMap[currentObject->name]=currentNode;
			}else if(currentObject->sceneGraphLevel==currentLevel){
				SceneGraphNode newnode;

				newnode.object=oa;
				newnode.parent=currentNode->parent;
				
				currentNode->parent->children.pushBack(newnode);
				currentNode=&currentNode->parent->children[currentNode->parent->children.size()-1];
				sceneGraph.nameMap[currentObject->name]=currentNode;
			}else{

				for(; currentLevel > currentObject->sceneGraphLevel; currentLevel--){
					currentNode=currentNode->parent;
				}

				SceneGraphNode newnode;

				newnode.object=oa;
				newnode.parent=currentNode->parent;

				currentNode->parent->children.pushBack(newnode);

				if(currentNode->parent==NULL){
					console().write("SCENE GRAPH ERROR");
					return false;
				}

				currentNode=&currentNode->parent->children[currentNode->parent->children.size()-1];
				sceneGraph.nameMap[currentObject->name]=currentNode;
			}

			if(currentObject->type==0){
				currentObject->sceneGraphNode=currentNode;
			}else if(currentObject->type==1){
				currentLight->sceneGraphNode=currentNode;
			}else if(currentObject->type==2){
				currentLight->sceneGraphNode=currentNode;
			}else if(currentObject->type==3){
				currentNull->sceneGraphNode=currentNode;
			}else if(currentObject->type==4){
				cCam->sceneGraphNode=currentNode;
			}else if(currentObject->type==9){
				currentCurve->sceneGraphNode=currentNode;
			}else{

			}
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
				logs().main.write("error, object "+currentObject->name+" doesn't have a material "+String(currentObject->type));
				return false;

			}

			bool transhack=false;

			if(currentObject->material<materialCount && currentObject->material>=0){
				//in case this was saved as a dev build, in which case it can't know if a texture is transparent...
				//transparent objects don't get put in either static or nonstatic

				if(materials[currentObject->material].textureDiffuse<textureCount && materials[currentObject->material].textureDiffuse>=0){

					if(textures[materials[currentObject->material].textureDiffuse].bytesPP==4){
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
				
				nonstaticObjects.pushBack(currentObject);
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);

			}else if(!(currentObject->flags & OBJECT_TRANSPARENT) &&
				!(currentObject->flags & OBJECT_STATIC) &&
				(currentObject->noclip) &&
				!transhack){
				

				//noclip objs need to be nonstatic
				nonstaticObjects.pushBack(currentObject);
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);
			}else if(!(currentObject->flags & OBJECT_TRANSPARENT) &&
				(currentObject->flags & OBJECT_STATIC) &&
				(currentObject->constraintPosName!="" || currentObject->constraintRotName!="" || currentObject->constraintScaleName!="") &&
				!transhack &&
				version==2){
				//constrained objs need to be nonstatic

				nonstaticObjects.pushBack(currentObject);
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);
			}else if(!(currentObject->flags & OBJECT_TRANSPARENT) &&
				(currentObject->flags & OBJECT_STATIC) &&
				version==2 &&
				!transhack){
				//objs with nonstatic parents need to be nonstatic

				if(sceneGraph.nonstatic(currentObject->sceneGraphNode)){
					nonstaticObjects.pushBack(currentObject);
					materials[currentObject->material].nonstaticRefs++;
					materials[currentObject->material].nonstaticRefList.pushBack(currentObject);
				}

			}else if( (materials[currentObject->material].reflectivity.x>0 || 
				materials[currentObject->material].reflectivity.y>0 ||
				materials[currentObject->material].reflectivity.z>0 ||
				materials[currentObject->material].reflectivity.w>0) &&
				!(currentObject->flags & OBJECT_WATER) &&
				!(currentObject->flags & OBJECT_TRANSPARENT) &&
				!(materials[currentObject->material].shader & MATERIAL_SHADER_ENVIRONMENT) &&
				! transhack){

				//REAL reflective objects

				reflectiveObjects.pushBack(currentObject);

			}else if((transhack || currentObject->flags & OBJECT_TRANSPARENT) && !(currentObject->flags & OBJECT_WATER)){

				//transparent objects don't get put in either static or nonstatic

				transparentObjects.pushBack(currentObject);
				if(!(currentObject->flags & OBJECT_STATIC)){
					materials[currentObject->material].nonstaticRefs++;
					materials[currentObject->material].nonstaticRefList.pushBack(currentObject);
				}
			}else if(currentObject->flags & OBJECT_STATIC && !(currentObject->flags & OBJECT_WATER)){
				//plain static objects
				staticObjects.pushBack(currentObject);
			}else if(!(currentObject->flags & OBJECT_STATIC  && !(currentObject->flags & OBJECT_WATER))){
				//plain nonstatic objects
				nonstaticObjects.pushBack(currentObject);
				materials[currentObject->material].nonstaticRefs++;
				materials[currentObject->material].nonstaticRefList.pushBack(currentObject);
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
			}else if(currentObject->type==9 && currentCurve!=NULL){

				if(!(currentObject->flags & OBJECT_STATIC)){
					luaUpload(currentCurve);
				}
			}

			if(currentObject->flags & OBJECT_WATER){
				water=currentObject;
			}
		}

		currentNull=NULL;
		updateProgressBar();
	}

	delete[] indices;
	logs().main.write("OBJECTS:"+String(engineTime.getTimerSingle("OBJECTS")));

	engineTime.startTimerSingle("ENDFUNCTS");

	//call create functions
	for(int i=0; i<objects.size(); i++){

		if(objects[i]->action!=""){
			script().run(objects[i]->action+"(\"create\")");
		}
	}

	//set camera
	if(cameras.size()>0){
		camera=cameras[0];
	}

	/*upload textures to OGL
	have to do this down here at the end to deal with cube maps
	cube maps all have to be bound to the same target, so we tag them when making materials
	the face info is stored in materials, not textures
	*/

	for(unsigned int i=0; i<localTextureCount; i++){

		bool prefound=false;

		for(int t=0; t<=baseTexture; t++){
			if(textures[t].file==textures[baseTexture+i].file && textures[t].ref){
					if(t!=baseTexture+i){
						prefound=true;

						textures[baseTexture+i].glNameSet(true);
						textures[baseTexture+i].glName(textures[t].glName());
					}
			}
		}

		if(!prefound){
			if(!texture.load(&textures[baseTexture+i])){
				return false;
			}

			*textures[baseTexture+i].dataLoaded=false;
			delete[] textures[baseTexture+i].data;
		}else{

		}

		updateProgressBar();
	}


	//update all
	int ss=0;

	for(unsigned int i=0; i<objects.size(); i++){
		objects[i]->update();

		if(ss==3){	// 1/5 of the time

			updateProgressBar();
			ss=0;
		}else{
			ss++;
		}
	}

	ss=0;

	for(unsigned int i=0; i<materials.size(); i++){
		materials[i].update();

		if(ss==3){	// 1/5 of the time

			updateProgressBar();
			ss=0;
		}else{
			ss++;
		}
	}

	conf->forceTextureLevelChanged();
	updateTextureLevel();

	engineTime.skipNextFrame();

	logs().main.write("ENDFUNCTS:"+String(engineTime.getTimerSingle("ENDFUNCTS")));

	logs().main.write("TOTALLOAD:"+String(engineTime.getTimerSingle("TOTALLOAD")));

	currentlyLoading=false;

	//file versions <2 don't have the data to support scene graph functs
	if(version<2){
		conf->sceneGraph=false;
	}

	return true;
}

#endif

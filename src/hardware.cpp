#include "hardware.hpp"
#include "script.hpp"
#include "os.h"
#include "sort.hpp"
#include "HelperLibMath.hpp"
#include "Log.hpp"
#include "GlHeader.hpp"
#include "conf.h"

Hardware hardware;

bool Hardware::GetExtensionsString(){
	#ifdef _WIN32
		
	#else
		extensions = (char *) glGetString(GL_EXTENSIONS);
	#endif

	return true;
}

bool Hardware::isExtensionSupported(char *extension){
	#ifdef _WIN32

	#else
		if (extensions){
			char *str = extensions;
			int len = strlen(extension);
			
			while ((str = strstr(str, extension)) != NULL){
				str += len;
				if (*str == ' ' || *str == '\0') return true;
			}
		}
	#endif

	return false;
}

void Hardware::minimumDebug(){

	if(conf->minimumHardwareDebug){
		logs().renderer.write("Minimum Hardware Debug enabled, all extensions have been DISABLED");
		glsl=false;
		vertexProgram=false;
		fragmentProgram=false;
		vbo=false;

	}
}

bool Hardware::init(){

	char* s=(char*)glGetString(GL_VENDOR);

	if(s[0]=='N' &&
		s[1]=='V' &&
		s[2]=='I' &&
		s[3]=='D' &&
		s[4]=='I' &&
		s[5]=='A'){

		vendor="nvidia";
	}else if(s[0]=='A' &&
		s[1]=='T' &&
		s[2]=='I'){
		vendor="ati";
	}else{
		vendor="unknown";
	}

	char* r=(char*)glGetString(GL_RENDERER);
	
	char* v=(char*)glGetString(GL_VERSION);
	

	logs().renderer.write(String("VENDOR:")+s);
	logs().renderer.write(String("RENDERER:")+r);
	logs().renderer.write(String("VERSION:")+v);

	GLint maxlight=0;
	glGetIntegerv(GL_MAX_LIGHTS,&maxlight);
	logs().renderer.write(String("MAX_LIGHTS:")+String((int)maxlight));

	#ifdef _WIN32
	
	#else
		GetExtensionsString();
		if (isExtensionSupported("GL_ARB_multitexture")){
		
		}else{
			return false;
		}
		
		if (isExtensionSupported("GL_ARB_shader_objects")){
			glsl=true;
		}else{
			glsl=false;
		}
		
		if (isExtensionSupported("GL_ARB_vertex_program")){
			vertexProgram=true;
		}else{
			vertexProgram=false;
		}
		
		if (isExtensionSupported("GL_ARB_fragment_program")){
			fragmentProgram=true;
		}else{
			fragmentProgram=false;
		}
		
		if (isExtensionSupported("GL_ARB_vertex_buffer_object")){
			vbo=true;
		}else{
			vbo=false;
		}

		if(conf->minimumHardwareDebug){
			vbo=false;
			fragmentProgram=false;
			glsl=false;
			vertexProgram=false;
		}
	#endif
	

#ifdef _WIN32
	String platform="['windows']";
#elif LINUX
	String platform="['linux']";
#else
	String platform="['mac']";
#endif
	
	script().run("if hardware.compatibility"+platform+"==nil then hardwarehack.slowTextureLevel=true else hardwarehack.slowTextureLevel=false end");

	if(compatibility.slowTextureLevel){
		platform="['all']";
	}

	String vendor="['"+String(s)+"']";

	script().run("if hardware.compatibility"+platform+vendor+"==nil then hardwarehack.slowTextureLevel=true else hardwarehack.slowTextureLevel=false end");

	if(compatibility.slowTextureLevel){
		vendor="['all']";
	}

	String model="['"+String(r)+"']";

	script().run("if hardware.compatibility"+platform+vendor+model+"==nil then hardwarehack.slowTextureLevel=true else hardwarehack.slowTextureLevel=false end");

	if(compatibility.slowTextureLevel){
		model="['all']";
	}

	String index=platform+vendor+model;


	logs().renderer.write(index);

	script().run("hardwarehack.slowTextureLevel=hardware.compatibility"+index+".slowTextureLevel");
	script().run("hardwarehack.shaders=hardware.compatibility"+index+".shaders");
	script().run("hardwarehack.mixer1_hardware_vp=hardware.compatibility"+index+".mixer1_hardware_vp");
	script().run("hardwarehack.mixer2_hardware_vp=hardware.compatibility"+index+".mixer2_hardware_vp");
	script().run("hardwarehack.mixer3_hardware_vp=hardware.compatibility"+index+".mixer3_hardware_vp");
	script().run("hardwarehack.mixer1_fp=hardware.compatibility"+index+".mixer1_fp");
	script().run("hardwarehack.mixer2_fp=hardware.compatibility"+index+".mixer2_fp");
	script().run("hardwarehack.mixer3_fp=hardware.compatibility"+index+".mixer3_fp");

	if(!compatibility.shaders){
		fragmentProgram=false;
		vertexProgram=false;
		glsl=false;
	}

	if(fragmentProgram){
		logs().renderer.write("FRAGMENT PROGRAM SUPPORT");
	}else{
		logs().renderer.write("FRAGMENT PROGRAM DISABLED");
	}
	
	return true;
}

Hardware::Hardware(){
	fragmentProgram=false;
	vertexProgram=false;
	glsl=false;
	audio=false;
	vbo=false;
}

ScreenMode Hardware::validateScreenMode(int sizex, int sizey, int bitDepth){
	Array<ScreenMode> smodes=os().screenModes();

	Array<SortItem> si;

	int foundx=0;
	int foundy,foundbit;

	bool ffoundx=false,ffoundy=false,ffoundbit=false;

	for(int i=0; i<smodes.size(); i++){
		si.pushBack(SortItem(i,smodes[i].x));
	}

	Array<int> r1=sort.sort(si,true);

	for(int i=0; i<r1.size(); i++){
		if(smodes[r1[i]].x<=sizex || !ffoundx){
			foundx=smodes[r1[i]].x;
			ffoundx=true;
		}else{
			if( math.abs(smodes[r1[i]].x-sizex) < math.abs(foundx-sizex) ){
				foundx=smodes[r1[i]].x;
			}

			break;
		}
	}

	si.clear();

	for(int i=0; i<r1.size(); i++){
		if(smodes[r1[i]].x==foundx){
			si.pushBack(SortItem(r1[i],smodes[r1[i]].y));
		}
	}

	r1.clear();
	r1=sort.sort(si,true);

	for(int i=0; i<r1.size(); i++){
		if(smodes[r1[i]].y<=sizey || !ffoundy){
			foundy=smodes[r1[i]].y;
			ffoundy=true;
		}else{
			if( math.abs(smodes[r1[i]].y-sizey) < math.abs(foundy-sizey) ){
				foundy=smodes[r1[i]].y;
			}

			break;
		}
	}

	si.clear();

	for(int i=0; i<r1.size(); i++){
		if(smodes[r1[i]].x==foundx && smodes[r1[i]].y==foundy){
			si.pushBack(SortItem(r1[i],smodes[r1[i]].bitDepth));
		}
	}

	r1.clear();
	r1=sort.sort(si,true);

	for(int i=0; i<r1.size(); i++){
		if(smodes[r1[i]].bitDepth<=bitDepth|| !ffoundbit){
			foundbit=smodes[r1[i]].bitDepth;
			ffoundbit=true;
		}else{
			if( math.abs(smodes[r1[i]].bitDepth-bitDepth) < math.abs(foundbit-bitDepth) ){
				bitDepth=smodes[r1[i]].bitDepth;
			}

			break;
		}
	}

	ScreenMode s;
	s.x=foundx;
	s.y=foundy;
	s.bitDepth=foundbit;

	return s;
}


static int luaValidateScreenMode(lua_State *L){
	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=3){
		console().write("validateScreenMode error: bad arguments");
		return 0;
	}

	int sizex=0;
	int sizey=0;
	int bit=0;

	if(args[0].type==SCRIPT_DOUBLE_STORED){
		sizex=args[0].value.ds;

	}else{
		console().write("validateScreenMode error: bad arguments");
		return 0;
	}

	if(args[1].type==SCRIPT_DOUBLE_STORED){
		sizey=args[1].value.ds;

	}else{
		console().write("validateScreenMode error: bad arguments");
		return 0;
	}


	if(args[2].type==SCRIPT_DOUBLE_STORED){
		bit=args[2].value.ds;

	}else{
		console().write("validateScreenMode error: bad arguments");
		return 0;
	}
	
	ScreenMode s=hardware.validateScreenMode(sizex,sizey,bit);

	script().resultTable(L);
	script().result(L,"x",s.x);
	script().result(L,"y",s.y);
	script().result(L,"bitDepth",s.bitDepth);

	return 1;
}


bool Hardware::scriptInit(){

	luaHardware=script().add("hardware");

			script().add("fragmentProgram",&fragmentProgram,luaHardware);
			script().add("vbo",&vbo,luaHardware);
			script().add("audio",&audio,luaHardware);
			script().add("vendor",&vendor,luaHardware);

			script().add("textureMemoryUsed",&textureMemoryUsed,luaHardware);

			script().add("validateScreenMode",luaValidateScreenMode,luaHardware);
			LuaType* sm=script().add("screenModes",luaHardware);

			Array<ScreenMode> smodes=os().screenModes();

			LuaType* smc;
			LuaType* sms;


			for(int i=0; i<smodes.size(); i++){
				smc=script().add(i+1,sm);

					sms=script().add("size",smc);
						script().add("x",smodes[i].x,sms);
						script().add("y",smodes[i].y,sms);

					script().add("bitDepth",smodes[i].bitDepth,smc);
					script().add("refreshRate",smodes[i].refreshRate,smc);
		
			}

		LuaType* cs=script().add("compatibility",luaHardware);

			LuaType* ca=script().add("all",cs);

				LuaType* alr=script().add("all",ca);
					LuaType* al=script().add("all",alr);
						script().add("slowTextureLevel",true,al);
						script().add("shaders",true,al);
						script().add("mixer1_hardware_vp","shaders/mixer1_hardware.vp",al);
						script().add("mixer2_hardware_vp","shaders/mixer2_hardware.vp",al);
						script().add("mixer3_hardware_vp","shaders/mixer3_hardware.vp",al);
						script().add("mixer1_fp","shaders/mixer1.fp",al);
						script().add("mixer2_fp","shaders/mixer2.fp",al);
						script().add("mixer3_fp","shaders/mixer3.fp",al);

					LuaType* nvsa=script().add("NVIDIA Corporation",ca);
						LuaType* nva=script().add("all",nvsa);
							script().add("slowTextureLevel",true,nva);
							script().add("shaders",true,nva);
							script().add("mixer1_hardware_vp","shaders/mixer1_hardware_nvidia.vp",nva);
							script().add("mixer2_hardware_vp","shaders/mixer2_hardware_nvidia.vp",nva);
							script().add("mixer3_hardware_vp","shaders/mixer3_hardware_nvidia.vp",nva);
							script().add("mixer1_fp","shaders/mixer1_nvidia.fp",nva);
							script().add("mixer2_fp","shaders/mixer2_nvidia.fp",nva);
							script().add("mixer3_fp","shaders/mixer3_nvidia.fp",nva);

			LuaType* c=script().add("windows",cs);

				LuaType* ar=script().add("all",c);
					LuaType* a=script().add("all",ar);
						script().add("slowTextureLevel",true,a);
						script().add("shaders",true,a);
						script().add("mixer1_hardware_vp","shaders/mixer1_hardware.vp",a);
						script().add("mixer2_hardware_vp","shaders/mixer2_hardware.vp",a);
						script().add("mixer3_hardware_vp","shaders/mixer3_hardware.vp",a);
						script().add("mixer1_fp","shaders/mixer1.fp",a);
						script().add("mixer2_fp","shaders/mixer2.fp",a);
						script().add("mixer3_fp","shaders/mixer3.fp",a);

				LuaType* atis=script().add("ATI Technologies Inc.",c);
					LuaType* ati=script().add("all",atis);
						script().add("slowTextureLevel",true,ati);
						script().add("shaders",true,ati);
						script().add("mixer1_hardware_vp","shaders/mixer1_hardware.vp",ati);
						script().add("mixer2_hardware_vp","shaders/mixer2_hardware.vp",ati);
						script().add("mixer3_hardware_vp","shaders/mixer3_hardware.vp",ati);
						script().add("mixer1_fp","shaders/mixer1.fp",ati);
						script().add("mixer2_fp","shaders/mixer2.fp",ati);
						script().add("mixer3_fp","shaders/mixer3.fp",ati);

				LuaType* nvs=script().add("NVIDIA Corporation",c);
					LuaType* nv=script().add("all",nvs);
						script().add("slowTextureLevel",true,nv);
						script().add("shaders",true,nv);
						script().add("mixer1_hardware_vp","shaders/mixer1_hardware_nvidia.vp",nv);
						script().add("mixer2_hardware_vp","shaders/mixer2_hardware_nvidia.vp",nv);
						script().add("mixer3_hardware_vp","shaders/mixer3_hardware_nvidia.vp",nv);
						script().add("mixer1_fp","shaders/mixer1_nvidia.fp",nv);
						script().add("mixer2_fp","shaders/mixer2_nvidia.fp",nv);
						script().add("mixer3_fp","shaders/mixer3_nvidia.fp",nv);

				LuaType* its=script().add("Intel",c);
					LuaType* it=script().add("all",its);
						script().add("slowTextureLevel",true,it);
						script().add("shaders",false,it);
						script().add("mixer1_hardware_vp","shaders/mixer1_hardware.vp",it);
						script().add("mixer2_hardware_vp","shaders/mixer2_hardware.vp",it);
						script().add("mixer3_hardware_vp","shaders/mixer3_hardware.vp",it);
						script().add("mixer1_fp","shaders/mixer1.fp",it);
						script().add("mixer2_fp","shaders/mixer2.fp",it);
						script().add("mixer3_fp","shaders/mixer3.fp",it);

	LuaType* hh=script().add("hardwarehack");
		script().add("slowTextureLevel",&compatibility.slowTextureLevel,hh);
		script().add("shaders",&compatibility.shaders,hh);
		script().add("mixer1_hardware_vp",&compatibility.mixer1_hardware_vp,hh);
		script().add("mixer2_hardware_vp",&compatibility.mixer2_hardware_vp,hh);
		script().add("mixer3_hardware_vp",&compatibility.mixer3_hardware_vp,hh);
		script().add("mixer1_fp",&compatibility.mixer1_fp,hh);
		script().add("mixer2_fp",&compatibility.mixer2_fp,hh);
		script().add("mixer3_fp",&compatibility.mixer3_fp,hh);

	return true;
}

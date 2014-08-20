#include "shader.hpp"
#include "Extensions.h"
#include "Log.hpp"
#include "script.hpp"
#include "hardware.hpp"
#include "md5.h"

Shader shader;


ShaderData::ShaderData(){
	program=0;
	vertexShader=0;
	fragmentShader=0;

	vertexLength=0;
	fragmentLength=0;

	type=0;

}

void Shader::createShader(ShaderData &shader){

	if(shader.lang==LANG_GLSL){

		if(!hardware.glsl){
			return;
		}

		if(shader.program==0){
			shader.program = glCreateProgramObjectARB();
		}else{
			//glDeleteObjectARB(shader.program);
		}

		
		if (shader.type == GL_FRAGMENT_SHADER_ARB){

			shader.fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		}else{

			shader.vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
		}

	}else{
 	
 		if (shader.type == GL_FRAGMENT_PROGRAM_ARB){
	
			if(!hardware.fragmentProgram){
				return;
			}

			glGenProgramsARB(1, (GLuint*)&shader.id);
			glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, shader.id);
		}else if (shader.type == GL_VERTEX_PROGRAM_ARB){
		
			if(!hardware.vertexProgram){
				return;
			}


			glGenProgramsARB(1,(GLuint*) &shader.id);
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB, shader.id);
		}
		 
	}

}


static int luaReload(lua_State *L){

	String name=luaL_checkstring(L,1);



	shader.reload(name);
	return 0;

}

Shader::Shader(){
	currentShaderId=SHADER_NONE;
	currentShaderPack=SHADER_NONE;

	

}

bool Shader::scriptInit(){
	LuaType* luaAddress=script().add("shader");
	script().add("reload",luaReload,luaAddress);
	return true;
}

bool Shader::uploadShader(ShaderData &shader){

	if(shader.lang==LANG_GLSL){

		if(!hardware.glsl){
			return SHADER_NONE;
		}

		GLint vsResult;
		char infoLog[2048];
		GLsizei len=0;

		if(shader.fragmentShader!=0  && shader.fragmentLength!=0){

			glShaderSourceARB(shader.fragmentShader, 1, (const GLcharARB**)&shader.fragmentSource, (GLint*)&shader.fragmentLength);
			glCompileShaderARB(shader.fragmentShader);

			glGetObjectParameterivARB(shader.fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &vsResult);

			if (vsResult){
				
			} else {

				glGetInfoLogARB(shader.fragmentShader, sizeof(infoLog), &len, infoLog);

				logs().renderer.write(String("Fragment Shader Error:")+infoLog);
				return false;

			}

			glAttachObjectARB(shader.program, shader.fragmentShader);

			shader.fragmentLength=0;
		}

		if(shader.vertexShader!=0  && shader.vertexLength!=0){

			glShaderSourceARB(shader.vertexShader, 1, (const GLcharARB**)&shader.vertexSource, (GLint*)&shader.vertexLength);
			glCompileShaderARB(shader.vertexShader);

			glGetObjectParameterivARB(shader.vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &vsResult);

			if (vsResult){
				
			} else {

				glGetInfoLogARB(shader.vertexShader, sizeof(infoLog), &len, infoLog);

				logs().renderer.write(String("Vertex Shader Error:")+infoLog);
				return false;

			}
			
			glAttachObjectARB(shader.program, shader.vertexShader);

			shader.vertexLength=0;
		}

		
		glLinkProgramARB(shader.program);

		glGetInfoLogARB(shader.program, sizeof(infoLog), &len, infoLog);

		logs().renderer.write(infoLog);

		return true;

	}else{
		if(shader.type==GL_FRAGMENT_PROGRAM_ARB){
			if(!hardware.fragmentProgram){
				return false;
			}

			glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, shader.fragmentLength, shader.fragmentSource); 
		}else if(shader.type==GL_VERTEX_PROGRAM_ARB){

			if(!hardware.vertexProgram){
				return false;
			}

			glProgramStringARB(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, shader.vertexLength, shader.vertexSource); 
		}

		const unsigned char* test=glGetString(GL_PROGRAM_ERROR_STRING_ARB);
	
		char* test2=(char*)test;
	
		std::string error=test2;
	
		if(error!=""){
			logs().renderer.write("[Shader][ERROR] "+error);

			//if we get a shader error, we'll just disable shaders
			if(shader.type==GL_FRAGMENT_PROGRAM_ARB){
				hardware.fragmentProgram=false;
			}else{
				hardware.vertexProgram=false;
			}

			reset();

			logs().renderer.write("[Shader] Shaders Disabled");

			return false;
		}
	}


	return true;
}

bool Shader::reloadFragmentFile(Path program){

	if(!hardware.fragmentProgram){
		return SHADER_NONE;
	}

#ifdef DEVELOPER
	if(!os().fileExists(program)){
		program.setRelative("../tools/"+program.getRelative());
	}
#endif

	logs().renderer.write("reload fragment shader "+program.getRelative());
	

	ShaderId id=-1;
	
	if(!hardware.fragmentProgram){
		return false;
	}
	
	for(int i=0; i<shaders.size(); i++){
		if(shaders[i].file==program){
			id=i;
			break;
		}

	}

	if(id==-1){
		console().write("shader '"+program.getRelative()+"' not found");
		return false;
	}



	shaders[id].type = GL_FRAGMENT_PROGRAM_ARB;
	shaders[id].lang= LANG_ASM;

	createShader(shaders[id]);
	
	if (program != ""){
		FILE *file = fopen(program.getAbsolute().c_str(), "rb");

		if (file == NULL) return false;
		
		// Find file size
		fseek(file,  0, SEEK_END);
		shaders[id].fragmentLength = ftell(file);
		fseek(file,  0, SEEK_SET);


		shaders[id].fragmentSource = new char[shaders[id].fragmentLength];
		fread(shaders[id].fragmentSource,shaders[id].fragmentLength, 1, file);
		fclose(file);
		
		uploadShader(shaders[id]);

	}else{
		logs().renderer.write("fragment program doesn't exist");
	}

	
	return true;
	
}



bool Shader::reloadVertexFile(Path program){

	if(!hardware.vertexProgram){
		return SHADER_NONE;
	}

#ifdef DEVELOPER
	if(!os().fileExists(program)){
		program.setRelative("../tools/"+program.getRelative());
	}
#endif

	logs().renderer.write("reload vertex shader "+program.getRelative());
	

	ShaderId id=-1;
	
	
	for(int i=0; i<shaders.size(); i++){
		if(shaders[i].file==program){
			id=i;
			break;
		}
	}

	if(id==-1){
		console().write("shader '"+program.getRelative()+"' not found");
		return false;
	}

	shaders[id].type = GL_VERTEX_PROGRAM_ARB;
	shaders[id].lang= LANG_ASM;

	createShader(shaders[id]);
	
	if (program != ""){
		FILE *file = fopen(program.getAbsolute().c_str(), "rb");

		if (file == NULL) return false;
		
		// Find file size
		fseek(file,  0, SEEK_END);
		shaders[id].vertexLength = ftell(file);
		fseek(file,  0, SEEK_SET);


		shaders[id].vertexSource = new char[shaders[id].vertexLength];
		fread(shaders[id].vertexSource,shaders[id].vertexLength, 1, file);
		fclose(file);
		
		uploadShader(shaders[id]);
	}else{
		logs().renderer.write("vertex program doesn't exist");
	}

	return true;
}


void rebuildLists();


bool Shader::reload(String program){

#ifdef DEVELOPER
	if(!os().fileExists(program)){
		program="../tools/"+program;
	}
#endif

	logs().renderer.write("reload shader "+program);
	

	ShaderId id=-1;
	
	
	for(int i=0; i<shaders.size(); i++){
		if(shaders[i].file==program){
			id=i;
			break;
		}

	}

	if(id==-1){
		console().write("shader '"+program+"' not found");
		return false;
	}


	if(shaders[id].lang==LANG_GLSL){
		if(shaders[id].type==GL_FRAGMENT_SHADER_ARB){
			return reloadFragmentFile(program);
		}else{
			return reloadVertexFile(program);
		}
	}else{
		if(shaders[id].type==GL_FRAGMENT_PROGRAM_ARB){
			int x=reloadFragmentFile(program);
			rebuildLists();
			return x;
		}else{
			int x=reloadVertexFile(program);
			rebuildLists();
			return x;
		}
	}	
}


ShaderId Shader::addFragmentFile(Path filename, String md5h){

#ifdef DEVELOPER
	if(!os().fileExists(filename)){
		filename.setRelative("../tools/"+filename.getRelative());

		if(!os().fileExists(filename)){
			filename.setRelative("../"+filename.getRelative());
		}

	}
#endif

	logs().renderer.write("adding fragment shader "+filename.getRelative());
	
	if(!hardware.fragmentProgram){
		logs().renderer.write("No support for fragment programs");
		return SHADER_NONE;
	}


	ShaderId id;
	
	shaders.pushBack();
	id=shaders.size()-1;
	
	if(filename.getExtension()=="glf"){
		shaders[id].type = GL_FRAGMENT_SHADER_ARB;
		shaders[id].lang=LANG_GLSL;
	}else if(filename.getExtension()=="fp"){
		shaders[id].type = GL_FRAGMENT_PROGRAM_ARB;
		shaders[id].lang=LANG_ASM;
	}else{
		logs().renderer.write("Bad file type "+filename.getExtension());
		return SHADER_NONE;
	}

	shaders[id].file=filename;

	createShader(shaders[id]);
	
	if (filename != ""){

		FILE *file = fopen(filename.getAbsolute().c_str(), "rb");

		if (file == NULL){
			logs().renderer.write("Could not open file '"+filename.getAbsolute()+"'");
			return SHADER_NONE;
		}
		
		// Find file size
		fseek(file,  0, SEEK_END);
		shaders[id].fragmentLength = ftell(file);
		fseek(file,  0, SEEK_SET);

		shaders[id].fragmentSource = new char[shaders[id].fragmentLength];

		fread(shaders[id].fragmentSource,shaders[id].fragmentLength, 1, file);
		fclose(file);
		
		if(md5h!=""){
			char* hexd=new char[33];

			MD5 md5;
			md5.update((unsigned char*)shaders[id].fragmentSource,shaders[id].fragmentLength);
			md5.finalize();
			hexd=md5.hex_digest();

			for(int i=0; i<32; i++){
				if(hexd[i]!=md5h[i]){
					logs().renderer.write("shader "+filename.getAbsolute()+" different from expected!");
					break;
				}
			}
		}

		uploadShader(shaders[id]);
	}else{
		logs().renderer.write("fragment program doesn't exist");
		return SHADER_NONE;
	}


	return id;

}



ShaderId Shader::addFragment(String code){

	logs().renderer.write("adding fragment shader from code");
	
	if(!hardware.fragmentProgram){
		logs().renderer.write("No support for fragment programs");
		return SHADER_NONE;
	}


	ShaderId id;
	
	shaders.pushBack();
	id=shaders.size()-1;
	
	shaders[id].type = GL_FRAGMENT_PROGRAM_ARB;
	shaders[id].lang=LANG_ASM;

	createShader(shaders[id]);
	
	shaders[id].fragmentLength=code.size();

	shaders[id].fragmentSource = new char[shaders[id].fragmentLength];

	memcpy(shaders[id].fragmentSource,code.c_str(),code.size());
	
	uploadShader(shaders[id]);

	return id;

}

ShaderId Shader::addVertexFile(Path filename, String md5h){

	if(!hardware.vertexProgram){
		return SHADER_NONE;
	}

#ifdef DEVELOPER
	if(!os().fileExists(filename)){
		filename.setRelative("../tools/"+filename.getRelative());

		if(!os().fileExists(filename)){
			filename.setRelative("../"+filename.getRelative());
		}
	}
#endif

	logs().renderer.write("adding vertex shader "+filename.getRelative());
	
	ShaderId id;
	
	shaders.pushBack();
	id=shaders.size()-1;
	

	if(filename.getExtension()=="glv"){

		shaders[id].type = GL_VERTEX_SHADER_ARB;
		shaders[id].lang=LANG_GLSL;
	}else if(filename.getExtension()=="vp"){

		shaders[id].type = GL_VERTEX_PROGRAM_ARB;
		shaders[id].lang=LANG_ASM;
	}else{
		logs().renderer.write("Bad file type "+filename.getExtension());
		return SHADER_NONE;
	}

	if(shaders[id].lang==LANG_ASM && !hardware.vertexProgram){
		logs().renderer.write("No support for vertex programs");
		return SHADER_NONE;
	}


	if(shaders[id].lang==LANG_GLSL && !hardware.glsl){
		logs().renderer.write("No support for glsl vertex programs");
		return SHADER_NONE;
	}

	shaders[id].file=filename;

	createShader(shaders[id]);
	
	if (filename != ""){
	
		FILE *file = fopen(filename.getAbsolute().c_str(), "rb");
		
		if (file == NULL){
			logs().renderer.write("Could not open file '"+filename.getAbsolute()+"'");
			return SHADER_NONE;
		}

		// Find file size
		fseek(file,  0, SEEK_END);
		shaders[id].vertexLength = ftell(file);
		fseek(file,  0, SEEK_SET);

		shaders[id].vertexSource = new char[shaders[id].vertexLength];
		fread(shaders[id].vertexSource,shaders[id].vertexLength, 1, file);
		fclose(file);
		
		if(md5h!=""){
			char* hexd=new char[33];

			MD5 md5;
			md5.update((unsigned char*)shaders[id].vertexSource,shaders[id].vertexLength);
			md5.finalize();
			hexd=md5.hex_digest();

			for(int i=0; i<32; i++){
				if(hexd[i]!=md5h[i]){
					logs().renderer.write("shader "+filename.getAbsolute()+" different from expected!");
					break;
				}
			}
		}

		uploadShader(shaders[id]);

	}else{

		logs().renderer.write("fragment program doesn't exist");
		return SHADER_NONE;
	}


	return id;

}



ShaderId Shader::addVertexFile(Path filename,ShaderId link, String md5h){

	if(!hardware.vertexProgram){
		return SHADER_NONE;
	}

#ifdef DEVELOPER
	if(!os().fileExists(filename)){
		filename.setRelative("../tools/"+filename.getRelative());

		if(!os().fileExists(filename)){
			filename.setRelative("../"+filename.getRelative());
		}
	}
#endif

	logs().renderer.write("adding vertex shader "+filename.getRelative());
	

	if(link==SHADER_NONE){
		logs().renderer.write("No shader to link to, '"+filename.getRelative()+"'");
		return SHADER_NONE;
	}
	

	if(filename.getExtension()=="glv"){

		shaders[link].type = GL_VERTEX_SHADER_ARB;
		shaders[link].lang=LANG_GLSL;
	}else{
		logs().renderer.write("Bad file type "+filename.getExtension());
		return SHADER_NONE;
	}



	if(shaders[link].lang==LANG_GLSL && !hardware.glsl){
		logs().renderer.write("No support for glsl vertex programs");
		return SHADER_NONE;
	}


	createShader(shaders[link]);
	
	if (filename != ""){
		FILE *file = fopen(filename.getAbsolute().c_str(), "rb");

		if (file == NULL){
			logs().renderer.write("Could not open file '"+filename.getAbsolute()+"'");
			return SHADER_NONE;
		}

		// Find file size
		fseek(file,  0, SEEK_END);
		shaders[link].vertexLength = ftell(file);
		fseek(file,  0, SEEK_SET);

		shaders[link].vertexSource = new char[shaders[link].vertexLength];
		fread(shaders[link].vertexSource,shaders[link].vertexLength, 1, file);
		fclose(file);
		
		if(md5h!=""){
			char* hexd=new char[33];

			MD5 md5;
			md5.update((unsigned char*)shaders[link].vertexSource,shaders[link].vertexLength);
			md5.finalize();
			hexd=md5.hex_digest();

			for(int i=0; i<32; i++){
				if(hexd[i]!=md5h[i]){
					logs().renderer.write("shader "+filename.getAbsolute()+" different from expected!");
					break;
				}
			}
		}

		uploadShader(shaders[link]);
	}else{
		logs().renderer.write("vertex shader doesn't exist");
		return SHADER_NONE;
	}


	return link;
}
	
void Shader::set(ShaderPackId packId, ShaderId shaderId){

 	if(currentShaderId==shaderId && currentShaderPack==packId){
 		return;
 	}
 	
  	if(packId<packs.size()){
  		
  	
  	}else{
  		logs().renderer.write("[Shader][ERROR] Pack out of range");
  		return;
  	}
  	
  	if(shaderId<packs[packId].shaders.size()){
  	
	}else{
  		logs().renderer.write("[Shader][ERROR] Id out of range");
  		return;
	}
  		

	if(shaderId == SHADER_NONE){

		logs().renderer.write("Shader error, no shader");

  		glDisable(packs[packId].shaders[shaderId].type);
	}else{
		glEnable(packs[packId].shaders[shaderId].type);
		
		glBindProgramARB(packs[packId].shaders[shaderId].type, packs[packId].shaders[shaderId].id);

	}
 	
 	currentShaderId = shaderId;
 	currentShaderPack = packId;
}


void Shader::set(ShaderId shaderId){

	if(shaderId == SHADER_NONE){
		logs().renderer.write("Shader error, no shader");
	}else{

		if(shaders[shaderId].lang==LANG_GLSL){

			if(!hardware.glsl){
				return;
			}

			glUseProgramObjectARB(shaders[shaderId].program);
		}else{

			glEnable(shaders[shaderId].type);
	
			glBindProgramARB(shaders[shaderId].type, shaders[shaderId].id);
		}
	}
		

}

bool Shader::reset(){

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: Shader::reset glUseProgramObjectARB");
#endif

	if(hardware.glsl){
		glUseProgramObjectARB(0);
	}

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: Shader::reset DISABLE");
#endif

	glDisable(GL_FRAGMENT_PROGRAM_ARB);
	glDisable(GL_VERTEX_PROGRAM_ARB);


	currentShaderId=SHADER_NONE;
	currentShaderPack=SHADER_NONE;

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: Shader::reset END");
#endif

	return true;
}

void Shader::localParameterFragment(unsigned int index,FloatVector4d value){
	if(hardware.fragmentProgram){
		glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, index, value.x,  value.y,  value.z,  value.w );
	}
}

void Shader::localParameterFragment(unsigned int index,double x, double y, double z, double w){
	if(hardware.fragmentProgram){
		glProgramLocalParameter4dARB(GL_FRAGMENT_PROGRAM_ARB, index, x,  y, z, w );
	}
}

void Shader::localParameterFragment(unsigned int index,FloatVector3d value){
	if(hardware.fragmentProgram){
		glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, index, value.x,  value.y,  value.z,0);
	}
}


void Shader::localParameterFragment(unsigned int index,float value){
	if(hardware.fragmentProgram){
		glProgramLocalParameter4fARB(GL_FRAGMENT_PROGRAM_ARB, index, value, value, value, value );
	}
}


void Shader::localParameterFragment(unsigned int index,double* value){
	if(hardware.fragmentProgram){
		glProgramLocalParameter4dvARB(GL_FRAGMENT_PROGRAM_ARB, index, value);
	}
}


void Shader::localParameterVertex(unsigned int index,FloatVector4d value){
	if(hardware.vertexProgram){
		glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, index, value.x,  value.y,  value.z,  value.w );
	}
}

void Shader::localParameterVertex(unsigned int index,double x, double y, double z, double w){
	if(hardware.vertexProgram){
		glProgramLocalParameter4dARB(GL_VERTEX_PROGRAM_ARB, index, x,  y, z, w );
	}
}

void Shader::localParameterVertex(unsigned int index,FloatVector3d value){

	if(hardware.vertexProgram){
		glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, index, value.x,  value.y,  value.z,0);
	}
}


void Shader::localParameterVertex(unsigned int index,float value){

	if(hardware.vertexProgram){
		glProgramLocalParameter4fARB(GL_VERTEX_PROGRAM_ARB, index, value, value, value, value );
	}
}


void Shader::localParameterVertex(unsigned int index,double* value){
	if(hardware.vertexProgram){
		glProgramLocalParameter4dvARB(GL_VERTEX_PROGRAM_ARB, index, value);
	}
}


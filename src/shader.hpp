#ifndef SHADER_H
#define SHADER_H

#include "Containers.hpp"

#include "HelperLibString.hpp"

typedef int ShaderId;
typedef int ShaderPackId;

#define SHADER_NONE   (-1)
#define LANG_ASM 1
#define LANG_GLSL 2

#include "Gl/Extensions.h"

#include "GlHeader.hpp"

#include "os.h"

/*
names:
fp=fragment program
vp=vertex program
glf=glsl fragment program
glv=glsl vertex program
*/

class ShaderData{
	public:

	unsigned int id;

	char *fragmentSource;
	char *vertexSource;

	int fragmentLength;
	int vertexLength;
	GLenum type;
	Path file;
	unsigned int lang;

	GLhandleARB program;
	GLhandleARB vertexShader;
	GLhandleARB fragmentShader;

	ShaderData();
};

class ShaderPack{
	public:
	String name;
	String file;
	Array<ShaderData> shaders;
};


class Shader {
	private:
		Array<ShaderPack> packs;
		Array<ShaderData> shaders;

		ShaderId currentShaderId;
		ShaderPackId currentShaderPack;
		bool reloadFragmentFile(Path program);
		bool reloadVertexFile(Path program);

	public:
	
	
	bool uploadShader(ShaderData &fragmentShader);
	void createShader(ShaderData &fragmentShader);

	ShaderId addVertexFile(Path program, String md5="");
	ShaderId addVertexFile(Path program,ShaderId link, String md5="");

	ShaderId addFragmentFile(Path program, String md5="");
	ShaderId addFragment(String code);
	
	bool scriptInit();
	bool reload(String program);

	void set(ShaderPackId packId, ShaderId shader);
	void set( ShaderId shader);

	void localParameterFragment(unsigned int index,FloatVector4d value);	//index starts at 0
	void localParameterFragment(unsigned int index,FloatVector3d value);	//index starts at 0
	void localParameterFragment(unsigned int index,float value);			//index starts at 0
	void localParameterFragment(unsigned int index,double* value);
	void localParameterFragment(unsigned int index,double x, double y, double z, double w);
	void localParameterVertex(unsigned int index,FloatVector4d value);	//index starts at 0
	void localParameterVertex(unsigned int index,FloatVector3d value);	//index starts at 0
	void localParameterVertex(unsigned int index,float value);			//index starts at 0
	void localParameterVertex(unsigned int index,double* value);
	void localParameterVertex(unsigned int index,double x, double y, double z, double w);

	bool reset();
	
	Shader();
};

extern Shader shader;

#endif

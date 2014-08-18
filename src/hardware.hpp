#ifndef HARDWARE_H
#define HARDWARE_H

#include "os.h"
#include "script.hpp"


class Compatibility{
	public:
		bool slowTextureLevel;
		bool shaders;
		String mixer1_hardware_vp;
		String mixer2_hardware_vp;
		String mixer3_hardware_vp;
		String mixer1_fp;
		String mixer2_fp;
		String mixer3_fp;
};

class Hardware{
	private:
		bool GetExtensionsString();
		bool isExtensionSupported(char *extension);
		char* extensions;
		LuaType* luaHardware;

	public:
		Compatibility compatibility;

		bool fragmentProgram;
		bool vertexProgram;
		bool glsl;
		bool audio;
		bool vbo;

		String vendor;

		void minimumDebug();

		double textureMemoryUsed;		//in bytes
		ScreenMode validateScreenMode(int sizex, int sizey, int bitDepth);
		Hardware();
		bool init();
		bool scriptInit();
		bool earlyScriptInit();
};

extern Hardware hardware;

#endif

#ifndef VIDEOPANEL_H
#define VIDEOPANEL_H

#include "Types.hpp"
#include "HelperLibString.hpp"
#include "Helperlib/Containers.hpp"
#include "script.hpp"
#include "shader.hpp"
#include "video/Video.h"

/*class Thumb{
	public:
		FloatVector2d pos;
		FloatVector2d scale;
		int thumbTex;
		String path;
};*/

class VideoPanel{
	private:
		//Array<Thumb> thumbs;
		//Map<String,int> remap;

		Map<String,VideoSource*> remap;
		Map<String,unsigned int> remapPictures;
		Map<String,ShaderId> remapShaders;

		Array<String> layers;
		Array<Bool> layersAutoplay;
		Array<float> layersFrame;
		Array<float> layersSpeed;	//1==normal, 2==double, etc

		Array<FloatVector4d> shaderParameters;

		ShaderId shaderId;
		//void drawOLD();


	public:
		bool mouseover;

		bool visible;
		//String mouseoverThumbnail;
		//int mouseoverThumbnailInt;
		//void setMouseover(int i);

		String name;

		FloatVector3d pos;
		FloatVector2d scale;
		FloatVector3d rot;
		FloatVector4d color;

		float thumbnailSize;

		bool percent;

		bool load(int layer, String source);
		void autoplay(int layer, bool autoplay);
		void frame(int layer, float);
		float frame(int layer);
		void fps(int layer, float);
		float fps(int layer);

			
		bool loadShader(String code);
		bool shaderParameter(int id,FloatVector4d value);
		//float shaderParameter(int id);
		//bool shaderParameter(int id,float value);
		FloatVector4d shaderParameter(int id);	//create a new parameter

		//FloatVector2d thumbScale(String);

		//bool add(String name);
		void draw();
		VideoPanel();
};

int luaVideoPanelLoad(lua_State *L);
int luaVideoPanelShader(lua_State *L);
int luaVideoPanelShaderParameter (lua_State *L);
int luaVideoPanelAutoplay(lua_State *L);
int luaVideoPanelFrame(lua_State *L);
int luaVideoPanelFps(lua_State *L);
//int luaThumbnailsAdd(lua_State *L);
//int luaThumbnailsThumbPos(lua_State *L);
//int luaThumbnailsThumbScale(lua_State *L);

#endif

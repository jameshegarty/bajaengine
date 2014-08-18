#ifndef PANEL_H
#define PANEL_H

#include "HelperLibString.hpp"
#include "script.hpp"

#define LAZY_PANEL_LOAD 1 //only load panels when they actually become visible

#include "GlHeader.hpp"

/*lua:

:file(filename)
:texture()
color
pos				//z=layer/top/bottom/etc
scale
*/

int luaPanelLoad(lua_State *L);

int luaPanelLoadMouseover(lua_State *L);

int luaPanelEnableMouseover(lua_State *L);
int luaPanelDisableMouseover(lua_State *L);
int luaPanelScissor(lua_State *L);
int luaPanelScissorOff(lua_State *L);
class Panel{
	private:
		bool forced;

		unsigned int textureId;
		bool assigned;

		unsigned int textureIdMouseover;
		bool assignedMouseover;
		float currentMouseoverFade;
		bool doMouseover;

		FloatVector4d scissorStore;
		bool scissorSet;

		#ifdef LAZY_PANEL_LOAD
			bool loaded;
			bool loadedMouseover;
		#endif

		Path filename;
		Path filenameMouseover;
	public:

		FloatVector3d pickerPos;
		FloatVector2d pickerScale;

		bool mouseover;
		float mouseoverFade;

		bool visible;
		bool clickable;
		bool center;
		float smartCenter;

		bool percent;
		bool keepAspectRatio;
		
		FloatVector2d texsize;

		void enableMouseover();
		void disableMouseover();

		void (*onMouseLDown)();
		void (*onMouseLUp)();

		String name;
		FloatVector4d color;
		FloatVector3d pos;		//0,0 = top left 640,-480=bottom right
		FloatVector2d scale;	//in pixels (autoset on load)
		FloatVector3d rot;
		FloatVector2d pivot;	//the center, used for rotating, position, scaling, etc - based on percentage of panel, ie (0.5,-0.5) for the middle o' the image, (0,0) for the top left

		void forceTexture(unsigned int texid);	//force the panel to use gl texture with id 'texid'
		bool load(Path file, int type);	//type, 0=normal, 1=
		void draw();
		void scissor(float minX, float maxX, float minY, float maxY);
		void scissorOff();

		Panel();
		~Panel();
};


#endif


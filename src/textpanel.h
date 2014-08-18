#ifndef TEXTPANEL_H
#define TEXTPANEL_H

#include "HelperLibString.hpp"
#include "script.hpp"
#include "Text.hpp"

/*

font.load(name,file)
font.default(name)

textpanel.text="blah"
textpanel:font("fontname")
textpanel.pos

*/

int luaTextPanelFont(lua_State *L);
int luaTextPanelText(lua_State *L);
int luaTextPanelScissor(lua_State *L);

//int luaTextpanelLoadMouseover(lua_State *L);


class TextPanel{
	private:
		FloatVector2d pixelsize;
		float oldFontSpacing;	//we need to adjust our ratio when our font changes spacing
		float oldFontLeading;

		FloatVector4d scissorStore;
		bool scissorSet;
	public:
		//bool mouseover;
		//float mouseoverFade;

		FloatVector3d pickerPos;
		FloatVector2d pickerScale;

		bool clickable;

		bool setFont(String);
		bool setText(String);

		float smartCenter;

		void (*onMouseLDown)();
		void (*onMouseLUp)();

		bool mouseover;

		bool visible;
		bool center;
		bool percent;
		bool keepAspectRatio;
		
		void scissor(float minX, float maxX, float minY, float maxY);

		FloatVector2d textscale;

		String storedtext;
		String name;
		String align;
		FontId font;

		FloatVector4d color;
		FloatVector3d pos;	//0,0 = top left 640,-480=bottom right
		FloatVector2d scale;	//in pixels (autoset on load)
		void draw();
		TextPanel();
};


#endif


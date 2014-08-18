#ifndef TEXT_H
#define TEXT_H

#include "Containers.hpp"

#include "GlHeader.hpp"

#include <string>
#include <vector>

#ifdef MANSION_FREETYPE
	#include <ft2build.h>	//<ft2build.h>
	#include FT_FREETYPE_H
	#include FT_GLYPH_H  
#endif

#include "HelperLibString.hpp"
#include "script.hpp"

typedef int FontId;

enum TextPos{
	TEXT_POS_TL,
 	TEXT_POS_TR,
 	TEXT_POS_CENTER,
 	TEXT_POS_BL,
 	TEXT_POS_BR,
 	TEXT_POS_CL,
 	TEXT_POS_CR,
};

class Glyph{
	public:
		FloatVector2d size;
		FloatVector2d offset;
		IntVector2d bitmapSize;
		FloatVector2d textureSize;
		FloatVector2d textureSize1;
  
   		unsigned char* data;
};

enum FontType{
	FONT_TYPE_FREETYPE,
	FONT_TYPE_BITMAP,

};
class Font{
	friend class Text;
	friend class Renderer;
	
	private:
		Glyph glyphs[128];	//holds data about glyphs
		unsigned int texture;
		bool textureSet;
		
		float h;										// Holds The Height Of The Font.
		unsigned int listBase;									// Holds The First Display List Id
		unsigned int lastSize;
		float lastSpacing;
		Path lastFile;
		bool lastTb;
		bool lastAntialiased;
		
		bool remakeSpacing();
		#ifdef MANSION_FREETYPE
			bool remakeFontFreetype();
		#endif

		bool remakeFontBitmap();
		bool remakeFont();
		void clean();
		bool init();

		#ifdef MANSION_FREETYPE
			void make_dlist ( FT_Face face, char ch);
		#endif

		int maxX;	//max x+y of a glyph
		int maxY;

		FontType type;

		FloatVector2d getSizeBase(String input,bool displacement);

	public:
		FloatVector4d color;
		FloatVector2d rescaleFactor;	//setting x to 0.5 would make it draw half as large in x
		unsigned int size;
		float leading;	//a percentage of "h" 1= default, 2= double
		float spacing;	//space between letters (percentage, like leading)
		Path file;
		std::string name; //our name
		float waste;
		bool antialiased;
		
		void print(float x, float y, String text);
		void print(float x, float y, float z, String text);
		FloatVector2d getSize(String input);			//get the TOTAL SIZE of all pixels drawn on screen
		FloatVector2d getDisplacement(String input);	//get the size from the origin (x,y pos where the text is drawn from).  smaller than getSize
		FloatVector2d getSizeSmart(String input);		//get size with std. leading
		
		bool make();  //remakes the font.  you need to have the variables set
		
		bool tb;
		bool tb2;
		
		Font();
};

class Text{
	private:
		
		Path defaultFont;
		
		Font* getFont(FontId id);	//this shoulden't be public, really
		LuaType* fontObj;
		
	public:
		FontId luaDefaultFont;

		SafeArray<Font> fonts;
		Map<String,FontId> luaFonts;

		void uploadFont(FontId id);

		bool init();

		void draw2dRescale(float x, float y, String text,FloatVector2d rescale);  //assumes TEXT_POS_TL
		void draw2dRescale(FontId id, float x, float y, String text,FloatVector2d rescale);  //assumes TEXT_POS_TL
		void draw2dRescale(FontId id, float x, float y, float z, String text,FloatVector2d rescale);  //assumes TEXT_POS_TL

		void draw2d(float x, float y, String text);  //assumes TEXT_POS_TL

		void draw2d(FontId id, float x, float y, float z, String text);  //assumes TEXT_POS_TL
		void draw2d(FontId id, TextPos pos, float x, float y, String text);	//rewrite this so the "pos" is "flags" - a 32 bit flag
		void draw2d(FontId id, float x, float y, String text);  //assumes TEXT_POS_TL
  			//sets pos relative to given pos:
  			//	|TL			   TR|
  			//	|CL  CENTER	   CR|
  			//	|BL		       BR|
		
		int leading(FontId id=-1);				//returns h*leading

		bool setDefaultFont(String font);	//set default font
		
		FontId addFont(Path fontfile);
		FontId getLuaDefaultFont();
		bool setLuaDefaultFont(FontId id);

		bool scriptInit();
		FloatVector2d getSize(FontId id, String text);
		FloatVector2d getSizeSmart(FontId id, String text);
		FloatVector2d getDisplacement(FontId id, String text);
		Text();
		~Text();
};

int luaFontLoad(lua_State *L);
int luaFontDefault(lua_State *L);

extern Text text;

#endif

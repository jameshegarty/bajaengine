#ifndef EDITOR_TEXTBOX_H
#define EDITOR_TEXTBOX_H

#include "editor.h"
#include "helperLibstring.hpp"
#include "text.hpp"

class EditorTextbox{
	private:
		int xCursorPos;
		ObjectAddress textpanel;
		ObjectAddress cursor;
		int cursorPos;	//0 indicates that it is BEFORE the first character, 1 after
	public:
		FloatVector3d pos;
		FloatVector4d color;

		FontId font;

		String* string;
		void init();
		bool visible;
		void perframe();
};


#endif
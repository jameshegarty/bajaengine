#ifndef EDITOR_CHECKBOX_H
#define EDITOR_CHECKBOX_H

#include "editor.h"
#include "helperLibstring.hpp"

class EditorCheckbox{
	private:
	
		ObjectAddress text;
		ObjectAddress bkg;
		ObjectAddress bkgTop;
		ObjectAddress checked;
		
	public:
		FloatVector3d pos;
		FloatVector4d color;

		//void setText(String);
		//String getText();
		String* string;
		void init();
		bool visible;
		void perframe();
};


#endif
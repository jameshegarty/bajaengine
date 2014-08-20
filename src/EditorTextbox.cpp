#ifdef DEVELOPER

#include "EditorTextbox.h"
#include "keyboard.h"
#include "HelperLibMath.hpp"
#include "timeing.hpp"
#include "text.hpp"

void EditorTextbox::perframe(){
	textpanel.textpanel->pos.x=pos.x;
	textpanel.textpanel->pos.y=pos.y;
	textpanel.textpanel->pos.z=pos.z;

	textpanel.textpanel->color=color;

	textpanel.textpanel->visible=visible;

	textpanel.textpanel->font=font;

	cursor.panel->pos.z=pos.z;
	cursor.panel->visible=visible;
	static float fc=0;
	fc+=engineTime.time*10;
	cursor.panel->color.w=math.sin(fc);

	if(string!=NULL){

		String cursorLine="";
		FloatVector2d cursorLineSize;
		int cursorLineNumber=0;

		if(cursorPos>string->size()){cursorPos=string->size();}

		for(int i=0; i<cursorPos; i++){
			if((*string)[i]=='\n'){
				cursorLine="";
				cursorLineNumber++;
			}
			cursorLine+=(*string)[i];
		}

		cursorLineSize=text.getSize(font,cursorLine);
		cursor.panel->pos.x=cursorLineSize.x+pos.x;
		cursor.panel->pos.y=pos.y-text.leading()*cursorLineNumber;
		
		*string=string->substrIndex(0,cursorPos)+keyboard.currentStringDown+string->substrIndex(cursorPos,string->size());
		if(keyboard.currentStringDown!=""){cursorPos++;}

		if(keyboard.backspaceDown){
			if(cursorPos>0){
				*string=string->substrIndex(0,cursorPos-1)+string->substrIndex(cursorPos,string->size());
				cursorPos--;
				xCursorPos=-1;
			}
		}
		
		if(keyboard.deleteKeyDown){
			
			*string=string->substrIndex(0,cursorPos)+string->substrIndex(cursorPos+1,string->size());
			xCursorPos=-1;
		}

		if(keyboard.enter_down){
			*string=string->substrIndex(0,cursorPos)+"\n"+string->substrIndex(cursorPos,string->size());
			xCursorPos=-1;
			cursorPos++;
		}

		if(keyboard.arrow_right_down){
			cursorPos++;
			cursor.panel->color.w=1;
			xCursorPos=-1;
		}

		if(keyboard.arrow_left_down){
			cursorPos--;
			cursor.panel->color.w=1;
			if(cursorPos<0){cursorPos=0;}
			xCursorPos=-1;
		}

		if(keyboard.arrow_up_down){
			if(xCursorPos==-1){xCursorPos=cursorLine.size();}
			
			int ncp=-1;
			for(int i=cursorPos-1; i>=0; i--){
				if((*string)[i]=='\n' && ncp==-1){
					ncp=i;
				}else if((*string)[i]=='\n' || i==0){
					cursorPos=i+xCursorPos;
					if(cursorPos>ncp){cursorPos=ncp;}
					break;
				}
			}
		}

		if(keyboard.arrow_down_down){
			if(xCursorPos==-1){xCursorPos=cursorLine.size();}
			
			int ncp=-1;
			for(int i=cursorPos; i<string->size(); i++){
				if(ncp==xCursorPos){cursorPos=i-1;break;}

				if(ncp!=-1){ncp++;}

				if((*string)[i]=='\n'){
					if(ncp>0){
						cursorPos=i;
						break;
					}
					ncp=0;
				}
			}

		}

		textpanel.textpanel->setText(*string);
	}
}

void EditorTextbox::init(){
	textpanel=editor.add("textpanel","textpanel");
	textpanel.textpanel->pos.x=0;
	textpanel.textpanel->pos.y=0;
	textpanel.textpanel->pos.z=91;
	textpanel.textpanel->color.x=1;
	textpanel.textpanel->color.y=0;
	textpanel.textpanel->color.z=0;
	textpanel.textpanel->color.w=1;
	textpanel.textpanel->percent=false;
	textpanel.textpanel->visible=false;

	cursor=editor.add("cursor","panel");
	cursor.panel->pos.x=0;
	cursor.panel->pos.y=0;
	cursor.panel->pos.z=91;
	cursor.panel->scale.x=2;
	cursor.panel->scale.y=text.leading();
	cursor.panel->color.x=1;
	cursor.panel->color.y=1;
	cursor.panel->color.z=1;
	cursor.panel->color.w=1;
	cursor.panel->percent=false;
	cursor.panel->visible=false;

	string=NULL;

	xCursorPos=-1;
	cursorPos=0;
}

#endif

#ifndef PICKING_H
#define PICKING_H

//#define LEGACY_PICKER 1

#ifdef LEGACY_PICKER

#include "Containers.hpp"

#ifdef _WIN32
	#include <gl/gl.h>
#elif LINUX
	#include <gl.h>
#else
	#include <GLUT/glut.h>
#endif

enum{	//top level picker stuff
	
	//filter types
	
	PICKER_DEPTH,
	PICKER_INDEX_ASC,	//choose one with lowest index
	PICKER_INDEX_DESC,	//choose one with highest index
	PICKER_INDEX2_DESC,	//liked PICKER_INDEX_DESC, but only compares value of second index, ie [1]
	PICKER_DETAIL
};

/*

engine.picker.draw="nonstatic" --static,none
engine.picker.on=true
engine.picker.sort
engine.picker.pos.x
engine.picker.pos.y
engine.picker.scale.x
engine.picker.scale.y
engine.picker.clip.near
engine.picker.clip.far
*/

/*
FORMAT:

level1) the first level is to top usage, such "PICKER_GUI" or "PICKER_OBJECT"

for 2d:
level2) the depth level, first is 0, second is 1, etc
level3) object id
*/

#define PICKER_GUI 1
#define PICKER_OBJECT 2	//objects

//can't call it "Picker" because that name is already used on the mac

class GlPicker{
	private:
		
		Array<int> pos;  //stores max ids, for use in some functions
		int posLevel;
		GLuint	buffer[512];
		float tf;
		float tn;

	public:
		bool debug;
		int sort;
		bool active;
		
		Array<int> result;
		
		String perspective;	//ortho

		//window size  tl=0,0 br=1024,-768
		int sizeX;
		int sizeY;
		int posX;
		int posY;

		float clipNear;
		float clipFar;

		bool start();	//start the picking - sets up camera automatically based on level->currentCamera.  you don't need to call resadjust
		bool stop();
		bool clear();  //clear the stack
		
		bool pushId(int i);
		bool changeId(int id);
		bool pop();
		
		int push(); //adds to top, returns id
		int change();
		
		
		
		GlPicker();
};

extern GlPicker glPicker;

/*
class PickViewer : public Box{
	public:
		void extraDraw();
		PickViewer();
};

class PickerWindow : public GuiWindow{
	protected:
    
    
    public:
    	void init();
		PickerWindow();
    
};

class PickerWindowMaker : public ObjectMaker {
	private:
		PickerWindowMaker() : ObjectMaker("PickerWindow") {}
    	static const PickerWindowMaker registerThis;

		void type(ObjectType* type){

		}
		
		Object* make(){ 
			return new PickerWindow(); 
		}
};*/
#endif

#endif

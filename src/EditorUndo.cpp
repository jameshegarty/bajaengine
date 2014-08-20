#ifdef DEVELOPER

#include "EditorUndo.h"
#include "Script.hpp"
#include "Editor.h"

EditorUndo::EditorUndo(){
	undoPos=-1;
	redoPos=0;
	storeActive=false;
	redoMode=false;
}

bool EditorUndo::add(String undo,String redo){
	
	if(redoMode){
		//this is being called INSIDE a redo transation, so we don't want to add it
		return false;
	}

	if(undoPos<undoStack.size()-1){
		while(undoPos<undoStack.size()-1){
			redoStack.erase(redoStack.size()-1);
			undoStack.erase(undoStack.size()-1);
		}
	}

	undoPos++;
	redoPos++;
	

	undoStack.pushBack(undo);
	redoStack.pushBack(redo);

	return true;
}

bool EditorUndo::undo(){

	if(undoPos==-1){

		editor.editorError("Coulden't undo, no actions have transpired");
		return false;
	}

	script().run(undoStack[undoPos]);
	undoPos--;
	redoPos--;

	return true;
}


bool EditorUndo::redo(){

	if(redoPos==-1){

		editor.editorWarning("Coulden't redo, no actions have transpired");
		return false;
	}


	if(redoPos>=redoStack.size()){
		editor.editorError("Coulden't redo, no undos have occured");
		return false;
	}

	redoMode=true;
	script().run(redoStack[redoPos]);
	redoMode=false;

	redoPos++;
	undoPos++;

	return true;
}

#endif
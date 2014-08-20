#ifndef EDITOR_UNDO_H
#define EDITOR_UNDO_H 1

#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"

class EditorUndo{
	private:
		Array<String> undoStack;
		Array<String> redoStack;
		int undoPos;
		int redoPos;
		bool redoMode;

	public:
		bool storeActive;
		Array<FloatVector3d> storeFloatVector3d;

		bool add(String undo, String redo);
		bool undo();
		bool redo();

		EditorUndo();
};

#endif
#ifndef PICKER_MATRIX_H
#define PICKER_MATRIX_H

#include "Helperlib/Containers.hpp"
#include "objects.hpp"

class GlPickerMatrix{
	private:
		Array<Object*> objects;
		Array<int> ids;
	public:
		int posX;
		int posY;

		void clear();
		void add(int id,Object*);
		Array<int> pick();
};

extern GlPickerMatrix pickerMatrix;

#endif
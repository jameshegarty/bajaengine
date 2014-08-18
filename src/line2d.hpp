#ifndef LINE2D_H
#define LINE2D_H

#include "script.hpp"

class Line2d{
	private:
	
	public:
		FloatVector3d pos;
		FloatVector3d scale;
		FloatVector4d color;
		float thickness;
		bool visible;
		bool percent;
		bool keepAspectRatio;

		String name;
		
		Line2d();
		void draw();
};

#endif


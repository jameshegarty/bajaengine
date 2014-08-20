#ifndef ANIMATION_H
#define ANIMATION_H

#include "Helperlib/Containers.hpp"
#include "HelperLibString.hpp"

class ObjectKey{
	public:
		float frame;
		float value;
		float leftTanX;
		float leftTanY;
		float rightTanX;
		float rightTanY;
};

class ObjectAnimation{
	private:
		float oldFrame;
		float oldBlend;

	public:
		unsigned int type;	//0=constant, 1=linear, 2=cubic, 3=hermite
		Array<ObjectKey> keys;
		String variable;
		String name;	//the name of the animation, ie 'walk','run'.  Default: 'default'
		bool write(std::ofstream& out);
		unsigned int size();

		float start;	//start frame, for looping
		float end;	//end frame, for looping
		float frame;
		float fps;
		float blend;
		bool loop;
		bool play;
		bool dirty;
		void perframe();
		ObjectAnimation();
};

class Animation{
	private:
		void setValue(Object* obj, float value, String variable);
	public:
		void perframe();
		bool scriptInit();
};

extern Animation animation;

#endif


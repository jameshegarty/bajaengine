#ifndef MOUSE_H
#define MOUSE_H

#include "GlHeader.hpp"

#include "Types.hpp"

class Mouse{
    private:
		#ifdef _WIN32
		POINT          mouse_pos;
		#endif
		bool		   l_down_wait;
		bool		   l_up_wait;
		bool 		   r_down_wait;
   		bool           r_up_wait;
   		bool 		   m_down_wait;
   		bool           m_up_wait;
	   	
		#ifdef _WIN32
		DWORD dwFlags;
		DWORD dx;
		DWORD dy;
		DWORD dwData;
		ULONG_PTR dwExtraInfo;
		#endif
		
		unsigned int	lDoubleClickUpWait;
		float wait;
		bool lastVirtualMouse;
	
    public:
		bool virtualMouse;	//if true, it's drawn in gl, if false, it's the OS mouse
    	bool           l;
		bool           r;
		bool		   m;
    	bool messageLDown;
    	bool messageLUp;
		bool messageLDouble;
    	bool messageRDown;
    	bool messageRUp;
    	bool messageMDown;
    	bool messageMUp;
    	bool messageWheelUp;
		bool messageWheelDown;
		float messageX;
		float messageY;
		
		bool visible;

        GLfloat        x;                               //Mouse pos y
        GLfloat        y;                               //Mouse pos y
		float percentX;
		float percentY;
        GLfloat        x_from_center;                   //Mouse pos x with center as point 0
        GLfloat        y_from_center;                   //Mouse pos x with center as point 0
        GLfloat        change_x;
        GLfloat        change_y;
        float percent_change_x;		//percent of screen
		float percent_change_y;

        bool left();
		bool right();
  		bool middle();
           
        bool		   lDown;
        bool		   lUp;
        bool           rDown;
        bool           rUp;
        bool           mDown;
        bool           mUp;
        bool lDouble;

		bool wheelUp;
		bool wheelDown;

        void           update();
        void		   show();
        void 		   hide();		

		int textureId;	//-1 if none
		FloatVector2d textureScale;
		FloatVector4d color;

		bool scriptInit();

        Mouse();
};

extern Mouse mouse;

#endif

#ifndef GL_HEADER_HPP
#define GL_HEADER_HPP

#ifdef _WIN32
	#include <windows.h> 
	#include <gl/gl.h>
	#include <gl/glu.h>	
	#include "gl/extensions.h"	
#elif LINUX
	#include <gl.h>
	#include <glu.h>	
#else
	#include <GLUT/glut.h>
#endif

#endif

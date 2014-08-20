/* Os.h
 * This is a base class for platform specific stuff
 * There is a derived class for each supported OS
 * this is for connecting with stuff in the os, like windows, searching, etc
 */

#ifndef OS_H
#define OS_H

#include <string>
#include "Helperlib/Containers.hpp"
#include "Types.hpp"
#include "HelperLibString.hpp"


class Window{	//a rendering window
	public:
		unsigned int depth;		//color depth
		unsigned int zDepth;	//zbuffer depth
		bool fullscreen;  //change this to change to windowed
        bool multisample;
		bool contextReady; //false until context is ready for drawing
        IntVector2d size;  //size is positive (this is not screen space) - should be unsigned, but this causes problems where you need a (-windowSize.x) or whatever
		IntVector2d pos;	//offset from top of screen to drawable area, in our screen space format.  w goes from 0 to screen.x, h goes from 0 to -screen.y
		bool active;	//is the game window the current front/active window?
        Window();
};

class ScreenMode{
public:
	unsigned int x;
	unsigned int y;
	unsigned int bitDepth;
	unsigned int refreshRate;
};

enum PathFormat{
	PATH_RELATIVE,
	PATH_ABSOLUTE
};

class Path{
	private:
		String path;
		PathFormat format;
	public:
		bool set();	//has the path been set?
		String getExtension();						//NO DOT
		String getRelativeTo(String directory);		//directory should have NO TRAILING SLASH
		String getRelativeDirectory();						//NO TRAILING SLASH
		String getFilename();	//get just the filename, with extension
		String getFilenameNoExtension();
		String getRelative();
		String getRelativeNoExtension();
		String getAbsolute();
		void setRelative(String relativePath);
		void setAbsolute(String relativePath);
		bool operator!=(const Path& right);
		bool operator==(const Path& right);
		friend int operator<(const Path& left, const Path& right);
		Path(String);	//sets RELATIVE, not ABSOLUTE
		Path(const char* sr);
		Path();
		void clear();
};
	
class Os{
	private:
		
	
	public:
		std::string executableFile;     	//the path to the current running executable (aka, this program), includes exe name
        std::string executableDirectory;        //directory the exe is in.  INCLUDES A TRAILING SLASH
        std::string workingDirectory;           //root directory for file access.  INCLUDES TRAILING SLASH		

		Window* window;	//the current window we're drawing to
	    Array<ScreenMode> screenModes();
		bool init();
		bool perFrame();
		bool killWindow();
		bool createWindow(char* title);
		bool error(String error);
	    bool yesNo(std::string question);
		Array<Path> folderContents(Path);
		bool folderExists(Path);
		bool fileExists(Path);
		bool mkdir(Path dir);
		bool copy(Path sourcefile, Path destfile);
		std::string execute(std::string program,std::string arguments, std::string workingDirectory="");	//returns "" if successful, else returns an error
		bool hideMouse();
	    bool showMouse();
		unsigned int time();
	    float doubleClickTime();
		Os();
};

#ifdef _WIN32
	#include "WindowsOs.h"
#elif LINUX
	#include "LinuxOs.hpp"
#else
	#include "mac/MacOs.h"
#endif


bool osScriptInit();


#endif

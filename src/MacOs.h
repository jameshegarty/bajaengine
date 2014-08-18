#ifndef MAC_OS_H
#define MAC_OS_H

#include <Carbon/Carbon.h>
#include "Os.h"

#include <string>
#include "Containers.hpp"
#include <AGL/agl.h>

extern Map<String,CFDictionaryRef> macModes;
extern CFDictionaryRef macStartMode;
		
class MacSystem : public Os {
	private:
		
	public:
		std::string macPath;
		bool macPathStored;	
		
		AGLContext aglContext;
		Array<ScreenMode> screenModes();    
		bool init();
		bool perFrame();
		bool killWindow();
		bool createWindow(char* title);
		bool error(std::string error);
		Array<Path> folderContents(Path);
		bool folderExists(Path);
		bool fileExists(Path);
		bool mkdir(Path);
		MacSystem();
};

MacSystem& os();


#endif

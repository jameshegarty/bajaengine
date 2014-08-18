#include "MacOs.h"
#include <Carbon/Carbon.h>
#include <AGL/AGL.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#include <OpenGL/OpenGL.h>
#include "Engine.h"
#include <sys/stat.h>

#include "log.hpp"
#include "HelperLibString.hpp"
#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFDictionary.h>

extern "C"{
#include <GLUT/glut.h>
}

Map<String,CFDictionaryRef> macModes;
CFDictionaryRef macStartMode;

MacSystem& os(){
    static MacSystem o;
    return o;
}

MacSystem::MacSystem(){
	macStartMode=NULL;
	macPathStored=false;
}

bool MacSystem::init(){
	return true;
}

bool MacSystem::perFrame(){
	return true;
}

bool MacSystem::killWindow(){
	return true;
}

bool MacSystem::createWindow(char* title){
	return true;
}

bool MacSystem::error(std::string error){
	logs().main.write("ERROR:"+error);
	
	String e=error;
	SInt16 out;
	
	//printf(error.c_str());
	
	/*NSAlert *alert = [[NSAlert alloc] init];
	 [alert setAlertStyle:NSInformationalAlertStyle];
	 [alert setMessageText:NSLocalizedString(@"Menubar Countdown Complete", @"Expiration message")];
	 [alert setInformativeText:NSLocalizedString(@"The countdown timer has reached 00:00:00.",
	 @"Expiration information")];
	 [alert addButtonWithTitle:NSLocalizedString(@"OK", @"OK button title")];
	 [alert addButtonWithTitle:NSLocalizedString(@"Restart Countdown...", @"Restart button title")];
	 
	 NSInteger clickedButton = [alert runModal];
	 [alert release];*/ 
	
	
	
//	OSErr err=StandardAlert(kAlertStopAlert,e.pascalStr (),NULL,NULL,&out);
	
	/*
	 //	OSErr err=StandardAlert(kAlertStopAlert,e.pascalStr (),NULL,NULL,&out);
	 NSAlert* a = [NSAlert alertWithMessageText:@"hello" defaultButton:nil alternateButton:nil 
	 otherButton:nil informativeTextWithFormat:@""];
	 
	 [a runModal];
	 */
	
	
	//  Alert(128,NULL);
	return true;
}

Array<Path> MacSystem::folderContents(Path i){
    Array<Path> temp;
	
    return temp;
}

bool MacSystem::folderExists(Path f){
	
	struct stat buf;
	int i = stat ( f.getAbsolute().c_str(), &buf );
	if ( i == 0 ){
		return true;
	}
	
	return false;
}


bool MacSystem::fileExists(Path f){
	
	
    struct stat buf;
	int i = stat ( f.getAbsolute().c_str(), &buf );
	if ( i == 0 )
	{
		return true;
	}
	
	return false;
}

bool MacSystem::mkdir(Path dir){
	String dire="mkdir \""+dir.getAbsolute()+"\"";
	
	system(dire.c_str());
	
	return true;
}

Array<ScreenMode> MacSystem::screenModes(){
	CGDirectDisplayID main=CGMainDisplayID();
	
	CFArrayRef modes=CGDisplayAvailableModes(main);
	
	Array<ScreenMode> fm;
	for(int i=0; i<CFArrayGetCount(modes); i++){
		CFDictionaryRef d=(CFDictionaryRef)CFArrayGetValueAtIndex(modes,i);
		CFNumberRef cw=(CFNumberRef)CFDictionaryGetValue(d,kCGDisplayWidth);
		CFNumberRef ch=(CFNumberRef)CFDictionaryGetValue(d,kCGDisplayHeight);
		CFNumberRef cbit=(CFNumberRef)CFDictionaryGetValue(d,kCGDisplayBitsPerPixel);
		CFNumberRef cr=(CFNumberRef)CFDictionaryGetValue(d,kCGDisplayRefreshRate);
		
		int rx,ry,rbit,rr;
		CFNumberGetValue(cw,kCFNumberIntType,&rx);
		CFNumberGetValue(ch,kCFNumberIntType,&ry);
		CFNumberGetValue(cbit,kCFNumberIntType,&rbit);
		CFNumberGetValue(cr,kCFNumberIntType,&rr);
		
		if(rbit>=16 && rx>=800 && ry>=600){
			macModes[String(rx)+"X"+String(ry)+"X"+String(rbit)]=d;
			
			ScreenMode st;
			st.x=rx;
			st.y=ry;
			st.bitDepth=rbit;
			st.refreshRate=rr;
			fm.pushBack(st);
		}
	}
	
	Array<ScreenMode> fmf;
	int cup=0;
	for(int i=fm.size()-1; i>=0; i--){
		
		fmf.pushBack(fm[i]);
		cup++;
	}
	
	for(int i=0; i<fmf.size()-1; i++){
		ScreenMode t=fmf[i];
		if(fmf[i+1].bitDepth<fmf[i].bitDepth && fmf[i+1].x==fmf[i].x  && fmf[i+1].y==fmf[i].y){
			fmf[i]=fmf[i+1];
			fmf[i+1]=t;
		}
	}
	
	return fmf;
}


String Path::getRelative(){
	if(format==PATH_RELATIVE){
		return path;
	}
	
	return "";
}

String Path::getAbsolute(){
	if(format==PATH_ABSOLUTE){
		return path;
	}else{
		
		if(!os().macPathStored){
			CFBundleRef main=CFBundleGetMainBundle ();
			CFURLRef x=CFBundleCopyResourcesDirectoryURL(main);
			
			CFStringRef r;
			
			x=CFURLCopyAbsoluteURL(x);
			r=CFURLCopyFileSystemPath(x,kCFURLPOSIXPathStyle);
			
			char pbuff[255];
			CFStringGetCString(r,pbuff,255,kCFStringEncodingASCII);
			os().macPath=String(pbuff);		
			os().macPath+="/";
			
			os().macPathStored=true;
		}
		
		String fin=os().macPath+path;
		
		if(path[0]=='~'){
			NSString* ins = [NSString stringWithUTF8String:path.cStr()];
			NSString* res = [ins stringByExpandingTildeInPath];
			fin = String([res cString]);
		}
		
#if defined(_DEBUG)
		
		//working directory actually valid here
		//no, actually wildly inconsistant
		//return path;
		
		return String("/Users/jsh/Desktop/testdir/")+path;
#endif
		
		return fin;
		
	}
	
}

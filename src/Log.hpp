#ifndef HELPERLIB_LOG_H
#define HELPERLIB_LOG_H

#include <string>
#include "HelperLibString.hpp"
#include "StringFunctions.hpp"
#include "Containers.hpp"
#include "Types.hpp"
#include "os.h"

//http://www.cplusplus.com/doc/tutorial/tut4-2.html

#ifdef _WIN32

#else
	#define LOG_KEEP_FILES_OPEN 1
#endif

class LogFileStop{
	public:
};
class LogFile{
	friend class LogViewer;
	friend class LogViewerWindow;
	friend class ClearButton;
	
	private:

		Array<String> data;
	
	protected:
		Path fileName;
		
		#ifdef LOG_KEEP_FILES_OPEN
			std::ofstream out;
		#endif
		
	public:
		std::string writeBuffer;
		
		void write(const std::string& input);
		void write(const int& input);
		void write(const float& input);
		void write(const unsigned int& input);
		void write(const FloatVector3d& input);
		void write(const FloatVector2d& input);
		void write(const IntVector2d& input);
		void write(const String& input);
		void write(const char* input);
		LogFile();
		~LogFile();
};

extern LogFileStop endLine;

LogFile& operator<<(LogFile& l, const std::string& s);
LogFile& operator<<(LogFile& l, const String& s);
LogFile& operator<<(LogFile& l, const char* s);
LogFile& operator<<(LogFile& l, const int& s);
LogFile& operator<<(LogFile& l, const float& s);
LogFile& operator<<(LogFile& l, const LogFileStop& s);
LogFile& operator<<(LogFile& l, const FloatVector2d& s);
LogFile& operator<<(LogFile& l, const FloatVector3d& s);
LogFile& operator<<(LogFile& l, const unsigned int& s);
LogFile& operator<<(LogFile& l, const bool& s);
LogFile& operator<<(LogFile& l, const unsigned long& s);

class MainLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		MainLogFile();
};

class GuiLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		GuiLogFile();
};

class RendererLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		RendererLogFile();
};

class OsLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		OsLogFile();
};

class ConsoleLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		ConsoleLogFile();
};

class AudioLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		AudioLogFile();
};

class NetworkLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		NetworkLogFile();
};

class TextLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		TextLogFile();
};

class IoLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		IoLogFile();
};

class PropertyLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		PropertyLogFile();
};

class ScriptLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		ScriptLogFile();
};

class ScriptLogLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		ScriptLogLogFile();
};

class ScriptDebugLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		ScriptDebugLogFile();
};

class VideoLogFile : public LogFile{
	friend class LogViewer;
	
	public:
		VideoLogFile();
};


class Logs{
	public:
	MainLogFile main;
	//GuiLogFile gui;
	RendererLogFile renderer;
	//OsLogFile os;
	//ConsoleLogFile console;
	AudioLogFile audio;
	//NetworkLogFile network;
	TextLogFile text;
	//IoLogFile io;
	//PropertyLogFile property;
	ScriptLogFile script;
	VideoLogFile video;
	//ScriptLogLogFile scriptLog;
	//ScriptDebugLogFile scriptDebug;
	
	void init();
	
	Logs();
};

Logs& logs();

#endif

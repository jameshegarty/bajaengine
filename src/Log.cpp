#include "Log.hpp"

#ifndef XSICONVERT
	#include "os.h"
#endif

#include <fstream>

#ifdef _WIN32
	#include <time.h>
#endif

using namespace std;

LogFileStop endLine;

LogFile& operator<<(LogFile& l, const std::string& s) {
  l.writeBuffer+=s;
  return l;
}

LogFile& operator<<(LogFile& l, const String& s) {
	std::string t=s;
	l.writeBuffer+=t;
	return l;
}

LogFile& operator<<(LogFile& l, const char* s) {
	std::string temp=s;
	l.writeBuffer+=s;
	return l;
}

LogFile& operator<<(LogFile& l, const int& s) {
	l.writeBuffer+=stringFunctions.toString(s);
	return l; 
}

LogFile& operator<<(LogFile& l, const float& s) {
	l.writeBuffer+=stringFunctions.toString(s);
	return l;
}

LogFile& operator<<(LogFile& l, const LogFileStop& s) {
	l.write(l.writeBuffer);
	l.writeBuffer="";
	return l;
}

LogFile& operator<<(LogFile& l, const FloatVector2d& s) {
	l.writeBuffer+="[x] "+stringFunctions.toString(s.x)+" [y] "+stringFunctions.toString(s.y);
	return l;
}

LogFile& operator<<(LogFile& l, const FloatVector3d& s) {
	l.writeBuffer+="[x] "+stringFunctions.toString(s.x)+" [y] "+stringFunctions.toString(s.y)+" [z] "+stringFunctions.toString(s.z);
	return l;
}

LogFile& operator<<(LogFile& l, const unsigned int& s) {
	l.writeBuffer+=stringFunctions.toString(s);
 	return l;
}

LogFile& operator<<(LogFile& l, const bool& s) {
	if(s){
		l.writeBuffer+="true";
	}else{
		l.writeBuffer+="false";
	}
	
 	return l;
}

LogFile& operator<<(LogFile& l, const unsigned long& s) {
	l.writeBuffer+=stringFunctions.toString(s);
 	return l;
}


Logs::Logs(){
	
}

void LogFile::write(const int& input){
	write(stringFunctions.toString(input));
}

void LogFile::write(const float& input){
	write(stringFunctions.toString(input));
}

void LogFile::write(const unsigned int& input){
	write(stringFunctions.toString(input));
}

void LogFile::write(const FloatVector3d& input){
	write("[x] "+String(input.x)+"[y] "+String(input.y)+"[z] "+String(input.z));

}

void LogFile::write(const FloatVector2d& input){
	write("[x] "+String(input.x)+"[y] "+String(input.y));
	
}

void LogFile::write(const IntVector2d& input){
	write("[x] "+String(input.x)+"[y] "+String(input.y));
	
}

void LogFile::write(const String& input){
	std::string t=input;
	write(t);
}

void LogFile::write(const char* input){
	std::string i=input;
	write(i);
}




void LogFile::write(const std::string& input){

#ifdef HEADLESS
	std::cout << input.c_str() << endl;
#else

	//get time
	time_t rawtime;
	struct tm* timeinfo;

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	//generate string
	std::string add;
 	add+="[";
	add+=stringFunctions.toString(timeinfo->tm_mon);
	add+="/";
	add+=stringFunctions.toString(timeinfo->tm_mday);
	add+="/";
	add+=stringFunctions.toString(timeinfo->tm_year);
	add+=" ";
	add+=stringFunctions.toString(timeinfo->tm_hour);
	add+=":";
	add+=stringFunctions.toString(timeinfo->tm_min);
	add+=":";
	add+=stringFunctions.toString(timeinfo->tm_sec);
	add+="]: ";
 	add+=input;
 	
	
	#ifdef LOG_KEEP_FILES_OPEN
		if(out.is_open()==false){
			out.open(fileName.getAbsolute().c_str());
		}
		
		out << add << endl;
	#else
	data.pushBack(add);

	ofstream out(fileName.getAbsolute().c_str(),ofstream::out);

	if(out!=NULL){
		for(int i=0; i<data.size(); i++){
   			out << data[i] << "\n";
		}
	    
		out.close();
	}
	
	#endif
#endif

}

LogFile::LogFile(){

#ifndef XSICONVERT
	if(!os().folderExists(String("~/Library/Application Support/com.thelostmansion.thelostmansion"))){
		os().mkdir(String("~/Library/Application Support/com.thelostmansion.thelostmansion"));
	}
	
	if(!os().folderExists(String("~/Library/Application Support/com.thelostmansion.thelostmansion/logs"))){
		os().mkdir(String("~/Library/Application Support/com.thelostmansion.thelostmansion/logs"));
	}
#endif
}

LogFile::~LogFile(){
	#ifdef LOG_KEEP_FILES_OPEN
	out.close();
	#endif
}

GuiLogFile::GuiLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="gui";
	tfileName+=".log";

	fileName.setRelative(tfileName);

	write("Logfile started...");
}

RendererLogFile::RendererLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="renderer";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}


OsLogFile::OsLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="os";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

ConsoleLogFile::ConsoleLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="console";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

AudioLogFile::AudioLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="audio";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

TextLogFile::TextLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="text";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

NetworkLogFile::NetworkLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="network";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

MainLogFile::MainLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="main";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

IoLogFile::IoLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="io";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

PropertyLogFile::PropertyLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="property";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

ScriptLogFile::ScriptLogFile(){
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="script";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

ScriptLogLogFile::ScriptLogLogFile(){
	//output from ls (log) command
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="scriptLog";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

ScriptDebugLogFile::ScriptDebugLogFile(){
	//output from ls (debug) command
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="scriptDebug";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

VideoLogFile::VideoLogFile(){
	//output from ls (debug) command
	String tfileName="~/Library/Application Support/com.thelostmansion.thelostmansion/logs/";
	tfileName+="video";
	tfileName+=".log";
	
	fileName.setRelative(tfileName);

	write("Logfile started...");
}

void Logs::init(){

}

Logs& logs() {
	static Logs e;
	return e;
}

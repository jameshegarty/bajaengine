#ifndef SCRIPT_H
#define SCRIPT_H


//#define SCRIPTDEBUG 1
//#define OLD_SYNC

#include "Containers.hpp"
#include "HelperLibString.hpp"
#include "os.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#define MAX_SCRIPT_BYTES 51200

class ConsoleMessage{

	public:
		String message;
		char type;	//0=user, 1=error
};

class ScriptConsole{
	private:
		int repeatCount;
		Array<ConsoleMessage> store;
		String currentCommand;
		String currentCommandBkup;
		int ccIndex;

	public:
		bool visible;
		ScriptConsole();
		void write(String s);
		void writeUser(String s);
		void draw();
};

ScriptConsole& console();

#define SCRIPT_DOUBLE 0
#define SCRIPT_DOUBLE_STORED 1
#define SCRIPT_FLOAT 2
#define SCRIPT_BOOL 3
#define SCRIPT_TABLE 4
#define SCRIPT_INT 5
#define SCRIPT_STRING 6
#define SCRIPT_STRING_STORED 7
#define SCRIPT_FUNCTION 8
#define SCRIPT_BOOL_STORED 9
#define SCRIPT_INT_STORED 10
#define SCRIPT_TABLE_INT 11
#define SCRIPT_UNSIGNED_INT 12
#define SCRIPT_UNSIGNED_INT_STORED 13

class LuaType;

class LuaValue{
public:

	union{
		float* f;
		bool* b;
		bool bs;
		int* i;
		int is;
		unsigned int* ui;
		unsigned int uis;
		double* d;
		double ds;
		LuaType* t;
		String* s;
		lua_CFunction cfunc;
	};

	String ss;
};

class LuaType{
	private:
		

	public:
		LuaValue value;
		int type;
		LuaType* parent;
		bool newTable;

		SafeArray<LuaType> children;

		String name;
		int nameint;	//if the name is an intager, ie. the numeric index value of an array

		int remapIndex;
		~LuaType();
		LuaType();
		LuaType(const LuaType& s){
			value=s.value;
			type=s.type;
			parent=s.parent;
			newTable=s.newTable;
			children=s.children;
			name=s.name;
			nameint=s.nameint;
			remapIndex=s.remapIndex;
		}
};

#ifndef OLD_SYNC
	extern Array<LuaType*>* scriptVariableRemap;	//expose the remap table to that the lua cfunctions can access it
#endif

class Script{
	friend class ScriptConsole;

	private:
		Map<String,unsigned int> scriptBytesLoaded;
		bool signedFileLoaded;

		lua_State* L;

		LuaType lastIter;

		SafeArray<LuaType> global;
		bool readTable(SafeArray<LuaType>* lt);
		
		bool searchGlobal(void* f,SafeArray<LuaType>* lt);
		bool fatalError;
		bool doSync;

		#ifndef OLD_SYNC
		Array<LuaType*> remap;
		#endif

		bool pushTable(LuaType* obj);

		bool bindVariablesToTable(LuaType* parent,String path);
		bool initLuaSocket(lua_State *L);
	public:
		int next(int t,lua_State* L);	//a replacement for lua_next deals with our weird tables

		bool upload(LuaType* parent);

		bool syncTo(SafeArray<LuaType>* lt=NULL);
		bool syncFrom(SafeArray<LuaType>* lt=NULL);

		bool init();
		bool call(String function,int args, int results);

		LuaType* add(String name);	//add a table, null if error
		LuaType* add(String name,LuaType* parent);	//add a table, null if error
		LuaType* add(int name,LuaType* parent);	//add a table, null if error
		LuaType* add(String name, float* value);	//null if error
		LuaType* add(String name, float* value,LuaType* parent);	//null if error
		LuaType* add(String name, int* value,LuaType* parent);	//null if error
		LuaType* add(String name, int value,LuaType* parent);	//null if error
		LuaType* add(String name, unsigned int* value,LuaType* parent);	//null if error
		LuaType* add(String name, unsigned int value,LuaType* parent);	//null if error
		LuaType* add(String name, bool* value,LuaType* parent);	//null if error
		LuaType* add(String name, bool value,LuaType* parent);	//null if error
		LuaType* add(String name, String* value,LuaType* parent);	//null if error
		LuaType* add(String name, String value,LuaType* parent);	//null if error
		LuaType* add(String name, const char* value,LuaType* parent);	//null if error
		LuaType* add(String name, String* value);
		LuaType* add(String name, double* value,LuaType* parent);	//null if error
		LuaType* add(String name,lua_CFunction func);
		LuaType* add(String name,lua_CFunction func,LuaType* target);

		LuaType* change(LuaType* var, double* newValue);
		LuaType* change(LuaType* var, float* newValue);
		LuaType* change(LuaType* var, bool* newValue);

		LuaType* rename(LuaType* var,String name);

		bool remove(LuaType* var);

		void arg(char* arg);
		void arg(String& arg);

		void getArgsTable(SafeArray<LuaType>*,lua_State* LL);
		Array<LuaType> getArgs(lua_State* LL);

		void resultTable(lua_State* LL);
		void resultStartTable(lua_State* LL,String name);	//start a new sub-table on the result stack (resultTable must have already been called)
		void resultEndTable(lua_State* LL);
		void result(lua_State* LL,float value);
		void result(lua_State* LL,int value);
		void result(lua_State* LL,double value);
		void result(lua_State* LL,bool value);
		void result(lua_State* LL,String value);
		void result(lua_State* LL,String name, float value);
		void result(lua_State* LL,String name,lua_CFunction func);
		void result(lua_State* LL,int name, String value);
		
		void enableSync();
		void disableSync();

		bool pushOntoStack(LuaType* lt);	//push the value into -1

		bool reload();

		bool run(String command);
		bool runFile(Path file);
		bool runRaw(String command);	//the same as run but with nosync

		bool deleteGlobal(LuaType*);

		Script();
		~Script();
};	

Script& script();

#endif

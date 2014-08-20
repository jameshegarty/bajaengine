#ifndef INSTANCE_HPP
#define INSTANCE_HPP 1

#include "Types.hpp"
#include "objects.hpp"
#include "HelperLibString.hpp"
#include "script.hpp"
#include "animation.hpp"

//class ObjectAddress;
//#include "sceneGraph.hpp"

class Override{
	public:
		union{
			float floatVector3dX;	
		};

		union{
			float floatVector3dY;	
		};

		union{
			float floatVector3dZ;	
		};

		union{
			float pastFloatVector3dX;	
		};

		union{
			float pastFloatVector3dY;	
		};

		union{
			float pastFloatVector3dZ;	
		};

		union{
			float* pointerFloatVector3dX;	
		};

		union{
			float* pointerFloatVector3dY;	
		};

		union{
			float* pointerFloatVector3dZ;	
		};
		
		CommonType type;
		bool overridden;

		Override();
};

class Instance{
	private:
	public:
		Map<String,Override*> overrides;
		LuaType* luaObj;
		String name;
		void pre();
		void post();
		ObjectAddress* object;
		void draw();
		void link(String);
		Instance();
		SceneGraphNode* sceneGraphNode;

};

int luaInstanceLink(lua_State *L);

#endif
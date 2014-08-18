#include "LuaGl.hpp"
#include "script.hpp"

LuaGl luaGl;

#include "GlHeader.hpp"

int luaGlBegin (lua_State *L){
	Array<LuaType> t=script().getArgs(L);

	if(t.size()==1){
		if(t[0].type==SCRIPT_STRING_STORED){
			if(t[0].value.ss=="triangles"){
				glBegin(GL_TRIANGLES);
			}
		}
	}else{
		console().write("GL Error");
	}

	return 0;
}

int luaGlEnd (lua_State *L){
	glEnd();
	return 0;
}

int luaGlVertex (lua_State *L){
	Array<LuaType> t=script().getArgs(L);

	if(t.size()==3){
		float x,y,z;

		if(t[0].type==SCRIPT_DOUBLE_STORED){
			x=t[0].value.ds;
		}

		if(t[0].type==SCRIPT_DOUBLE_STORED){
			y=t[1].value.ds;
		}

		if(t[0].type==SCRIPT_DOUBLE_STORED){
			z=t[2].value.ds;
		}

		glVertex3f(x,y,z);
	}else{
		console().write("GL Error");
	}

	return 0;
}

bool LuaGl::scriptInit(){
	LuaType* g=script().add("gl");
		script().add("begin",luaGlBegin,g);
		script().add("end",luaGlEnd,g);
		script().add("vertex",luaGlVertex,g);

	return true;
}

LuaGl::LuaGl(){
	
}
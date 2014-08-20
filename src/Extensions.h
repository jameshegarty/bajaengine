#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "common.h"

class ExtensionsBase{
    private:
	bool GetExtensionsString();
	bool isExtensionSupported(char *extension);
	char *extensions;
	
    public:
		bool GL_ARB_multitexture_supported;
		bool GL_ARB_occlusion_query_supported;
		bool GL_ARB_shader_objects_supported;
		bool GL_ARB_vertex_shader_supported;
 		bool GL_ARB_vertex_program_supported;
  		bool GL_ARB_vertex_buffer_object_supported;
                bool GL_ARB_fragment_program_supported;
     	
		bool init();
};

#ifdef _WIN32
    #include "ExtensionsWindows.h"
#elif defined(LINUX)
    #include "ExtensionsLinux.h"
#elif defined(MACOSX)
    #include "ExtensionsMac.h"
#endif


#endif

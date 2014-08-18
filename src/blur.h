#ifndef BLUR_H
#define BLUR_H

#ifdef FEATURE_BLUR

#include "shader.hpp"

//extern unsigned int glBlurBase;
//extern unsigned int glBlurBuffer;
//extern ShaderId blurShader;
//extern ShaderId blurShaderH;

void drawBlur();
bool blurInit();

#endif

#endif

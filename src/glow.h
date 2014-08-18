#ifndef GLOW_H
#define GLOW_H


#ifdef FEATURE_GLOW
#include "shader.hpp"

extern unsigned int glBlurBase;
extern unsigned int glBlurBuffer;
//extern ShaderId blurShader;
//extern ShaderId blurShaderH;

void drawGlow();
bool glowInit();

#endif

#endif

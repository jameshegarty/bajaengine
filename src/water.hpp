#ifndef WATER_H
#define WATER_H

#ifdef FEATURE_WATER

#include "objects.hpp"
#include "conf.h"
#include "shader.hpp"

extern FloatVector3d waterPos;
extern FloatVector2d waterDirection;

void drawWaterBest();
void drawWaterGood();

void drawReflect();


#endif

bool waterInit();
bool waterScriptInit();
void drawWaterBad();

#endif


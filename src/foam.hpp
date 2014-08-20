#ifndef FOAM_H
#define FOAM_H

#ifdef FEATURE_WATER
extern float fAge;
extern float fDecay;
extern float fSize;
extern float fAlpha;
extern float fSpeed;

bool foamInit();
void buildFoam();
void drawFoamEdges();
void drawFoam();
#endif

#endif
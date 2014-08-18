#ifndef DRAW_H
#define DRAW_H

//#define PIPELINE_DYNAMIC 1		//enable dynamic renderer

#include "objects.hpp"
#include "conf.h"
#include "shader.hpp"
#include "water.hpp"

void buildNonstaticList();
void drawNonstaticList();
void environmentMapSetup(Object* obj);
void buildVBOs();
void drawBB();
void drawImplicit();
void drawVBOs();
void drawArray(Array<Object*>* objects,bool transform);
void drawArrayQuick(Array<Object*>* objects);	//just draws the geometry, doesn't bother with fancy stuff
void drawArrayInner(Object* object,bool transform);
bool drawInit();
void drawLevel();
void drawPanels();
void drawNormals();
void drawMouse();
void drawWidescreenPanel(float);
void updateLights(Object* obj);
void updateMaterial(Object* obj,bool transform);
void drawRefreshAll();				//rebuild all arrays/lists/etc
void drawTransparent(float yvalue,bool above,bool nowater);
void drawModern();
void drawNonstaticCallback(ObjectAddress a);
void drawNonstaticInner(Object* object);

void doPicking();

extern GLint maxLights;
extern ShaderId displaceFragmentShader;
extern ShaderId displaceVertexShader;
extern ShaderId specularFragmentShader;
extern ShaderId specularVertexShader;
extern ShaderId displaceFragmentShader;
extern ShaderId displaceVertexShader;
extern ShaderId specularFragmentShader;
extern ShaderId specularVertexShader;
extern ShaderId displaceSpecularFragmentShader;
extern ShaderId displaceSpecularVertexShader;
extern ShaderId displaceSpecularBumpFragmentShader;
extern ShaderId displaceSpecularBumpVertexShader;
extern ShaderId mixer_1_v;
extern ShaderId mixer_1_f;
extern ShaderId mixer_2_v;
extern ShaderId mixer_2_f;
extern ShaderId mixer_3_f;
extern ShaderId mixer_3_v;
extern ShaderId envelopeVertexShader;
extern ShaderId envelopeFragmentShader;
extern bool lightState;
extern bool listBuild;

#endif

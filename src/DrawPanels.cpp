/* DrawPanels.cpp
 * This file deals with drawing 2d panels on the screen
 */

#include "GlHeader.hpp"
										
#include "draw.hpp"
#include "level.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mouse.h"
#include "picking.h"
#include "hardware.hpp"
#include "random.h"

#include "Log.hpp"
#include "sort.hpp"
#include "HelperLibMath.hpp"

#include "lightmap.h"
#include "timeing.hpp"

#ifdef _WIN32
	#include <math.h>
#endif

#include "picker_matrix.h"

/* void panelPicker()
 *This calculates what panels the mouse is over and sends messages to them
 */
void panelPicker(){
	int topPanel=-1;
	float topDist=-200;
	int type=-1;

	int size=0;
	bool* mouseover;
	bool* visible;
	float* colorw;
	bool* clickable;
	FloatVector3d* pickerPos;
	FloatVector2d* pickerScale;
	String* name;

#ifdef DEVELOPER
	for(int m=0; m<4; m++){
#else
	for(int m=0; m<2; m++){
#endif
		if(m==0){
			size=level->panels.size();
		}else if(m==1){
			size=level->textpanels.size();
		}
#ifdef DEVELOPER
		else if(m==2){
			size=editorPanels.size();
		}else if(m==3){
			size=editorTextpanels.size();
		}
#endif

		for(int i=0; i<size; i++){

			if(m==0){
				mouseover=&level->panels[i]->mouseover;
				visible=&level->panels[i]->visible;
				clickable=&level->panels[i]->clickable;
				colorw=&level->panels[i]->color.w;
				pickerPos=&level->panels[i]->pickerPos;
				pickerScale=&level->panels[i]->pickerScale;
			}else if(m==1){
				mouseover=&level->textpanels[i]->mouseover;
				visible=&level->textpanels[i]->visible;
				clickable=&level->textpanels[i]->clickable;
				colorw=&level->textpanels[i]->color.w;
				pickerPos=&level->textpanels[i]->pickerPos;
				pickerScale=&level->textpanels[i]->pickerScale;
			}
#ifdef DEVELOPER
			else if(m==2){
				mouseover=&editorPanels[i]->mouseover;
				visible=&editorPanels[i]->visible;
				clickable=&editorPanels[i]->clickable;
				colorw=&editorPanels[i]->color.w;
				pickerPos=&editorPanels[i]->pickerPos;
				pickerScale=&editorPanels[i]->pickerScale;
			}else if(m==3){
				mouseover=&editorTextpanels[i]->mouseover;
				visible=&editorTextpanels[i]->visible;
				clickable=&editorTextpanels[i]->clickable;
				colorw=&editorTextpanels[i]->color.w;
				pickerPos=&editorTextpanels[i]->pickerPos;
				pickerScale=&editorTextpanels[i]->pickerScale;
			}
#endif
			*mouseover=false;

			if(*visible && *colorw>0 && *clickable){

				if(	mouse.x>pickerPos->x && 
					mouse.y<pickerPos->y){

					if(	mouse.x<pickerPos->x+pickerScale->x && 
						mouse.y>pickerPos->y-pickerScale->y){

							if(pickerPos->z>topDist){
								topDist=pickerPos->z;
								topPanel=i;
								type=m;
							}
					}
				}
			}
		}
	}


	if(topPanel!=-1){
		if(type==0){
			mouseover=&level->panels[topPanel]->mouseover;
			name=&level->panels[topPanel]->name;
		}else if(type==1){
			mouseover=&level->textpanels[topPanel]->mouseover;
			name=&level->textpanels[topPanel]->name;
		}
#ifdef DEVELOPER
		else if(type==2){
			mouseover=&editorPanels[topPanel]->mouseover;
			name=&editorPanels[topPanel]->name;
		}else if(type==3){
			mouseover=&editorTextpanels[topPanel]->mouseover;
			name=&editorTextpanels[topPanel]->name;
		}
#endif

		*mouseover=true;

		if(mouse.lUp){
			if(type==0){
				if(level->panels[topPanel]->onMouseLUp!=NULL){
					level->panels[topPanel]->onMouseLUp();
				}
			}else if(type==1){
				if(level->textpanels[topPanel]->onMouseLUp!=NULL){
					level->textpanels[topPanel]->onMouseLUp();
				}
			}
#ifdef DEVELOPER
			else if(type==2){
				if(editorPanels[topPanel]->onMouseLUp!=NULL){
					editorPanels[topPanel]->onMouseLUp();
				}
			}else if(type==3){
				if(editorTextpanels[topPanel]->onMouseLUp!=NULL){
					editorTextpanels[topPanel]->onMouseLUp();
				}
			}
#endif
	
			if(type==0 || type==1){
				script().run("if obj['"+*name+"'].onMouseLUp~=nil then obj['"+*name+"'].onMouseLUp() end");
			}
		}else if(mouse.lDown){
			if(type==0){
				if(level->panels[topPanel]->onMouseLDown!=NULL){
					level->panels[topPanel]->onMouseLDown();
				}
			}else if(type==1){
				if(level->textpanels[topPanel]->onMouseLDown!=NULL){
					level->textpanels[topPanel]->onMouseLDown();
				}
			}
#ifdef DEVELOPER
			else if(type==2){
				if(editorPanels[topPanel]->onMouseLDown!=NULL){
					editorPanels[topPanel]->onMouseLDown();
				}
			}else if(type==3){
				if(editorTextpanels[topPanel]->onMouseLDown!=NULL){
					editorTextpanels[topPanel]->onMouseLUp();
				}
			}
#endif
			if(type==0 || type==1){
				script().run("if obj['"+*name+"'].onMouseLDown~=nil then obj['"+*name+"'].onMouseLDown() end");
			}
		}

	}

}

/* void drawPanels()
 * This function sets up matricies and OpenGL state properly
 * and then draws the 2d panels on the screen
 */
void drawPanels(){

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels START");
#endif

	//clear the depth info from the 3d objects, so that they don't overlap the panels
	glClear (GL_DEPTH_BUFFER_BIT);

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	glMatrixMode (GL_TEXTURE);										// Select The Modelview Matrix
		glLoadIdentity ();	

	glMatrixMode (GL_PROJECTION);										// Select The Projection Matrix
	glLoadIdentity ();

	glOrtho(0,conf->sizeX,-conf->sizeY,0,-100,100);

	glMatrixMode (GL_MODELVIEW);										// Select The Modelview Matrix

	glLoadIdentity ();	
	
#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels glEnable(GL_BLEND);");
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_LIGHTING);

	glDisable(GL_CULL_FACE);


	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels shader.reset()");
#endif
	shader.reset();

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels T0");
#endif
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels T1");
#endif
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels T2");
#endif
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels T3");
#endif
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels T4");
#endif
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);

	float amb[4];

	amb[0]=1;
	amb[1]=1;
	amb[2]=1;
	amb[3]=1;

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels DFOG");
#endif
	glDisable (GL_FOG);

	engineTime.startTimer("picking2d");

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels panelPicker");
#endif

	panelPicker();
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	engineTime.endTimer("picking2d");

	glEnable(GL_DEPTH_TEST);

	Array<SortItem> input;

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels panels");
#endif

	for(int i=0; i<level->panels.size(); i++){
		SortItem r;
		r.value=level->panels[i]->pos.z;
		r.key=i;
		input.pushBack(r);
	}

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels textpanels");
#endif

	for(int i=0; i<level->textpanels.size(); i++){
		SortItem r;
		r.value=level->textpanels[i]->pos.z;
		r.key=i+level->panels.size();
		input.pushBack(r);
	}


	Array<int> output=sort.sort(input,true);

#ifdef LOOP_DEBUG
	logs().main.write("LOOP: drawPanels output.size()");
#endif

	for(int i=0; i<output.size(); i++){
		if(input[output[i]].key>=level->panels.size()){
			level->textpanels[input[output[i]].key-level->panels.size()]->draw();
		}else{
			level->panels[input[output[i]].key]->draw();
		}
	}


	for(int i=0; i<level->lines2d.size(); i++){
		level->lines2d[i]->draw();
	}

	for(int i=0; i<level->circles.size(); i++){
		level->circles[i]->draw();
	}

	

#ifdef DEVELOPER

	glClear (GL_DEPTH_BUFFER_BIT);

	input.clear();

	for(int i=0; i<editorTextpanels.size(); i++){
		SortItem r;
		r.value=editorTextpanels[i]->pos.z;
		r.key=i;
		input.pushBack(r);
	}

	for(int i=0; i<editorPanels.size(); i++){
		SortItem r;
		r.value=editorPanels[i]->pos.z;
		r.key=i+editorTextpanels.size();
		input.pushBack(r);
	}

	output.clear();
	output=sort.sort(input,true);

	for(int i=0; i<output.size(); i++){
		if(input[output[i]].key>=editorTextpanels.size()){
			editorPanels[input[output[i]].key-editorTextpanels.size()]->draw();
		}else{
			editorTextpanels[input[output[i]].key]->draw();
		}
	}

	for(int i=0; i<editorLines2d.size(); i++){
		if(editorLines2d[i]->visible){
			editorLines2d[i]->draw();
		}
	}

	for(int i=0; i<editorPolygons2d.size(); i++){
		if(editorPolygons2d[i]->visible){
			editorPolygons2d[i]->draw();
		}
	}
#endif

	
}


#ifdef DEVELOPER

#include "editor.h"
#include "keyboard.h"
#include "EditorTextbox.h"
#include "script.hpp"
#include "conf.h"
#include "mouse.h"
#include "texturelibrary.h"
#include "lightmap.h"
#include "HelperLibMath.hpp"
#include "texture.hpp"
#include "level.hpp"

ObjectAddress propertiesFade;
bool propertiesVisible=false;

ObjectAddress lmTag;
ObjectAddress lmTagW;
ObjectAddress lmTagH;
ObjectAddress lmTagWBkg;
ObjectAddress lmTagWBkgTop;
ObjectAddress lmTagHBkg;
ObjectAddress lmTagHBkgTop;
bool lmTagWActive=false;
bool lmTagHActive=false;

ObjectAddress propStatic;
ObjectAddress propStaticBkg;
ObjectAddress propStaticBkgTop;
ObjectAddress propStaticChecked;

ObjectAddress propCollide;
ObjectAddress propCollideBkg;
ObjectAddress propCollideBkgTop;
ObjectAddress propCollideChecked;

ObjectAddress noclip;
ObjectAddress noclipBkg;
ObjectAddress noclipBkgTop;
ObjectAddress noclipChecked;

void setLmTagWActive(){
	lmTagWActive=true;
}

void setLmTagHActive(){
	lmTagHActive=true;
}

void toggleCollide(){
	selectedObjects[0]->collide=!selectedObjects[0]->collide;

	if(selectedObjects[0]->collide){
		level->colliders.pushBack(selectedObjects[0]);
	}else{
		int fnd=level->colliders.find(selectedObjects[0]);
		if(fnd!=-1){
			level->colliders.erase(fnd);
		}
	}
}

void toggleStatic(){
	
	selectedObjects[0]->flags^=OBJECT_STATIC;
}


void toggleNoclip(){
	selectedObjects[0]->noclip=!selectedObjects[0]->noclip;
	selectedObjects[0]->flags^=OBJECT_NOCLIP;
}

void Editor::drawProperties(){

	if(deactivateGui("properties")){
		propertiesVisible=false;
	}else if(activateGui("properties")){
		propertiesVisible=true;
	}

	if(propertiesVisible){

		propertiesFade.panel->visible=true;

		if(mouse.lDown || keyboard.enter_down){
			lmTagWActive=false;
			if(selectedObjects.size()==1){
				lmSizes[selectedObjects[0]->name].x=targetPowerOf2(lmSizes[selectedObjects[0]->name].x);
				lmSizes[selectedObjects[0]->name].y=targetPowerOf2(lmSizes[selectedObjects[0]->name].y);
				saveLightmapSizes();
			}

			lmTagHActive=false;
		}

		lmTag.textpanel->visible=true;
		lmTagW.textpanel->visible=true;
		lmTagH.textpanel->visible=true;
		lmTagWBkg.panel->visible=true;
		lmTagWBkgTop.panel->visible=true;
		lmTagHBkg.panel->visible=true;
		lmTagHBkgTop.panel->visible=true;

		propCollide.textpanel->visible=true;
		propCollideBkg.panel->visible=true;
		propCollideBkgTop.panel->visible=true;

		if(selectedObjects[0]->collide){
			propCollideChecked.panel->visible=true;
		}else{
			propCollideChecked.panel->visible=false;
		}

		propStatic.textpanel->visible=true;
		propStaticBkg.panel->visible=true;
		propStaticBkgTop.panel->visible=true;

		if(selectedObjects[0]->flags & OBJECT_STATIC){
			propStaticChecked.panel->visible=true;
		}else{
			propStaticChecked.panel->visible=false;
		}

		noclip.textpanel->visible=true;
		noclipBkg.panel->visible=true;
		noclipBkgTop.panel->visible=true;

		if(selectedObjects[0]->flags & OBJECT_NOCLIP){
			noclipChecked.panel->visible=true;
		}else{
			noclipChecked.panel->visible=false;
		}


		loadLightmapSizes();

		

		if(!lmSizes.find(selectedObjects[0]->name)){
			IntVector2d v(256,256);
			lmSizes[selectedObjects[0]->name]=v;
		}

		String w;
		if(lmSizes.find(selectedObjects[0]->name)){
			w=lmSizes[selectedObjects[0]->name].x;
		}else{
			w="0";
		}

		String h;
		if(lmSizes.find(selectedObjects[0]->name)){
			h=lmSizes[selectedObjects[0]->name].y;
		}else{
			h="0";
		}

		lmTagW.textpanel->setText(w);
		lmTagH.textpanel->setText(h);

		if(lmTagWActive && keyboard.backspaceDown){
			lmSizes[selectedObjects[0]->name].x=math.floor(float(lmSizes[selectedObjects[0]->name].x)*float(0.1));
		}

		if(lmTagWActive && keyboard.number_down_hit){
			int o=lmSizes[selectedObjects[0]->name].x;

			lmSizes[selectedObjects[0]->name].x*=10;
			lmSizes[selectedObjects[0]->name].x+=keyboard.current_number_down;

			if(lmSizes[selectedObjects[0]->name].x>4096){
				lmSizes[selectedObjects[0]->name].x=o;
			}
		}


		if(lmTagHActive && keyboard.backspaceDown){
			lmSizes[selectedObjects[0]->name].y=math.floor(float(lmSizes[selectedObjects[0]->name].y)*float(0.1));
		}

		if(lmTagHActive && keyboard.number_down_hit){
			int o=lmSizes[selectedObjects[0]->name].y;

			lmSizes[selectedObjects[0]->name].y*=10;
			lmSizes[selectedObjects[0]->name].y+=keyboard.current_number_down;

			if(lmSizes[selectedObjects[0]->name].y>4096){
				lmSizes[selectedObjects[0]->name].y=o;
			}
		}

		if(lmTagWActive){
			lmTagWBkgTop.panel->color.x=0.2f;
			lmTagWBkgTop.panel->color.y=0.4f;
			lmTagWBkgTop.panel->color.z=0.2f;

		}else{
			lmTagWBkgTop.panel->color.x=0.2f;
			lmTagWBkgTop.panel->color.y=0.3f;
			lmTagWBkgTop.panel->color.z=0.2f;

		}

		if(lmTagHActive){
			lmTagHBkgTop.panel->color.x=0.2f;
			lmTagHBkgTop.panel->color.y=0.4f;
			lmTagHBkgTop.panel->color.z=0.2f;
		}else{
			lmTagHBkgTop.panel->color.x=0.2f;
			lmTagHBkgTop.panel->color.y=0.3f;
			lmTagHBkgTop.panel->color.z=0.2f;
		}

		lmTag.textpanel->pos.y=-conf->sizeY+25;

		lmTagW.textpanel->pos.y=-conf->sizeY+float(25);
		lmTagWBkg.panel->pos.y=-conf->sizeY+float(28);
		lmTagWBkgTop.panel->pos.y=-conf->sizeY+float(26);

		lmTagH.textpanel->pos.y=-conf->sizeY+float(25);
		lmTagHBkg.panel->pos.y=-conf->sizeY+float(28);
		lmTagHBkgTop.panel->pos.y=-conf->sizeY+float(26);

		if(mouse.lDown && lmTagWBkg.panel->mouseover){
			editorButtonDownHack=true;
		}

		if(mouse.lDown && lmTagHBkg.panel->mouseover){
			editorButtonDownHack=true;
		}

		if(mouse.lDown && lmTagWBkgTop.panel->mouseover){
			editorButtonDownHack=true;
		}

		if(mouse.lDown && lmTagHBkgTop.panel->mouseover){
			editorButtonDownHack=true;
		}
	}else{

		propertiesFade.panel->visible=false;

		lmTag.textpanel->visible=false;
		lmTagW.textpanel->visible=false;
		lmTagH.textpanel->visible=false;
		lmTagWBkg.panel->visible=false;
		lmTagWBkgTop.panel->visible=false;
		lmTagHBkg.panel->visible=false;
		lmTagHBkgTop.panel->visible=false;

		propCollide.textpanel->visible=false;
		propCollideBkg.panel->visible=false;
		propCollideBkgTop.panel->visible=false;
		propCollideChecked.panel->visible=false;

		propStatic.textpanel->visible=false;
		propStaticBkg.panel->visible=false;
		propStaticBkgTop.panel->visible=false;
		propStaticChecked.panel->visible=false;

		noclip.textpanel->visible=false;
		noclipBkg.panel->visible=false;
		noclipBkgTop.panel->visible=false;
		noclipChecked.panel->visible=false;
	}

}

void Editor::initProperties(){

	propertiesFade=add("propertiesFade","panel");
	propertiesFade.panel->pos.x=0;
	propertiesFade.panel->pos.y=0;
	propertiesFade.panel->pos.z=90;
	propertiesFade.panel->color.x=0;
	propertiesFade.panel->color.y=0;
	propertiesFade.panel->color.z=0;
	propertiesFade.panel->color.w=0.5;
	propertiesFade.panel->percent=true;
	propertiesFade.panel->scale.x=1;
	propertiesFade.panel->scale.y=1;
	propertiesFade.panel->visible=false;

	lmTag=add("lmTag","textpanel");
	lmTag.textpanel->pos.x=float(55);
	lmTag.textpanel->pos.y=-float(1160)+float(800);
	lmTag.textpanel->pos.z=94;
	lmTag.textpanel->color.x=1.0;
	lmTag.textpanel->color.y=1.0;
	lmTag.textpanel->color.z=1.0;
	lmTag.textpanel->color.w=1.0;
	lmTag.textpanel->setText("Lightmap Size:");
	lmTag.textpanel->visible=false;
	lmTag.textpanel->percent=false;

	lmTagW=add("lmTagW","textpanel");
	lmTagW.textpanel->pos.x=float(310)-float(130);
	lmTagW.textpanel->pos.y=-float(1160)+float(800);
	lmTagW.textpanel->pos.z=96;
	lmTagW.textpanel->color.x=1.0f;
	lmTagW.textpanel->color.y=1.0f;
	lmTagW.textpanel->color.z=1.0f;
	lmTagW.textpanel->color.w=1.0f;
	lmTagW.textpanel->setText("256");
	lmTagW.textpanel->visible=false;
	lmTagW.textpanel->percent=false;

	lmTagWBkg=add("lmTagWBkg","panel");
	lmTagWBkg.panel->pos.x=float(300)-float(130);
	lmTagWBkg.panel->pos.y=-float(1156)+float(800);
	lmTagWBkg.panel->pos.z=93.0f;
	lmTagWBkg.panel->keepAspectRatio=true;
	lmTagWBkg.panel->color.x=0.2f;
	lmTagWBkg.panel->color.y=0.5f;
	lmTagWBkg.panel->color.z=0.2f;
	lmTagWBkg.panel->scale.x=float(60);
	lmTagWBkg.panel->scale.y=float(26);
	lmTagWBkg.panel->visible=false;
	lmTagWBkg.panel->percent=false;

	lmTagWBkgTop=add("lmTagWBkgTop","panel");
	lmTagWBkgTop.panel->pos.x=float(302)-float(130);
	lmTagWBkgTop.panel->pos.y=-float(1158)+float(800);
	lmTagWBkgTop.panel->pos.z=94.0f;
	lmTagWBkgTop.panel->keepAspectRatio=true;
	lmTagWBkgTop.panel->color.x=0.2f;
	lmTagWBkgTop.panel->color.y=0.3f;
	lmTagWBkgTop.panel->color.z=0.2f;
	lmTagWBkgTop.panel->scale.x=float(56);
	lmTagWBkgTop.panel->scale.y=float(22);
	lmTagWBkgTop.panel->visible=false;
	lmTagWBkgTop.panel->percent=false;
	lmTagWBkgTop.panel->onMouseLDown=setLmTagWActive;

	lmTagH=add("lmTagH","textpanel");
	lmTagH.textpanel->pos.x=float(400)-float(150);
	lmTagH.textpanel->pos.y=-float(1160)+float(800);
	lmTagH.textpanel->pos.z=96.0f;
	lmTagH.textpanel->color.x=1.0f;
	lmTagH.textpanel->color.y=1.0f;
	lmTagH.textpanel->color.z=1.0f;
	lmTagH.textpanel->color.w=1.0f;
	lmTagH.textpanel->setText("256");
	lmTagH.textpanel->visible=false;
	lmTagH.textpanel->percent=false;

	lmTagHBkg=add("lmTagHBkg","panel");
	lmTagHBkg.panel->pos.x=float(390)-float(150);
	lmTagHBkg.panel->pos.y=-float(1156)+float(800);
	lmTagHBkg.panel->pos.z=93;
	lmTagHBkg.panel->keepAspectRatio=true;
	lmTagHBkg.panel->color.x=0.2f;
	lmTagHBkg.panel->color.y=0.5f;
	lmTagHBkg.panel->color.z=0.2f;
	lmTagHBkg.panel->scale.x=float(60);
	lmTagHBkg.panel->scale.y=float(26);
	lmTagHBkg.panel->visible=false;
	lmTagHBkg.panel->percent=false;

	lmTagHBkgTop=add("lmTagHBkgTop","panel");
	lmTagHBkgTop.panel->pos.x=float(392)-float(150);
	lmTagHBkgTop.panel->pos.y=-float(1158)+float(800);
	lmTagHBkgTop.panel->pos.z=94.0f;
	lmTagHBkgTop.panel->keepAspectRatio=true;
	lmTagHBkgTop.panel->color.x=0.2f;
	lmTagHBkgTop.panel->color.y=0.3f;
	lmTagHBkgTop.panel->color.z=0.2f;
	lmTagHBkgTop.panel->scale.x=float(56);
	lmTagHBkgTop.panel->scale.y=float(22);
	lmTagHBkgTop.panel->visible=false;
	lmTagHBkgTop.panel->percent=false;
	lmTagHBkgTop.panel->onMouseLDown=setLmTagHActive;

	propCollide=add("propCollide","textpanel");
	propCollide.textpanel->pos.x=float(90);
	propCollide.textpanel->pos.y=-50.0f;
	propCollide.textpanel->pos.z=96.0f;
	propCollide.textpanel->color.x=1.0f;
	propCollide.textpanel->color.y=1.0f;
	propCollide.textpanel->color.z=1.0f;
	propCollide.textpanel->color.w=1.0f;
	propCollide.textpanel->setText("Collide");
	propCollide.textpanel->visible=false;
	propCollide.textpanel->percent=false;

	propCollideBkg=add("propCollideBkg","panel");
	propCollideBkg.panel->pos.x=float(60);
	propCollideBkg.panel->pos.y=-50.0f;
	propCollideBkg.panel->pos.z=96.0f;
	propCollideBkg.panel->color.x=0.2f;
	propCollideBkg.panel->color.y=0.5f;
	propCollideBkg.panel->color.z=0.2f;
	propCollideBkg.panel->color.w=1.0f;
	propCollideBkg.panel->scale.x=20.0f;
	propCollideBkg.panel->scale.y=20.0f;
	propCollideBkg.panel->visible=false;
	propCollideBkg.panel->percent=false;
	propCollideBkg.panel->onMouseLUp=toggleCollide;

	propCollideBkgTop=add("propCollideBkgTop","panel");
	propCollideBkgTop.panel->pos.x=float(62);
	propCollideBkgTop.panel->pos.y=-52.0f;
	propCollideBkgTop.panel->pos.z=97.0f;
	propCollideBkgTop.panel->color.x=0.0f;
	propCollideBkgTop.panel->color.y=0.0f;
	propCollideBkgTop.panel->color.z=0.0f;
	propCollideBkgTop.panel->color.w=1.0f;
	propCollideBkgTop.panel->scale.x=16.0f;
	propCollideBkgTop.panel->scale.y=16.0f;
	propCollideBkgTop.panel->visible=false;
	propCollideBkgTop.panel->percent=false;
	propCollideBkgTop.panel->clickable=false;

	propCollideChecked=add("propCollideChecked","panel");
	propCollideChecked.panel->pos.x=float(64);
	propCollideChecked.panel->pos.y=-54.0f;
	propCollideChecked.panel->pos.z=98.0f;
	propCollideChecked.panel->color.x=0.2f;
	propCollideChecked.panel->color.y=0.5f;
	propCollideChecked.panel->color.z=0.2f;
	propCollideChecked.panel->color.w=1.0f;
	propCollideChecked.panel->scale.x=12.0f;
	propCollideChecked.panel->scale.y=12.0f;
	propCollideChecked.panel->visible=false;
	propCollideChecked.panel->percent=false;
	propCollideChecked.panel->clickable=false;

	propStatic=add("propStatic","textpanel");
	propStatic.textpanel->pos.x=float(230);
	propStatic.textpanel->pos.y=-50.0f;
	propStatic.textpanel->pos.z=96.0f;
	propStatic.textpanel->color.x=1.0f;
	propStatic.textpanel->color.y=1.0f;
	propStatic.textpanel->color.z=1.0f;
	propStatic.textpanel->color.w=1.0f;
	propStatic.textpanel->setText("Static");
	propStatic.textpanel->visible=false;
	propStatic.textpanel->percent=false;

	propStaticBkg=add("propStaticBkg","panel");
	propStaticBkg.panel->pos.x=float(200);
	propStaticBkg.panel->pos.y=-50.0f;
	propStaticBkg.panel->pos.z=96.0f;
	propStaticBkg.panel->color.x=0.2f;
	propStaticBkg.panel->color.y=0.5f;
	propStaticBkg.panel->color.z=0.2f;
	propStaticBkg.panel->color.w=1.0f;
	propStaticBkg.panel->scale.x=20.0f;
	propStaticBkg.panel->scale.y=20.0f;
	propStaticBkg.panel->visible=false;
	propStaticBkg.panel->percent=false;
	propStaticBkg.panel->onMouseLUp=toggleStatic;


	propStaticBkgTop=add("propStaticBkgTop","panel");
	propStaticBkgTop.panel->pos.x=float(202);
	propStaticBkgTop.panel->pos.y=-52.0f;
	propStaticBkgTop.panel->pos.z=97.0f;
	propStaticBkgTop.panel->color.x=0.0f;
	propStaticBkgTop.panel->color.y=0.0f;
	propStaticBkgTop.panel->color.z=0.0f;
	propStaticBkgTop.panel->color.w=1.0f;
	propStaticBkgTop.panel->scale.x=16.0f;
	propStaticBkgTop.panel->scale.y=16.0f;
	propStaticBkgTop.panel->visible=false;
	propStaticBkgTop.panel->percent=false;
	propStaticBkgTop.panel->clickable=false;

	propStaticChecked=add("propStaticChecked","panel");
	propStaticChecked.panel->pos.x=float(204);
	propStaticChecked.panel->pos.y=-54;
	propStaticChecked.panel->pos.z=98;
	propStaticChecked.panel->color.x=0.2f;
	propStaticChecked.panel->color.y=0.5f;
	propStaticChecked.panel->color.z=0.2f;
	propStaticChecked.panel->color.w=1.0f;
	propStaticChecked.panel->scale.x=12;
	propStaticChecked.panel->scale.y=12;
	propStaticChecked.panel->visible=false;
	propStaticChecked.panel->percent=false;
	propStaticChecked.panel->clickable=false;

	noclip=add("noclip","textpanel");
	noclip.textpanel->pos.x=float(330);
	noclip.textpanel->pos.y=-50;
	noclip.textpanel->pos.z=96;
	noclip.textpanel->color.x=1.0f;
	noclip.textpanel->color.y=1.0f;
	noclip.textpanel->color.z=1.0f;
	noclip.textpanel->color.w=1.0f;
	noclip.textpanel->setText("No Clip");
	noclip.textpanel->visible=false;
	noclip.textpanel->percent=false;

	noclipBkg=add("noclipBkg","panel");
	noclipBkg.panel->pos.x=float(300);
	noclipBkg.panel->pos.y=-50;
	noclipBkg.panel->pos.z=96;
	noclipBkg.panel->color.x=0.2f;
	noclipBkg.panel->color.y=0.5f;
	noclipBkg.panel->color.z=0.2f;
	noclipBkg.panel->color.w=1.0f;
	noclipBkg.panel->scale.x=20;
	noclipBkg.panel->scale.y=20;
	noclipBkg.panel->visible=false;
	noclipBkg.panel->percent=false;
	noclipBkg.panel->onMouseLUp=toggleNoclip;


	noclipBkgTop=add("noclipBkgTop","panel");
	noclipBkgTop.panel->pos.x=float(302);
	noclipBkgTop.panel->pos.y=-52;
	noclipBkgTop.panel->pos.z=97;
	noclipBkgTop.panel->color.x=0.0f;
	noclipBkgTop.panel->color.y=0.0f;
	noclipBkgTop.panel->color.z=0.0f;
	noclipBkgTop.panel->color.w=1.0f;
	noclipBkgTop.panel->scale.x=16;
	noclipBkgTop.panel->scale.y=16;
	noclipBkgTop.panel->visible=false;
	noclipBkgTop.panel->percent=false;
	noclipBkgTop.panel->clickable=false;

	noclipChecked=add("noclipChecked","panel");
	noclipChecked.panel->pos.x=float(304);
	noclipChecked.panel->pos.y=-54;
	noclipChecked.panel->pos.z=98;
	noclipChecked.panel->color.x=0.2f;
	noclipChecked.panel->color.y=0.5f;
	noclipChecked.panel->color.z=0.2f;
	noclipChecked.panel->color.w=1.0f;
	noclipChecked.panel->scale.x=12;
	noclipChecked.panel->scale.y=12;
	noclipChecked.panel->visible=false;
	noclipChecked.panel->percent=false;
	noclipChecked.panel->clickable=false;

}

#endif

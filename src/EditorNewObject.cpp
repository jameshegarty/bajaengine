#ifdef DEVELOPER

#include "editor.h"
#include "keyboard.h"
#include "EditorTextbox.h"
#include "script.hpp"

ObjectAddress newFade;
ObjectAddress newTypes;
ObjectAddress newTypesBkg;
bool newActive=false;

void Editor::drawNew(){

	if(newActive){
		if(activateEvent("c",false,"new")){
			addCubeAction();
			forceGui("new",false);
			newActive=false;
		}
		
		if(activateEvent("s",false,"new")){
			addSpotlightAction();
			forceGui("new",false);
			newActive=false;
		}

		if(activateEvent("l",false,"new")){
			addPointLightAction();
			forceGui("new",false);
			newActive=false;
		}

		if(activateEvent("g",false,"new")){
			addGridAction();
			forceGui("new",false);
			newActive=false;
		}

		if(activateEvent("x",true,"new")){
			forceGui("new",false);
			newActive=false;
		}
	}

	if(deactivateGui("new")){
		newActive=false;
	}else if(activateGui("new")){
		newActive=true;
	}

	if(newActive){

		newFade.panel->visible=true;
		newTypes.textpanel->visible=true;
		newTypesBkg.panel->visible=true;
	}else{

		newFade.panel->visible=false;
		newTypes.textpanel->visible=false;
		newTypesBkg.panel->visible=false;

	}

}

void Editor::initNew(){

	newFade=add("newFade","panel");
	newFade.panel->pos.x=0;
	newFade.panel->pos.y=0;
	newFade.panel->pos.z=90;
	newFade.panel->color.x=0;
	newFade.panel->color.y=0;
	newFade.panel->color.z=0;
	newFade.panel->color.w=0.5;
	newFade.panel->percent=true;
	newFade.panel->scale.x=1;
	newFade.panel->scale.y=1;
	newFade.panel->visible=false;

	newTypesBkg=add("newTypesBkg","panel");
	newTypesBkg.panel->pos.x=50;
	newTypesBkg.panel->pos.y=-30;
	newTypesBkg.panel->pos.z=91;
	newTypesBkg.panel->scale.x=200;
	newTypesBkg.panel->scale.y=200;

	newTypesBkg.panel->color.x=0.5;
	newTypesBkg.panel->color.y=0.5;
	newTypesBkg.panel->color.z=0.5;
	newTypesBkg.panel->color.w=1;

	newTypesBkg.panel->percent=false;
	newTypesBkg.panel->visible=false;

	newTypes=add("newTypes","textpanel");
	newTypes.textpanel->pos.x=50;
	newTypes.textpanel->pos.y=-30;
	newTypes.textpanel->pos.z=92;
	newTypes.textpanel->color.x=1;
	newTypes.textpanel->color.y=1;
	newTypes.textpanel->color.z=1;
	newTypes.textpanel->color.w=1;
	newTypes.textpanel->percent=false;
	newTypes.textpanel->visible=false;
	newTypes.textpanel->setText("Select:\n\nc\t:\tCube\ng\t:\tGrid\nl\t:\tLight\ns\t:\tSpotlight\n\nctrl - x: Cancel");
}

#endif
#ifdef DEVELOPER

#include "Editor.h"
#include "SceneGraph.hpp"
#include "Text.hpp"
#include "keyboard.h"
#include "mouse.h"

float explorerX=300;
float explorerY=-100;
Array<ObjectAddress> explorerPanels;
ObjectAddress explorerFade;

int Editor::drawExplorerNode(SceneGraphNode& node, int p){

	ObjectAddress tp;

	if(p>=explorerPanels.size()){

		tp=add("explorerPanel"+String(p),"textpanel");

		tp.textpanel->percent=false;
		tp.textpanel->pos.z=91;
		explorerPanels.pushBack(tp);
	}else{
		tp=explorerPanels[p];
	}

	String nn;

	tp.textpanel->color.x=1;
	tp.textpanel->color.y=1;
	tp.textpanel->color.z=1;
	tp.textpanel->color.w=1;

	tp.textpanel->visible=true;

	if(node.parent==NULL){
		nn="Scene Root";
	}else if(node.object.type==OBJECT){
		nn=node.object.object->name;

		if(!node.object.object->visible){
			tp.textpanel->color.x=0.5;
			tp.textpanel->color.y=0.5;
			tp.textpanel->color.z=0.5;
			tp.textpanel->color.w=1;
		}

		if(selectedObjects.find(node.object.object)!=-1){
			tp.textpanel->color.x=1;
			tp.textpanel->color.y=0;
			tp.textpanel->color.z=0;
			tp.textpanel->color.w=1;
		}

		if(tp.textpanel->mouseover && mouse.lUp){
			clearSelection();

			if(selectedObjects.find(node.object.object)==-1){
				selectedObjects.pushBack(node.object.object);
				if(selectedPolygons.size()<selectedObjects.size()){selectedPolygons.pushBack();}
				if(selectedEdges.size()<selectedObjects.size()){selectedEdges.pushBack();}
				if(selectedVertices.size()<selectedObjects.size()){selectedVertices.pushBack();}
			}
		}

	}else if(node.object.type==PANEL){
		nn=node.object.panel->name;
	}else if(node.object.type==NULL3D){
		nn=node.object.null->name;

		if(!node.object.null->visible){
			tp.textpanel->color.x=0.5;
			tp.textpanel->color.y=0.5;
			tp.textpanel->color.z=0.5;
			tp.textpanel->color.w=1;
		}

		if(selectedNulls.find(node.object.null)!=-1){
			tp.textpanel->color.x=1;
			tp.textpanel->color.y=0;
			tp.textpanel->color.z=0;
			tp.textpanel->color.w=1;
		}

		if(tp.textpanel->mouseover && mouse.lUp){
			clearSelection();

			if(selectedNulls.find(node.object.null)==-1){
				selectedNulls.pushBack(node.object.null);
			}
		}
	}else if(node.object.type==LINE){
		nn=node.object.line->name;
	}else if(node.object.type==INSTANCE){
		nn=node.object.instance->name;
	}else if(node.object.type==LIGHT){
		nn=node.object.light->name;

		if(!node.object.light->visible){
			tp.textpanel->color.x=0.5;
			tp.textpanel->color.y=0.5;
			tp.textpanel->color.z=0.5;
			tp.textpanel->color.w=1;
		}

		if(selectedLights.find(node.object.light)!=-1){
			tp.textpanel->color.x=1;
			tp.textpanel->color.y=0;
			tp.textpanel->color.z=0;
			tp.textpanel->color.w=1;
		}

		if(tp.textpanel->mouseover && mouse.lUp){
			clearSelection();

			if(selectedLights.find(node.object.light)==-1){
				selectedLights.pushBack(node.object.light);
			}
		}
	}else{
		
	}


	tp.textpanel->setText(nn);
	tp.textpanel->pos.x=explorerX;
	tp.textpanel->pos.y=explorerY;

	explorerX+=20;
	explorerY-=text.leading()+2;

	for(int i=0; i<node.children.size(); i++){
		p++;
		p=drawExplorerNode(node.children[i],p);
	}

	explorerX-=20;
	return p;
}

bool doDrawExplorer=false;


void Editor::drawExplorer(){

	if(deactivateGui("explorer")){
		doDrawExplorer=false;
	}else if(activateGui("explorer")){
		doDrawExplorer=true;
	}

	if(!doDrawExplorer){
		for(int i=0; i<explorerPanels.size(); i++){
			explorerPanels[i].textpanel->visible=false;
		}

		explorerFade.panel->visible=false;

		return;
	}

	glDisable(GL_SCISSOR_TEST);

	glColor4f(1,1,1,1);

	explorerY=-5;
	drawExplorerNode(level->sceneGraph.root,0);
	explorerFade.panel->visible=true;

}

void Editor::initExplorer(){
	explorerFade=add("explorerFade","panel");
	explorerFade.panel->pos.x=0;
	explorerFade.panel->pos.y=0;
	explorerFade.panel->pos.z=90;
	explorerFade.panel->color.x=0;
	explorerFade.panel->color.y=0;
	explorerFade.panel->color.z=0;
	explorerFade.panel->color.w=0.5;
	explorerFade.panel->percent=true;
	explorerFade.panel->scale.x=1;
	explorerFade.panel->scale.y=1;
	explorerFade.panel->visible=false;
}

#endif
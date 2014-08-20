#ifdef DEVELOPER

#include "editor.h"

#include "keyboard.h"
#include "mouse.h"
#include "conf.h"

ObjectAddress uvFade;
ObjectAddress texturePanel;

bool uvVisible=false;

String currentUvBmp="";

Array<ObjectAddress> uvEdges;
Array<ObjectAddress> uvPoints;
Array<ObjectAddress> uvPolygons;

void Editor::drawUv(){

	if(deactivateGui("uv")){

		uvVisible=false;
	}else if(activateGui("uv")){
		uvVisible=true;
		toolCommandsStart="Uv Tool\n\nmouse-l : Translate X\nmouse-m : Translate Y\nmouse-r : Translate Z";
	}

	

	if(selectedObjects.size()!=1){
		uvVisible=false;
	}

	if(uvVisible){
		uvFade.panel->visible=true;
		texturePanel.panel->visible=true;

		int tex=level->materials[selectedObjects[0]->material].textureDiffuse;

		if(tex!=-1){
			TextureImage* texture=&level->textures[tex];

			texturePanel.panel->scale.x=texture->x/float(1600);
			texturePanel.panel->scale.y=texture->y/float(1200);

			if(texturePanel.panel->scale.x>texturePanel.panel->scale.y){
				float osx=texturePanel.panel->scale.x;
				texturePanel.panel->scale.x=0.75;
				texturePanel.panel->scale.y*=texturePanel.panel->scale.x/osx;
			}else{
				float osy=texturePanel.panel->scale.y;
				texturePanel.panel->scale.y=0.75;
				texturePanel.panel->scale.x*=texturePanel.panel->scale.y/osy;
			}

			texturePanel.panel->pos.x=(1-texturePanel.panel->scale.x)/float(2);
			texturePanel.panel->pos.y=-(1-texturePanel.panel->scale.y)/float(2);
		}

		Vertex* v1;

		for(int i=0; i<selectedObjects[0]->getVertexCount(); i++){
			if(i>=uvPoints.size()){
				ObjectAddress l=add("uvEdges"+String(i),"panel");
				l.panel->percent=false;
				l.panel->keepAspectRatio=false;
				l.panel->visible=true;
				l.panel->color.x=0;
				l.panel->color.y=0;
				l.panel->color.z=1;
				l.panel->color.w=1;
				l.panel->scale.x=10;
				l.panel->scale.y=10;
				l.panel->pos.z=93;

				uvPoints.pushBack(l);
			}

			v1=&selectedObjects[0]->getVertexRaw(i);

			uvPoints[i].panel->visible=true;
			uvPoints[i].panel->pos.x=v1->uvLightmap.x*texturePanel.panel->pickerScale.x+texturePanel.panel->pickerPos.x-5;
			uvPoints[i].panel->pos.y=-v1->uvLightmap.y*texturePanel.panel->pickerScale.y+texturePanel.panel->pickerPos.y+5;

		}

		for(int i=0; i<selectedObjects.size(); i++){

			if(i>=uvEdges.size()){
				ObjectAddress p=add("uvPolygons"+String(i),"polygon2d");
				p.polygon2d->visible=true;
				p.polygon2d->stipple=false;
				p.polygon2d->color.x=0;
				p.polygon2d->color.y=0;
				p.polygon2d->color.z=1;
				p.polygon2d->color.w=1;
				p.polygon2d->pointSize=10;

				uvEdges.pushBack(p);
			}

			uvEdges[i].polygon2d->pos=texturePanel.panel->pos;
			uvEdges[i].polygon2d->scale=texturePanel.panel->scale;
			uvEdges[i].polygon2d->addPointVertexList(selectedObjects[i]->Vertices());
		}
		
		int pcnt=0;

		for(int i=0; i<selectedObjects[0]->getVertexCount(); i+=3){		
			pcnt++;
		}

		if(activateTool("translate","uv")){

		}

		if(toolActive("translate","uv")){

			Array<Vertex*> vlist;

			for(int p=0; p<selectedPolygons[0].size(); p++){

				Vertex* v1=&selectedObjects[0]->getVertexRaw(selectedPolygons[0][p]*3);
				Vertex* v2=&selectedObjects[0]->getVertexRaw(selectedPolygons[0][p]*3+1);
				Vertex* v3=&selectedObjects[0]->getVertexRaw(selectedPolygons[0][p]*3+2);

				vlist.pushBack(v1);
				vlist.pushBack(v2);
				vlist.pushBack(v3);
			}

			for(int v=0; v<vlist.size(); v++){
				if(mouse.l || mouse.r){
					vlist[v]->uvLightmap.x+=mouse.change_x*0.01;
				}

				if(mouse.l || mouse.m){
					vlist[v]->uvLightmap.y-=mouse.change_y*0.01;
				}
				
			}

			
			selectedObjects[0]->rebuildUVs();
		}

	}else{
		uvFade.panel->visible=false;
		texturePanel.panel->visible=false;

		for(int i=0; i<uvEdges.size(); i++){
			uvEdges[i].line2d->visible=false;
		}

		for(int i=0; i<uvPoints.size(); i++){
			uvPoints[i].panel->visible=false;
		}

		for(int i=0; i<uvPolygons.size(); i++){
			uvPolygons[i].polygon2d->visible=false;
		}
	}
}

void Editor::initUv(){
	uvFade=add("uvFade","panel");
	uvFade.panel->pos.x=0;
	uvFade.panel->pos.y=0;
	uvFade.panel->pos.z=90;
	uvFade.panel->color.x=0;
	uvFade.panel->color.y=0;
	uvFade.panel->color.z=0;
	uvFade.panel->color.w=0.5;
	uvFade.panel->percent=true;
	uvFade.panel->scale.x=1;
	uvFade.panel->scale.y=1;
	uvFade.panel->visible=false;

	texturePanel=add("texturePanel","panel");
	texturePanel.panel->pos.x=0.125;
	texturePanel.panel->pos.y=-0.125;
	texturePanel.panel->pos.x=0;
	texturePanel.panel->pos.y=0;
	texturePanel.panel->pos.z=91;
	texturePanel.panel->color.x=1;
	texturePanel.panel->color.y=1;
	texturePanel.panel->color.z=1;
	texturePanel.panel->color.w=0.5;
	texturePanel.panel->percent=true;
	texturePanel.panel->keepAspectRatio=true;
	texturePanel.panel->center=true;
	texturePanel.panel->scale.x=0.75;
	texturePanel.panel->scale.y=0.75;
	texturePanel.panel->smartCenter=float(1600)/float(1200);
	texturePanel.panel->visible=false;
	
}

#endif
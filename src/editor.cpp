#ifdef DEVELOPER

#include "editor.h"
#include "level.hpp"
#include "keyboard.h"
#include "Helperlib/HelperLibMath.hpp"
#include "Audio/Audio.h"
#include "mouse.h"
#include "picking.h"
#include "conf.h"
#include "lightmap.h"
#include "texturelibrary.h"
#include "draw.hpp"
#include "texture.hpp"
#include <fstream>
#include "os.h"

using namespace std;
bool cWasFrozen=false;
ObjectAddress editorCommands;
ObjectAddress toolCommands;
ObjectAddress stickyToolCommands;
ObjectAddress editorLeftBkg;
ObjectAddress editorRightBkg;
ObjectAddress editorTopBkg;
ObjectAddress editorBottomBkg;
ObjectAddress scriptBkg;
ObjectAddress scriptText;
ObjectAddress tooltipBkg;
ObjectAddress tooltipText;
ObjectAddress solomode;

ObjectAddress marqueeBkg;

Array<ObjectAddress> buttonsL;
Array<ObjectAddress> buttonsLB;
Array<ObjectAddress> buttonsLI;

Array<ObjectAddress> buttonsT;
Array<ObjectAddress> buttonsTB;
Array<ObjectAddress> buttonsTI;

Array<ObjectAddress> buttonsB;
Array<ObjectAddress> buttonsBB;
Array<ObjectAddress> buttonsBI;

Array<ObjectAddress> buttonsR;
Array<ObjectAddress> buttonsRB;
Array<ObjectAddress> buttonsRI;
Editor editor;
bool renderLMMode=false;
Array<Object*> selectedObjects;
Array<Light*> selectedLights;
Array<NullObject*> selectedNulls;
Map<String,IntVector2d> lmSizes;

bool oldEditorMouseVisible=false;
FloatVector4d oldEditorMouseColor;
int oldEditorMouseTextureId=-1;
FloatVector2d oldEditorMouseScale;
bool mouseVisibleHack=false;
bool editorButtonDownHack=false;
int statusTexId=-1;
bool lmSizesLoaded=false;
int mousetexid=-1;

Array<Array<int> > selectedPolygons;
Array<Array<int> > selectedEdges;
Array<Array<int> > selectedVertices;

bool editorLocal=false;
String editorCommandsStart;
String editorCommandsLocal;
String editorCommandsEnd;
String toolCommandsStart;
String toolCommandsSelected;

Array<Line3d*> editorLines;
Array<TextPanel*> editorTextpanels;
Array<Panel*> editorPanels;
Array<Line2d*> editorLines2d;
Array<Polygon2d*> editorPolygons2d;

ObjectAddress viewTitle;

void Editor::clearSelection(){
	selectedObjects.clear();
	selectedEdges.clear();
	selectedNulls.clear();
	selectedLights.clear();
}

void Editor::addButton(String name, void (*action)(), String icon,int panel){

	if(name==""){
		ObjectAddress button;
		button.panel=NULL;

		if(panel==0){
			buttonsL.pushBack(button);
			buttonsLB.pushBack(button);
			buttonsLI.pushBack(button);
		}

		return;
	}

	ObjectAddress button;

	if(panel==1){
		button=add("editorButtonR"+name,"panel");
	}else if(panel==0){
		button=add("editorButtonL"+name,"panel");
	}else if(panel==2){
		button=add("editorButtonT"+name,"panel");
	}else if(panel==3){
		button=add("editorButtonB"+name,"panel");
	}

	if(panel==1){
		button.panel->pos.x=float(1500);
		button.panel->pos.y=((float(0)-buttonsR.size()*50));
	}else if(panel==0){
		button.panel->pos.x=float(0);
		button.panel->pos.y=-buttonsL.size()*50;
	}else if(panel==2){
		button.panel->pos.x=float(50)+buttonsT.size()*50;
		button.panel->pos.y=0;
	}else if(panel==3){
		button.panel->pos.x=float(50)+buttonsB.size()*50;
		button.panel->pos.y=-float(1)+float(50)/float(1200);
	}
	
	button.panel->pos.z=52;
	button.panel->keepAspectRatio=true;
	button.panel->color.x=0.8;
	button.panel->color.y=0.8;
	button.panel->color.z=0.8;
	button.panel->scale.x=float(50);
	button.panel->scale.y=float(50);
	button.panel->percent=false;
	button.panel->visible=false;
	
	ObjectAddress buttonB;

	if(panel==1){
		buttonB=add("editorButtonRB"+String(buttonsR.size()),"panel");
	}else if(panel==0){
		buttonB=add("editorButtonLB"+String(buttonsL.size()),"panel");
	}else if(panel==2){
		buttonB=add("editorButtonTB"+String(buttonsT.size()),"panel");
	}else if(panel==3){
		buttonB=add("editorButtonBB"+String(buttonsB.size()),"panel");
	}

	if(panel==1){
		buttonB.panel->pos.x=float(1502);
		buttonB.panel->pos.y=((float(-2)-buttonsR.size()*50));
	}else if(panel==0){
		buttonB.panel->pos.x=float(2);
		buttonB.panel->pos.y=((float(-2)-buttonsL.size()*50));
	}else if(panel==2){
		buttonB.panel->pos.x=float(50+buttonsT.size()*50)+2;
		buttonB.panel->pos.y=float(-2);
	}else if(panel==3){
		buttonB.panel->pos.x=float(50+buttonsB.size()*50)+2;
		buttonB.panel->pos.y=float(0);
	}
	
	buttonB.panel->pos.z=53;
	buttonB.panel->keepAspectRatio=true;
	buttonB.panel->color.x=0.4;
	buttonB.panel->color.y=0.4;
	buttonB.panel->color.z=0.4;
	buttonB.panel->scale.x=float(46);
	buttonB.panel->scale.y=float(46);
	buttonB.panel->visible=false;
	buttonB.panel->percent=false;
	buttonB.panel->onMouseLUp=action;

	ObjectAddress buttonI;

	if(panel==1){
		buttonI=add("editorButtonRI"+String(buttonsR.size()),"panel");
	}else if(panel==0){
		buttonI=add("editorButtonLI"+String(buttonsL.size()),"panel");
	}else if(panel==2){
		buttonI=add("editorButtonTI"+String(buttonsT.size()),"panel");
	}else if(panel==3){
		buttonI=add("editorButtonBI"+String(buttonsB.size()),"panel");
	}

	if(panel==1){
		buttonI.panel->pos.x=float(1500);
		buttonI.panel->pos.y=((float(0)-buttonsR.size()*50));
	}else if(panel==0){
		buttonI.panel->pos.x=float(0);
		buttonI.panel->pos.y=-buttonsL.size()*50;
	}else if(panel==2){
		buttonI.panel->pos.x=float(50)+buttonsT.size()*50;
		buttonI.panel->pos.y=((float(0)));
	}else if(panel==3){
		buttonI.panel->pos.x=float(50)+buttonsB.size()*50;
		buttonI.panel->pos.y=((float(0)));
	}
	
	buttonI.panel->pos.z=54;
	buttonI.panel->keepAspectRatio=true;
	buttonI.panel->color.x=1;
	buttonI.panel->color.y=1;
	buttonI.panel->color.z=1;
	buttonI.panel->clickable=false;
	buttonI.panel->visible=false;
	buttonI.panel->percent=false;
	buttonI.panel->onMouseLUp=action;
	buttonI.panel->load(icon,0);
	buttonI.panel->scale.x=float(50);
	buttonI.panel->scale.y=float(50);

	if(panel==1){
		buttonsR.pushBack(button);
		buttonsRB.pushBack(buttonB);
		buttonsRI.pushBack(buttonI);
	}else if(panel==0){
		buttonsL.pushBack(button);
		buttonsLB.pushBack(buttonB);
		buttonsLI.pushBack(buttonI);
	}else if(panel==2){
		buttonsT.pushBack(button);
		buttonsTB.pushBack(buttonB);
		buttonsTI.pushBack(buttonI);
	}else if(panel==3){
		buttonsB.pushBack(button);
		buttonsBB.pushBack(buttonB);
		buttonsBI.pushBack(buttonI);
	}
}

void remakeLightmapsButtonAction(){
	renderLMMode=true;
	
}

void setTranslate(){
	editor.forceActive("translate","main");
}

void setRotate(){
	editor.forceActive("rotate","main");
}


void setScale(){
	editor.forceActive("scale","main");
}

void setNavigate(){
	editor.forceActive("navigate","main");
}


void setObjects(){
	editor.forceActive("selectobjects","main");
}

void frameAllAction(){
	editor.centerAll();
}

void setPolys(){
	editor.forceActive("selectpolygons","main");
}

void setEdges(){
	editor.forceActive("selectedges","main");
}

void setVertices(){
	editor.forceActive("selectvertices","main");
}

void setScript(){
	editor.scriptEditor=!editor.scriptEditor;
}

void saveAction(){
	editor.saveLevel();
}

void buildFinal();

void buildAction(){
	editor.yescancel("Really build release version of project?",buildFinal);
}	

void openAction(){
	editor.showProjects();
}

void copyAction(){

}

void cutAction(){

}

void quitAction(){
	editor.quit();
}

void pasteAction(){

}

void deleteAction(){

}

void undoAction(){

	editor.undo.undo();
}

void redoAction(){

	editor.undo.redo();
}

void showMaterialEditor(){
	editor.forceGui("material",true);
}

void showExplorer(){
	editor.forceGui("explorer",true);
}

void expertAction(){
	editor.expertMode=!editor.expertMode;
}

void showUVEditor(){
	editor.forceGui("uv",true);
}

void showScriptEditor(){
	editor.forceGui("script",true);
}

void showProperties(){
	editor.forceGui("properties",true);
}


void frameSelectedAction(){
	editor.centerSelected();
}


void hideSelectedAction(){
	editor.hideSelection();
}


void localAction(){
	editor.forceGui("properties",true);
}


void globalAction(){
	editor.forceGui("properties",true);
}


void snapAction(){
	
}


void normalsAction(){
	conf->normals=!conf->normals;
}

void freeCameraAction(){
	editor.freeCamera();
}

void freeOrthographicCameraAction(){
	editor.freeOrthographicCamera();
}


void topCameraAction(){
	editor.topCamera();
}

void rightCameraAction(){
	editor.rightCamera();
}

void frontCameraAction(){
	editor.frontCamera();
}


void soloAction(){
	editor.toggleSolo();
}

void selectAllAction(){
	editor.selectAll();
}

void wireframeAction(){

}

void Editor::initGrid(){
	int x=20;
	int z=20;

	for(int i=0; i<x*2+1; i++){
		ObjectAddress g=add("editorGridX"+String(i),"line");
		g.line->pos.x=-x+i;
		g.line->pos.z=-z;
		g.line->pos.y=0;
		g.line->scale.x=0;
		g.line->scale.y=0;
		g.line->scale.z=z*2;
		g.line->visible=false;

		if(i==x){
			g.line->color.x=1;
		}else{
			g.line->color.x=0.25;
			g.line->color.y=0.25;
			g.line->color.z=0.25;
		}

		grid.pushBack(g);
	}

	for(int i=0; i<z*2+1; i++){
		ObjectAddress g=add("editorGridZ"+String(i),"line");
		g.line->pos.x=-x;
		g.line->pos.z=-z+i;
		g.line->pos.y=0;
		g.line->scale.x=x*2;
		g.line->scale.y=0;
		g.line->scale.z=0;
		g.line->visible=false;

		if(i==x){
			g.line->color.x=1;
		}else{
			g.line->color.x=0.25;
			g.line->color.y=0.25;
			g.line->color.z=0.25;
		}

		grid.pushBack(g);
	}
}

bool Editor::init(){
	projectsInit();
	initAlerts();
	initUv();
	initScript();
	initExplorer();
	initProperties();
	initMaterial();
	initNew();

	buttonsL.clear();
	buttonsLB.clear();
	buttonsLI.clear();
	
	buttonsR.clear();
	buttonsRB.clear();
	buttonsRI.clear();
	
	buttonsT.clear();
	buttonsTB.clear();
	buttonsTI.clear();
	
	buttonsB.clear();
	buttonsBB.clear();
	buttonsBI.clear();

	//DO NOT use os().path here, because loadLibrary does it for us!
	String developerlmtpath="developer.lmt";

	if(os().fileExists("../tools/developer.lmt")){
		developerlmtpath="../tools/developer.lmt";
	}else if(os().fileExists("../../tools/developer.lmt")){
		developerlmtpath="../../tools/developer.lmt";
	}else{
		if(!os().fileExists("developer.lmt")){
			os().error("Error: developer.lmt not found!");
		}
	}

	if(!textureLibrary.libraryLoaded(developerlmtpath)){
		textureLibrary.loadLibrary(developerlmtpath);
	}

	initGrid();

	quad=gluNewQuadric();

	editorLeftBkg=add("editorLeftBkg","panel");
	editorLeftBkg.panel->pos.x=0;
	editorLeftBkg.panel->pos.y=0;
	editorLeftBkg.panel->color.x=0.2;
	editorLeftBkg.panel->color.y=0.2;
	editorLeftBkg.panel->color.z=0.2;
	editorLeftBkg.panel->scale.x=float(50);
	editorLeftBkg.panel->scale.y=float(1200);
	editorLeftBkg.panel->percent=false;
	editorLeftBkg.panel->visible=false;

	addButton("navigate",setNavigate,"camera.tga",0);
	addButton("Select Objects",setObjects,"objects.tga",0);
	addButton("Select Edges",setEdges,"edges.tga",0);
	addButton("Select Polygons",setPolys,"polys.tga",0);
	addButton("Select Vertices",setVertices,"points.tga",0);
	addButton("translate",setTranslate,"pos.tga",0);
	addButton("rotate",setRotate,"rot.tga",0);
	addButton("scale",setScale,"scale.tga",0);
	
	addButton("",showMaterialEditor,"material.tga",0);

	addButton("Properties",showProperties,"properties.tga",0);
	addButton("Material Editor",showMaterialEditor,"material.tga",0);
	addButton("UV Editor",showUVEditor,"uv.tga",0);
	addButton("Script Editor",showScriptEditor,"script.tga",0);
	addButton("Explorer",showExplorer,"explorer.tga",0);

	editorRightBkg=add("editorRightBkg","panel");
	editorRightBkg.panel->pos.x=float(1500);
	editorRightBkg.panel->pos.y=0;
	editorRightBkg.panel->color.x=0.2;
	editorRightBkg.panel->color.y=0.2;
	editorRightBkg.panel->color.z=0.2;
	editorRightBkg.panel->scale.x=float(50);
	editorRightBkg.panel->scale.y=float(1200);
	editorRightBkg.panel->percent=false;
	editorRightBkg.panel->visible=false;

	addButton("Add Cube",addCubeAction,"cube.tga",1);
	addButton("Add Grid",addGridAction,"grid.tga",1);
	addButton("Add Point Light",addPointLightAction,"light.tga",1);
	addButton("Add Spotlight",addSpotlightAction,"spotlight.tga",1);
	addButton("Rebuild Lightmap",remakeLightmapsButtonAction,"ls.tga",1);
	addButton("Delete Selected",deleteAction,"delete.tga",1);

	editorTopBkg=add("editorTopBkg","panel");
	editorTopBkg.panel->pos.x=float(50);
	editorTopBkg.panel->pos.y=0;
	editorTopBkg.panel->color.x=0.2;
	editorTopBkg.panel->color.y=0.2;
	editorTopBkg.panel->color.z=0.2;
	editorTopBkg.panel->scale.x=float(1600);
	editorTopBkg.panel->scale.y=float(50);
	editorTopBkg.panel->percent=false;
	editorTopBkg.panel->visible=false;
	
	addButton("Open",openAction,"open.tga",2);
	addButton("Save",saveAction,"save.tga",2);
	addButton("Build Project",buildAction,"cd.tga",2);
	addButton("Copy",copyAction,"copy.tga",2);
	addButton("Paste",pasteAction,"paste.tga",2);
	addButton("Cut",cutAction,"cut.tga",2);
	addButton("Undo",undoAction,"undo.tga",2);
	addButton("Redo",redoAction,"redo.tga",2);
	
	
	addButton("Quit",quitAction,"quit.tga",2);
	
	editorBottomBkg=add("editorBottomBkg","panel");
	editorBottomBkg.panel->pos.x=float(50);
	editorBottomBkg.panel->pos.y=-700;
	editorBottomBkg.panel->color.x=0.2;
	editorBottomBkg.panel->color.y=0.2;
	editorBottomBkg.panel->color.z=0.2;
	editorBottomBkg.panel->scale.x=float(1600);
	editorBottomBkg.panel->scale.y=float(50);
	editorBottomBkg.panel->percent=false;
	editorBottomBkg.panel->visible=false;
	
	addButton("Frame Selected",frameSelectedAction,"frameselection.tga",3);
	addButton("Frame All",frameAllAction,"frameall.tga",3);
	addButton("Hide Selected",hideSelectedAction,"hide.tga",3);
	addButton("Select All",selectAllAction,"selectall.tga",3);
	addButton("Local Mode",localAction,"local.tga",3);
	addButton("Global Mode",globalAction,"global.tga",3);
	addButton("Snap",snapAction,"snap.tga",3);
	addButton("Show Normals",normalsAction,"normals.tga",3);
	addButton("Wireframe",wireframeAction,"wireframe.tga",3);
	addButton("Solo",soloAction,"solo.tga",3);
	addButton("Expert Mode",expertAction,"expert.tga",3);
	addButton("Free Camera",freeCameraAction,"freecamera.tga",3);
	addButton("Free Orthographic Camera",freeOrthographicCameraAction,"freeorthographiccamera.tga",3);
	addButton("Top Camera",topCameraAction,"topcamera.tga",3);
	addButton("Front Camera",frontCameraAction,"frontcamera.tga",3);
	addButton("Right Camera",rightCameraAction,"rightcamera.tga",3);
	
	
	
	tooltipText=add("tooltipText","textpanel");
	tooltipText.textpanel->pos.x=float(110)/float(1600);
	tooltipText.textpanel->pos.y=-float(960)/float(1200);
	tooltipText.textpanel->pos.z=70;
	tooltipText.textpanel->color.x=1.0;
	tooltipText.textpanel->color.y=1.0;
	tooltipText.textpanel->color.z=1.0;
	tooltipText.textpanel->color.w=1.0;
	tooltipText.textpanel->setText("translate");
	tooltipText.textpanel->font=0;
	tooltipText.textpanel->visible=false;
	tooltipText.textpanel->percent=false;

	tooltipBkg=add("tooltipBkg","panel");
	tooltipBkg.panel->pos.x=float(100);
	tooltipBkg.panel->pos.y=-float(950);
	tooltipBkg.panel->pos.z=60;
	tooltipBkg.panel->color.x=0.2;
	tooltipBkg.panel->color.y=0.2;
	tooltipBkg.panel->color.z=0.2;
	tooltipBkg.panel->scale.x=float(1400);
	tooltipBkg.panel->scale.y=float(150);
	tooltipBkg.panel->visible=false;
	tooltipBkg.panel->percent=false;

	scriptBkg=add("scriptBkg","panel");
	scriptBkg.panel->pos.x=float(100)/float(1600);
	scriptBkg.panel->pos.y=-float(950)/float(1200);
	scriptBkg.panel->color.x=0.2;
	scriptBkg.panel->color.y=0.25;
	scriptBkg.panel->color.z=0.2;
	scriptBkg.panel->scale.x=float(1400)/float(1600);
	scriptBkg.panel->scale.y=float(150)/float(1200);
	scriptBkg.panel->smartCenter=float(1600)/float(1200);
	scriptBkg.panel->visible=false;

	scriptText=add("scriptText","textpanel");
	scriptText.textpanel->pos.x=float(110)/float(1600);
	scriptText.textpanel->pos.y=-float(960)/float(1200);
	scriptText.textpanel->pos.z=54;
	scriptText.textpanel->color.x=1.0;
	scriptText.textpanel->color.y=1.0;
	scriptText.textpanel->color.z=1.0;
	scriptText.textpanel->color.w=1.0;
	scriptText.textpanel->setText("[script]");
	scriptText.textpanel->scale.x=(scriptText.textpanel->scale.x/float(1600))*float(1.6);
	scriptText.textpanel->scale.y=(scriptText.textpanel->scale.y/float(1200))*float(1.6);
	scriptText.textpanel->smartCenter=float(1600)/float(1200);
	scriptText.textpanel->visible=false;
	scriptText.textpanel->percent=true;

	toolCommands=add("toolCommands","textpanel");
	toolCommands.textpanel->pos.x=float(60);
	toolCommands.textpanel->pos.y=float(-10);
	toolCommands.textpanel->pos.y=float(-40)-50;
	toolCommands.textpanel->color.w=0.5;
	toolCommands.textpanel->visible=false;
	toolCommands.textpanel->percent=false;
	toolCommands.textpanel->keepAspectRatio=true;
	toolCommands.textpanel->font=0;
	toolCommandsStart="Select Tool\n\nmouse-l : Select Objects";
	toolCommandsSelected="\n\n\\ : New Edge\nn : New Polygon\n\n";
	toolCommands.textpanel->align="right";

	stickyToolCommands=add("stickyToolCommands","textpanel");
	stickyToolCommands.textpanel->pos.x=float(60);
	stickyToolCommands.textpanel->pos.y=float(-200)-50;
	stickyToolCommands.textpanel->color.x=0;
	stickyToolCommands.textpanel->color.y=1;
	stickyToolCommands.textpanel->color.z=1;
	stickyToolCommands.textpanel->color.w=0.5;
	stickyToolCommands.textpanel->visible=false;
	stickyToolCommands.textpanel->percent=false;
	stickyToolCommands.textpanel->font=0;
	stickyToolCommands.textpanel->keepAspectRatio=true;
	stickyToolCommands.textpanel->setText(toolCommands.textpanel->storedtext);
	stickyToolCommands.textpanel->align="right";

	solomode=add("solomode","textpanel");
	solomode.textpanel->pos.x=float(60);
	solomode.textpanel->pos.y=float(-200);
	solomode.textpanel->pos.z=10;
	solomode.textpanel->color.x=0;
	solomode.textpanel->color.y=1;
	solomode.textpanel->color.z=1;
	solomode.textpanel->color.w=1;
	solomode.textpanel->visible=false;
	solomode.textpanel->percent=false;
	solomode.textpanel->keepAspectRatio=true;
	solomode.textpanel->setText("Solo Mode");

	marqueeBkg=add("marqueeBkg","panel");
	marqueeBkg.panel->pos.x=0;
	marqueeBkg.panel->pos.y=0;
	marqueeBkg.panel->pos.z=10;
	marqueeBkg.panel->color.x=1;
	marqueeBkg.panel->color.y=1;
	marqueeBkg.panel->color.z=1;
	marqueeBkg.panel->color.w=0.2;
	marqueeBkg.panel->scale.x=float(1400)/float(1600);
	marqueeBkg.panel->scale.y=float(150)/float(1200);
	marqueeBkg.panel->visible=false;

	viewTitle=add("viewTitle","textpanel");
	viewTitle.textpanel->pos.x=float(60);
	viewTitle.textpanel->pos.y=float(-60);
	viewTitle.textpanel->pos.z=10;
	viewTitle.textpanel->color.x=1;
	viewTitle.textpanel->color.y=1;
	viewTitle.textpanel->color.z=1;
	viewTitle.textpanel->color.w=0.5f;
	viewTitle.textpanel->visible=false;
	viewTitle.textpanel->percent=false;
	viewTitle.textpanel->font=0;
	viewTitle.textpanel->keepAspectRatio=true;
	viewTitle.textpanel->setText("Free Camera");

	marquee=false;

	return true;
}

static int luaEditorSelect(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		return 0;

	}

	String objs;
	if(args[0].type==SCRIPT_STRING_STORED){
		objs=args[0].value.ss;
	}

	if(level->objectKey.find(objs)){
		Object* obj=level->objectKey[objs];

		selectedObjects.pushBack(obj);
		selectedPolygons.pushBack();
		selectedEdges.pushBack();
		selectedVertices.pushBack();
	}else{
		console().write("obj not found to select");
	}

	return 0;
}

static int luaEditorAdd(lua_State *L){

	Array<LuaType> args=script().getArgs(L);

	if(args.size()!=1){
		return 0;

	}

	String objs;
	if(args[0].type==SCRIPT_STRING_STORED){
		objs=args[0].value.ss;
	}

	if(objs=="cube"){
		addCubeAction();
	}else if(objs=="light"){
		addPointLightAction();
	}else if(objs=="spotlight"){
		addSpotlightAction();
	}else{
		console().write("BADDEDADD");
	}

	return 0;
}


static int luaEditorDeselectAll(lua_State *L){
	editor.clearSelection();

	return 0;
}

bool Editor::scriptInit(){
	LuaType* luaa=script().add("editor");
	

	script().add("visible",&visible,luaa);
	script().add("select",&luaEditorSelect,luaa);
	script().add("deselectAll",&luaEditorDeselectAll,luaa);
	script().add("add",&luaEditorAdd,luaa);

	return true;
}


void Editor::saveLightmapSizes(){
	String fdir=level->file.getRelativeNoExtension();

	String fname=fdir+"_lightmapsizes.dat";

	ofstream out;
	out.open(fname.c_str(),ios::binary|ios::out);

	char head[]="LMD1";
	out.write(head,4);

	Array<String> keys=lmSizes.indices();

	unsigned int c=keys.size();
	out.write((char*)&c,4);

	for(int i=0; i<keys.size(); i++){
		keys[i].write(&out);
		out.write((char*)&lmSizes[keys[i]].x,4);
		out.write((char*)&lmSizes[keys[i]].y,4);
	}

	out.close();
}


void Editor::loadLightmapSizes(){

	if(!lmSizesLoaded){
	
		ifstream in;

		String fdir=level->file.getRelativeNoExtension();
		String fname=fdir+"_lightmapsizes.dat";

		in.open(fname.c_str(),ios::binary|ios::in);

		if(in==NULL){
			return;
		}

		char head[4];
		in.read(head,4);

		unsigned int c;
		in.read((char*)&c,4);

		for(int i=0; i<c; i++){
			String key;
			key.read(&in);
			unsigned int x;
			unsigned int y;
			in.read((char*)&x,4);
			in.read((char*)&y,4);

			IntVector2d jor;
			jor.x=x;
			jor.y=y;
			lmSizes[key]=jor;
		}

		in.close();

		lmSizesLoaded=true;
	}
}


void Editor::show(){
	visible=true;
}

Editor::Editor(){
	visible=false;
	scriptEditor=false;

	editorGui="main";

	topZoom=60;
	expertMode=0;
	cameraDistance=10;

	stickywatch=false;

	hideGUIs=false;

	monoSmall=-1;
	monoLarge=-1;

	cameraMode="free";
}

void Editor::preCamera(){
	if(visible){
		cWasFrozen=level->camera->freeze;
		level->camera->freeze=true;
	}
}

void Editor::postCamera(){
	if(visible){
		level->camera->freeze=cWasFrozen;
	}
}

ObjectAddress Editor::add(String name, String type){

	if(type=="line" || type=="Line"){

		Line3d* t=new Line3d;
		t->name=name;
		editorLines.pushBack(t);

		ObjectAddress a;
		a.type=LINE;
		a.line=t;

		return a;

	}else if(type=="textpanel" || type=="TextPanel"){

		TextPanel* t=new TextPanel;
		t->name=name;
		editorTextpanels.pushBack(t);

		ObjectAddress ta;
		ta.type=TEXTPANEL;
		ta.textpanel=t;

		return ta;

	}else if(type=="panel" || type=="Panel"){

		Panel* p=new Panel;
		p->name=name;
		editorPanels.pushBack(p);

		ObjectAddress t;
		t.type=PANEL;
		t.panel=p;


		return t;

	}else if(type=="line2d" || type=="Line2d"){
		Line2d* t=new Line2d;
		t->name=name;
		editorLines2d.pushBack(t);

		ObjectAddress a;
		a.type=LINE2D;
		a.line2d=t;

		return a;

	}else if(type=="polygon2d" || type=="Polygon2d"){
		Polygon2d* t=new Polygon2d;
		t->name=name;
		editorPolygons2d.pushBack(t);

		ObjectAddress a;
		a.type=POLYGON2D;
		a.polygon2d=t;

		return a;
	}

	ObjectAddress temp;
	temp.type=NULLOBJECT;
	return temp;
}


bool Editor::isVisible(){
	return visible;
}

#endif
#ifdef DEVELOPER

#include "editor.h"

#include "keyboard.h"
#include "mouse.h"
#include "level.hpp"
#include "draw.hpp"
#include "timeing.hpp"
#include "Helperlib/HelperLibMath.hpp"
#include "os.h"
#include <fstream>
#include "Helperlib/Log.hpp"

String Editor::previousTool(String gui){
	return editorToolPast[gui];
}

bool Editor::toolActive(String tool, String gui){
	return editorTool[gui]==tool && gui==editorGui;
}

bool Editor::activateEvent(String key, bool ctrl, String gui){
	if(editorGui!=gui){return false;}
	if(console().visible){return false;}

	if(keyboard.ctrl==ctrl){
		if(keyboard.currentStringDownExtended==key){

			return true;
		}

	}

	return false;
}

void Editor::toggleSolo(){
	if(soloedObjects.size()>0 || soloedLights.size()>0){
		solomode.textpanel->visible=false;

		soloedObjects.clear();
		soloedLights.clear();

		Array<Object*> oss=objectSoloState.indices();

		for(int i=0; i<oss.size(); i++){
			oss[i]->visible=objectSoloState[oss[i]];
		}

		Array<Light*> osl=lightSoloState.indices();

		for(int i=0; i<osl.size(); i++){
			osl[i]->visible=lightSoloState[osl[i]];
		}
	}else{
		solomode.textpanel->visible=true;

		if(selectedObjects.size()==0 && selectedLights.size()==0){
			editorError("Can't solo, no object selected");
		}else{
			for(int i=0; i<selectedObjects.size(); i++){
				soloedObjects.pushBack(selectedObjects[i]);
			}

			for(int i=0; i<selectedLights.size(); i++){
				soloedLights.pushBack(selectedLights[i]);
			}

			for(int i=0; i<level->objects.size(); i++){
				objectSoloState[level->objects[i]]=level->objects[i]->visible;

				if(soloedObjects.find(level->objects[i])==-1){
					level->objects[i]->visible=false;
				}
			}

			for(int i=0; i<level->lights.size(); i++){
				lightSoloState[level->lights[i]]=level->lights[i]->visible;

				if(soloedLights.find(level->lights[i])==-1){
					level->lights[i]->visible=false;
				}
			}

		}
	}
}

bool Editor::activateTool(String tool, String gui){

	if(engineTime.getTimerSingle("sticky")>0.4 && lastToolString!=""){
		stickyToolCommands.textpanel->pos.y=toolCommands.textpanel->pos.y-toolCommands.textpanel->scale.y-20;
		stickyToolCommands.textpanel->visible=true;
	}else{	
		stickyToolCommands.textpanel->visible=false;
	}

	if(editorGui!=gui){return false;}
	if(console().visible){return false;}

	bool act=false;

	if(tool==editorToolPast[gui]){
		if(editorTool[gui]=="translate" && keyboard.ctrl==false && keyboard.vUp){

			act=true;
		}else if(editorTool[gui]=="scale" && keyboard.ctrl==false && keyboard.xUp){
			act=true;
		}else if(editorTool[gui]=="rotate" && keyboard.ctrl==false && keyboard.cUp){
			act=true;
		}else if(editorTool[gui]=="navigate" && keyboard.ctrl==false && keyboard.sUp){
			act=true;
		}else if(editorTool[gui]=="selectobjects" && keyboard.ctrl==false && keyboard.space_up){
			act=true;
		}else if(editorTool[gui]=="selectpolygons" && keyboard.ctrl==false && keyboard.uUp){
			act=true;
		}else if(editorTool[gui]=="selectedges" && keyboard.ctrl==false && keyboard.eUp){
			act=true;
		}else if(editorTool[gui]=="selectvertices" && keyboard.ctrl==false && keyboard.tUp){
			act=true;
		}

		if(act){

			if(unstick()){
				lastToolString="";
				stickywatch=true;
				editorToolPast[gui]="";
				editorTool[gui]=tool;
				return true;
			}else{
				lastToolString="";
				return false;
			}
		}
	}

	if(tool==editorTool[gui]){return false;}

	
	if(tool=="translate" && keyboard.ctrl==false && keyboard.vDown){

		act=true;
	}else if(tool=="scale" && keyboard.ctrl==false && keyboard.xDown){
		act=true;
	}else if(tool=="rotate" && keyboard.ctrl==false && keyboard.cDown){
		act=true;
	}else if(tool=="navigate" && keyboard.ctrl==false && keyboard.sDown){
		act=true;
	}else if(tool=="selectobjects" && keyboard.ctrl==false && keyboard.space_down){
		act=true;
	}else if(tool=="selectpolygons" && keyboard.ctrl==false && keyboard.uDown){
		act=true;
	}else if(tool=="selectedges" && keyboard.ctrl==false && keyboard.eDown){
		act=true;
	}else if(tool=="selectvertices" && keyboard.ctrl==false && keyboard.tDown){
		act=true;
	}

	if(act){
		stickywatch=false;
		editorToolPast[gui]=editorTool[gui];
		editorTool[gui]=tool;
		startStickyTimer();
		lastToolString=toolCommands.textpanel->storedtext;
		stickyToolCommands.textpanel->setText(lastToolString);
		return true;
	}

	return false;
}

bool Editor::wasSticky(){
	return stickywatch;
}

void Editor::forceActive(String tool, String gui){

	editorTool[gui]=tool;
}

void saveLevelHL(Path file, bool developer){
	
	bool succ=level->save(file,developer);

	if(!succ){
		editor.editorError("Level save error");
	}else{
		editor.editorWarning("Level saved successfully!");
	}
}

void developerToRelease(String from, String to){

	bool t=conf->loadProgressive;
	conf->loadProgressive=false;

	if(level->name==""){
		level->name="editorLevel";
	}

	String currLevel=level->name;
	hideLevel(currLevel);

	newLevel();

	level->load(from);

	level->save(to,false);
	
	level->name="templevel";

	showLevel(currLevel);

	deleteLevel("templevel");

	conf->loadProgressive=t;
}


void buildFinal(){
	String folder="../builds";
	String toolsf="../tools";

	if(!os().folderExists(folder)){
		editor.editorError("Build Error: Builds folder doesn't exist");
	}

	if(!os().folderExists(toolsf)){
		editor.editorError("Build Error: Tools folder not found");
	}

	folder+="/";
	toolsf+="/";

	if(!os().folderExists(folder+editor.projectList[editor.loadedProject])){
		os().mkdir(folder+editor.projectList[editor.loadedProject]);
	}

	String exepath=folder+editor.projectList[editor.loadedProject]+"/"+editor.projectList[editor.loadedProject]+".exe";
	os().copy(toolsf+"engineRelease.exe",exepath);

	String target=folder+editor.projectList[editor.loadedProject]+"/";
	os().copy(toolsf+"wrap_oal.dll",target+"wrap_oal.dll");
	os().copy(toolsf+"OpenAL32.dll",target+"OpenAL32.dll");
	os().copy(toolsf+"socket",target+"socket");
	os().copy(toolsf+"shaders",target+"shaders");

	if(!os().folderExists(folder+editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel])){
		os().mkdir(folder+editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]);
	}

	String lvlpath=folder+editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]+"/"+editor.levelList[editor.loadedProject][editor.loadedLevel]+".lml";

	saveLevelHL(lvlpath,false);

	for(int i=0; i<editor.levelList[editor.loadedProject].size(); i++){

		if(i!=editor.loadedLevel){
			String from=editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][i]+"/"+editor.levelList[editor.loadedProject][i]+".lml";
			String to=folder+editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][i]+"/"+editor.levelList[editor.loadedProject][i]+".lml";

			if(!os().folderExists(folder+editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][i])){
				os().mkdir(folder+editor.projectList[editor.loadedProject]+"/"+editor.levelList[editor.loadedProject][i]);
			}

			developerToRelease(from,to);
		}
	}

	std::ofstream out;
	String lmsscript=editor.projectList[editor.loadedProject]+"/"+"lmscompiler.txt";
	out.open(lmsscript.c_str(),std::ios::binary|std::ios::out);

	String d="DEST:"+target+"\n";
	out.write(d.cStr(),d.size());

	for(int i=0; i<editor.scriptFiles.size(); i++){
		String d=editor.scriptFiles[i]+"\n";
		out.write(d.cStr(),d.size());
	}

	out.close();

	os().execute(toolsf+"lmscompiler.exe",lmsscript,os().workingDirectory);
	
	for(int i=0; i<editor.lmtpacker.size(); i++){
		os().execute(toolsf+"lmtpacker.exe",editor.lmtpacker[i],os().workingDirectory);
	}

	String batfile=editor.projectList[editor.loadedProject]+".exe "+editor.mainScriptFile;

	std::ofstream outt;
	outt.open((target+editor.projectList[editor.loadedProject]+".bat").c_str(),std::ios::binary|std::ios::out);
	outt.write(batfile.cStr(),batfile.size());
	outt.close();

	for(int i=0; i<editor.misc.size(); i++){
		std::ifstream in;
		in.open((editor.projectList[editor.loadedProject]+"/"+editor.misc[i]).c_str(),std::ios::in);

		std::string line;
		
		while(getline(in,line)){

			os().copy(editor.projectList[editor.loadedProject]+"/"+line,target+line);
		}

		in.close();

	}

#ifdef _WIN32
	String projdir="C:\\Documents and Settings\\All Users\\Start Menu\\Programs\\Baja Engine\\Projects\\"+editor.projectList[editor.loadedProject];

	if(!os().folderExists(projdir)){
		os().mkdir(projdir);
	}

	String batfilef=projdir+"\\"+editor.projectList[editor.loadedProject]+".bat";

	if(!os().fileExists(batfilef)){
		std::ofstream outt;
		
		
		outt.open(batfilef.c_str(),std::ios::out);

		String batsrc="cd "+os().workingDirectory+"\n";
		batsrc+="cd "+target+"\n";
		batsrc+=batfile;
		outt.write(batsrc.cStr(),batsrc.size());

		outt.close();
	}

#endif

}


void Editor::saveLevel(){
	saveLevelHL(level->file,true);
}

#define EPSILON 0.0001f

int findAVertexEpsilon(int selobj, int id){
	Vertex* v=&selectedObjects[selobj]->getVertexRaw(id);

	float ab=v->pos.x-EPSILON;


	for(int i=0; i<selectedObjects[selobj]->getVertexCount(); i++){

		if(selectedObjects[selobj]->getVertexRaw(i).pos.x==ab &&
			selectedObjects[selobj]->getVertexRaw(i).pos.y==v->pos.y &&
			selectedObjects[selobj]->getVertexRaw(i).pos.z==v->pos.z && i!=id){

			return i;
		}
	}

	return -1;
}
	
void Editor::deleteSelection(){

	if(selectedObjects.size()>0 || selectedLights.size()>0  || selectedNulls.size()>0){
		bool spoly=false;

		for(int i=0; i<selectedObjects.size(); i++){
			if(selectedPolygons[i].size()>0){
				spoly=true;
			}
		}

		if(spoly){
			for(int i=0; i<selectedObjects.size(); i++){
				for(int p=0; p<selectedPolygons[i].size(); p++){

					
					for(int j=selectedPolygons[i][p]*3; j<selectedObjects[i]->getVertexCount()-3; j+=3){
						selectedObjects[i]->getVertexRaw(j)=selectedObjects[i]->getVertexRaw(j+3);
						selectedObjects[i]->getVertexRaw(j+1)=selectedObjects[i]->getVertexRaw(j+4);
						selectedObjects[i]->getVertexRaw(j+2)=selectedObjects[i]->getVertexRaw(j+5);
					}

					if(selectedObjects[i]->getVertexCount()>3){
						if(selectedPolygons[i][p]*3==selectedObjects[i]->getVertexCount()-3){

							selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3-1).end=END_TRI;
						}
					}

					selectedObjects[i]->setVertexCount(selectedObjects[i]->getVertexCount()-3);

					for(int j=p+1; j<selectedPolygons[i].size(); j++){
						if(selectedPolygons[i][j]>selectedPolygons[i][p]){
							selectedPolygons[i][j]--;
						}
					}
				}

				selectedPolygons[i].clear();
			}

			
			rebuildSelection();
		}else{
			for(int i=0; i<selectedObjects.size(); i++){
				level->remove(selectedObjects[i]->name);
			}

			for(int i=0; i<selectedLights.size(); i++){
				level->remove(selectedLights[i]->name);
			}

			for(int i=0; i<selectedNulls.size(); i++){
				level->remove(selectedNulls[i]->name);
			}

			selectedNulls.clear();
			selectedObjects.clear();
			selectedLights.clear();
		}
	}else{
		editorError("No objects selected to delete");
	}
}

void Editor::duplicatePolygon(){
	Array<int> edgeFrom;
	Array<int> edgeTo;
	Array<FloatVector3d> sharedVertices;
	Array<Array<int> > sharedVerticesC;	//polygon indices
	Array<int> vid;
	Array<int> vidI;

	FloatVector3d ta,tb,tc;
	FloatVector3d *pa,*pb,*pc;
	int ia,ib,ic;
	Vertex v;

	for(int i=0; i<selectedObjects.size(); i++){

		edgeFrom.clear();
		edgeTo.clear();
		sharedVertices.clear();
		sharedVerticesC.clear();
		vid.clear();
		vidI.clear();

		for(int p=0; p<selectedPolygons[i].size(); p++){
			pa=&selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).pos;
			pb=&selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).pos;
			pc=&selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).pos;
		
			pa->x+=EPSILON;
			pb->x+=EPSILON;
			pc->x+=EPSILON;
		}

		for(int p=0; p<selectedPolygons[i].size(); p++){
			ta=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).pos;
			tb=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).pos;
			tc=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).pos;

			int a=sharedVertices.find(ta);

			if(a==-1){
				sharedVertices.pushBack(ta);
				sharedVerticesC.pushBack();
				sharedVerticesC[sharedVerticesC.size()-1].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3);
				vidI.pushBack(sharedVertices.size()-1);

			}else{
				sharedVerticesC[a].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3);
				vidI.pushBack(a);
			}

			int b=sharedVertices.find(tb);

			if(b==-1){
				sharedVertices.pushBack(tb);
				sharedVerticesC.pushBack();
				sharedVerticesC[sharedVerticesC.size()-1].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+1);
				vidI.pushBack(sharedVertices.size()-1);
			}else{
				sharedVerticesC[b].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+1);
				vidI.pushBack(b);
			}

			int c=sharedVertices.find(tc);

			if(c==-1){
				sharedVertices.pushBack(tc);
				sharedVerticesC.pushBack();
				sharedVerticesC[sharedVerticesC.size()-1].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+2);
				vidI.pushBack(sharedVertices.size()-1);
			}else{
				sharedVerticesC[c].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+2);
				vidI.pushBack(c);
			}

		}

		for(int p=0; p<selectedPolygons[i].size(); p++){
			int oia=selectedPolygons[i][p]*3;
			int oib=selectedPolygons[i][p]*3+1;
			int oic=selectedPolygons[i][p]*3+2;

			ia=vid.find(oia);
			ia=vidI[ia];

			ib=vid.find(oib);	
			ib=vidI[ib];

			ic=vid.find(oic);
			ic=vidI[ic];

			/////////////////////////////
			int fnd=0;
			for(int r=0; r<sharedVerticesC[ia].size(); r++){
				if(sharedVerticesC[ib].find(sharedVerticesC[ia][r])!=-1){fnd++;}
			}

			if(fnd<2){
				edgeFrom.pushBack(oia);
				edgeFrom.pushBack(oib);
		
				edgeTo.pushBack(findAVertexEpsilon(i,oia));
				edgeTo.pushBack(findAVertexEpsilon(i,oib));
			}
		

			/////////////////////////////
			fnd=0;
			for(int r=0; r<sharedVerticesC[ib].size(); r++){
				if(sharedVerticesC[ic].find(sharedVerticesC[ib][r])!=-1){fnd++;}
			}

			if(fnd<2){
				edgeFrom.pushBack(oib);
				edgeFrom.pushBack(oic);

				edgeTo.pushBack(findAVertexEpsilon(i,oib));
				edgeTo.pushBack(findAVertexEpsilon(i,oic));
			}
		
			/////////////////////////////
			fnd=0;
			for(int r=0; r<sharedVerticesC[ic].size(); r++){
				if(sharedVerticesC[ia].find(sharedVerticesC[ic][r])!=-1){fnd++;}
			}

			if(fnd<2){
				edgeFrom.pushBack(oic);
				edgeFrom.pushBack(oia);

				edgeTo.pushBack(findAVertexEpsilon(i,oic));
				edgeTo.pushBack(findAVertexEpsilon(i,oia));
			}
		}

		
		for(int p=0; p<edgeFrom.size(); p+=2){
			selectedObjects[i]->getVertexRaw(selectedObjects[i]->getVertexCount()-1).end=0;

			selectedObjects[i]->setVertexCount(selectedObjects[i]->getVertexCount()+6);

			
			v.end=0;
			v=selectedObjects[i]->getVertexRaw(edgeFrom[p+1]);
			selectedObjects[i]->addVertex(v);
			v=selectedObjects[i]->getVertexRaw(edgeFrom[p]);
			selectedObjects[i]->addVertex(v);
			v=selectedObjects[i]->getVertexRaw(edgeTo[p+1]);
			selectedObjects[i]->addVertex(v);

			v=selectedObjects[i]->getVertexRaw(edgeFrom[p]);
			selectedObjects[i]->addVertex(v);
			v=selectedObjects[i]->getVertexRaw(edgeTo[p]);
			selectedObjects[i]->addVertex(v);
			v=selectedObjects[i]->getVertexRaw(edgeTo[p+1]);
			v.end=END_TRI;
			selectedObjects[i]->addVertex(v);
		}
	}

	
}

void Editor::duplicateEdges(){
	//interior edges

	//exterior edges - not connected to a polygon on one side
	Array< Array<int> > extEdges;


	for(int i=0; i<selectedObjects.size(); i++){
		extEdges.pushBack();

		for(int e=0; e<selectedEdges[i].size(); e++){

			FloatVector3d v1,v2;

			if(e%3 == 0){
				v1=selectedObjects[i]->getVertexRaw(e).pos;
				v2=selectedObjects[i]->getVertexRaw(e+1).pos;
			}else if(e%3 == 1){
				v1=selectedObjects[i]->getVertexRaw(e).pos;
				v2=selectedObjects[i]->getVertexRaw(e+1).pos;
			}else if(e%3 == 2){
				v1=selectedObjects[i]->getVertexRaw(e).pos;
				v2=selectedObjects[i]->getVertexRaw(e-2).pos;
			}

			extEdges[i].pushBack(selectedEdges[i][e]);
		}
	}

	for(int i=0; i<selectedObjects.size(); i++){

		selectedEdges[i].clear();

		for(int m=0; m<extEdges[i].size(); m++){

			FloatVector3d v1,v2;

			int e=extEdges[i][m];

			if(e%3 == 0){
				v1=selectedObjects[i]->getVertexRaw(e).pos;
				v2=selectedObjects[i]->getVertexRaw(e+1).pos;
			}else if(e%3 == 1){
				v1=selectedObjects[i]->getVertexRaw(e).pos;
				v2=selectedObjects[i]->getVertexRaw(e+1).pos;
			}else if(e%3 == 2){
				v1=selectedObjects[i]->getVertexRaw(e).pos;
				v2=selectedObjects[i]->getVertexRaw(e-2).pos;
			}

			selectedObjects[i]->getVertexRaw(selectedObjects[i]->getVertexCount()-1).end=0;

			Vertex v;
			v.pos=v1;
			selectedObjects[i]->addVertex(v);

			v.pos=FloatVector3d(v1.x+EPSILON,v1.y+EPSILON,v1.z+EPSILON);
			selectedObjects[i]->addVertex(v);

			v.pos=v2;
			selectedObjects[i]->addVertex(v);

			


			/////////////
			v.pos=v2;
			selectedObjects[i]->addVertex(v);

			
			v.pos=FloatVector3d(v1.x+EPSILON,v1.y+EPSILON,v1.z+EPSILON);
			selectedObjects[i]->addVertex(v);

			v.pos=FloatVector3d(v2.x+EPSILON,v2.y+EPSILON,v2.z+EPSILON);
			v.end=END_TRI;
			selectedObjects[i]->addVertex(v);

			selectedObjects[i]->setVertexCount(selectedObjects[i]->getVertexCount()+6);

			selectedEdges[i].pushBack(selectedObjects[i]->getVertexCount()-2);
		}
	}

	editor.editorWarning("Edge Duplicated");
}
 
float calculateBestBridge(int obj,Array<Array<int> >* edgeList,Array<int>* polylist,int a,int b){

	Map<int, Map<int, float> > connections;

	for(int i=0; i<(*edgeList)[a].size(); i++){
		FloatVector3d na,nb,ta,tb;
		int ina,inb,ita,itb;

		if(i+1==(*edgeList)[a].size()){
			na=selectedObjects[obj]->getVertexRaw((*edgeList)[a][i]).pos;
			nb=selectedObjects[obj]->getVertexRaw((*edgeList)[a][0]).pos;
			ina=i;
			inb=0;
		}else{
			na=selectedObjects[obj]->getVertexRaw((*edgeList)[a][i]).pos;
			nb=selectedObjects[obj]->getVertexRaw((*edgeList)[a][i+1]).pos;
			ina=i;
			inb=i+1;
		}

		int to=-1;
		float cost=0;

		for(int j=0; j<(*edgeList)[b].size(); j++){

			if(i+1==(*edgeList)[b].size()){
				ta=selectedObjects[obj]->getVertexRaw((*edgeList)[b][i]).pos;
				tb=selectedObjects[obj]->getVertexRaw((*edgeList)[b][0]).pos;
				ita=i;
				itb=0;
			}else{
				ta=selectedObjects[obj]->getVertexRaw((*edgeList)[b][i]).pos;
				tb=selectedObjects[obj]->getVertexRaw((*edgeList)[b][i+1]).pos;
				ita=i;
				itb=i+1;
			}

			float tcost=math.magnitude(math.crossProduct(tb-ta,nb-ta))*0.5f;
			tcost+=math.magnitude(math.crossProduct(tb-na,na-nb))*0.5f;

			if(to==-1 || tcost<cost){
				if(connections.find(a)){
					if(!connections[a].find(j)){
						to=j;
						cost=tcost;
						connections[a][j]=1;
					}

				}else{
					to=j;
					cost=tcost;
					connections[a][j]=1;
				}
			}
		}

		polylist->pushBack((*edgeList)[a][inb]);
		polylist->pushBack((*edgeList)[b][ita]);
		polylist->pushBack((*edgeList)[b][itb]);
		polylist->pushBack((*edgeList)[b][itb]);
		polylist->pushBack((*edgeList)[a][ina]);
		polylist->pushBack((*edgeList)[a][inb]);
	}

	return 0;
}


void Editor::bridgePolygons(){
	Array<int> edgeFrom;
	Array<FloatVector3d> sharedVertices;
	Array<Array<int> > sharedVerticesC;	//polygon indices
	Array<int> vid;
	Array<int> vidI;
	Array<Array<int> > edgeList;

	Map<int,Map<int, int> > cost;
	Map<int, Map< int, Array<int> > > polylist;

	FloatVector3d ta,tb,tc;
	int ia,ib,ic;
	Vertex v;

	for(int i=0; i<selectedObjects.size(); i++){

		edgeFrom.clear();
		sharedVertices.clear();
		sharedVerticesC.clear();
		vid.clear();
		vidI.clear();
		cost.clear();
		polylist.clear();
		edgeList.clear();
		for(int p=0; p<selectedPolygons[i].size(); p++){
			ta=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).pos;
			tb=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).pos;
			tc=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).pos;

			int a=sharedVertices.find(ta);

			if(a==-1){
				sharedVertices.pushBack(ta);
				sharedVerticesC.pushBack();
				sharedVerticesC[sharedVerticesC.size()-1].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3);
				vidI.pushBack(sharedVertices.size()-1);

			}else{
				sharedVerticesC[a].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3);
				vidI.pushBack(a);
			}

			int b=sharedVertices.find(tb);

			if(b==-1){
				sharedVertices.pushBack(tb);
				sharedVerticesC.pushBack();
				sharedVerticesC[sharedVerticesC.size()-1].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+1);
				vidI.pushBack(sharedVertices.size()-1);
			}else{
				sharedVerticesC[b].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+1);
				vidI.pushBack(b);
			}

			int c=sharedVertices.find(tc);

			if(c==-1){
				sharedVertices.pushBack(tc);
				sharedVerticesC.pushBack();
				sharedVerticesC[sharedVerticesC.size()-1].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+2);
				vidI.pushBack(sharedVertices.size()-1);
			}else{
				sharedVerticesC[c].pushBack(selectedPolygons[i][p]);
				vid.pushBack(selectedPolygons[i][p]*3+2);
				vidI.pushBack(c);
			}

		}

		for(int p=0; p<selectedPolygons[i].size(); p++){
			int oia=selectedPolygons[i][p]*3;
			int oib=selectedPolygons[i][p]*3+1;
			int oic=selectedPolygons[i][p]*3+2;

			ia=vid.find(oia);
			ia=vidI[ia];

			ib=vid.find(oib);	
			ib=vidI[ib];

			ic=vid.find(oic);
			ic=vidI[ic];

			/////////////////////////////
			int fnd=0;
			for(int r=0; r<sharedVerticesC[ia].size(); r++){
				if(sharedVerticesC[ib].find(sharedVerticesC[ia][r])!=-1){fnd++;}
			}

			if(fnd<2){
				edgeFrom.pushBack(oia);
				edgeFrom.pushBack(oib);
			}
		

			/////////////////////////////
			fnd=0;
			for(int r=0; r<sharedVerticesC[ib].size(); r++){
				if(sharedVerticesC[ic].find(sharedVerticesC[ib][r])!=-1){fnd++;}
			}

			if(fnd<2){
				edgeFrom.pushBack(oib);
				edgeFrom.pushBack(oic);
			}
		
			/////////////////////////////
			fnd=0;
			for(int r=0; r<sharedVerticesC[ic].size(); r++){
				if(sharedVerticesC[ia].find(sharedVerticesC[ic][r])!=-1){fnd++;}
			}

			if(fnd<2){
				edgeFrom.pushBack(oic);
				edgeFrom.pushBack(oia);
			}
		}

		//calc edge list

		for(int e=0; e<edgeFrom.size(); e+=2){

			bool fnd=false;

			for(int f=0; f<edgeList.size(); f++){
				if(edgeList[f][0]==edgeFrom[e]){
					edgeList[f].pushFront(edgeFrom[e+1]);
					fnd=true;
					break;
				}else if(edgeList[f][0]==edgeFrom[e+1]){
					edgeList[f].pushFront(edgeFrom[e]);
					fnd=true;
					break;
				}else if(edgeList[f][edgeList[f].size()-1]==edgeFrom[e]){
					edgeList[f].pushBack(edgeFrom[e]+1);
					fnd=true;
					break;
				}else if(edgeList[f][edgeList[f].size()-1]==edgeFrom[e+1]){
					edgeList[f].pushBack(edgeFrom[e]);
					fnd=true;
					break;
				}
			}

			if(!fnd){
				edgeList.pushBack();
				edgeList[edgeList.size()-1].pushBack(edgeFrom[e]);
				edgeList[edgeList.size()-1].pushBack(edgeFrom[e+1]);
			}
		}

		if(edgeList.size()<2){
			return;
		}

		//find

		int bridgecount=0;
		if(edgeList.size()%2==0){
			bridgecount=edgeList.size()/2;
		}else{
			bridgecount=(edgeList.size()-1)/2;
		}

		float* fcost=new float[bridgecount];
		
		for(int u=0; u<bridgecount; u++){fcost[u]=-1.0f;}
		int* from=new int[bridgecount];
		int* to=new int[bridgecount];

		int t=0;

		for(int a=0; a<edgeList.size(); a++){
			for(int b=0; b<edgeList.size(); b++){
				if(cost.find(a)){
					
					if(!cost[a].find(b)){
						cost[a][b]=calculateBestBridge(i,&edgeList,&polylist[a][b],a,b);
						cost[b][a]=cost[a][b];
					}
				}else{
					cost[a][b]=calculateBestBridge(i,&edgeList,&polylist[a][b],a,b);
					cost[b][a]=cost[a][b];
				}

				if(t<bridgecount || cost[a][b]<fcost[bridgecount-1]){
					for(int u=0; u<bridgecount; u++){
						if(cost[a][b]<fcost[u] || fcost[u]==-1.0f){
							//spawn
							fcost[u]=cost[a][b];
							from[u]=a;
							to[u]=b;
						}
					}

					t++;
				}
			}
		}
		

		selectedObjects[i]->getVertexRaw(selectedObjects[i]->getVertexCount()-1).end=0;

		for(int s=0; s<bridgecount; s++){

			Array<int>* plist=&polylist[from[s]][to[s]];

			selectedObjects[i]->setVertexCount(selectedObjects[i]->getVertexCount()+plist->size());

			for(int r=0; r<plist->size(); r++){
				selectedObjects[i]->addVertex(selectedObjects[i]->getVertexRaw((*plist)[r]));
			}
		}

		selectedObjects[i]->getVertexRaw(selectedObjects[i]->getVertexCount()-1).end=END_TRI;

	}
}

void Editor::invertNormals(){
	for(int i=0; i<selectedObjects.size(); i++){
		for(int p=0; p<selectedPolygons[i].size(); p++){
			selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).normal=-selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3).normal;
			selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).normal=-selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1).normal;
			selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).normal=-selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2).normal;

			Vertex v1=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3);
			Vertex v2=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1);
			Vertex v3=selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2);
			
			selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3)=v3;
			selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+1)=v2;
			selectedObjects[i]->getVertexRaw(selectedPolygons[i][p]*3+2)=v1;
		}

		selectedObjects[i]->dirty=true;
		selectedObjects[i]->listBuilt=false;
		selectedObjects[i]->listBuiltQuick=false;
		selectedObjects[i]->listBuiltQuickPolygon=false;
	}
}


bool Editor::activateGui(String gui){


	if(gui==editorGui){return false;}
	if(console().visible){return false;}

	bool act=false;

	

	if((gui=="uv" && keyboard.ctrl && keyboard.uDown) || (forceShowGui=="uv" && forceShowGui==gui)){
		if(selectedObjects.size()==0){
			editorError("Can't Edit UVs, No Object Selected");
			forceShowGui="";
			forceHideGui="";
		}else if(selectedObjects.size()>1){
			editorError("Can't Edit UVs, Multiple Objects Selected");
			forceShowGui="";
			forceHideGui="";
		}else{
			act=true;
		}
	}else if((gui=="script" && keyboard.ctrl && keyboard.rDown) || (forceShowGui=="script" && forceShowGui==gui)){
		act=true;
	}else if((gui=="explorer" && keyboard.ctrl && keyboard.eDown) || (forceShowGui=="explorer" && forceShowGui==gui)){
		act=true;
	}else if((gui=="properties" && keyboard.ctrl && keyboard.dDown) || (forceShowGui=="properties" && forceShowGui==gui)){
		if(selectedObjects.size()==0){
			editorError("Can't Edit Properties, No Object Selected");
			forceShowGui="";
			forceHideGui="";
		}else if(selectedObjects.size()>1){
			editorError("Can't Edit Properties, Multiple Objects Selected");
			forceShowGui="";
			forceHideGui="";
		}else{
			act=true; 
		}
	}else if((gui=="material" && keyboard.ctrl && keyboard.mDown) || (forceShowGui=="material" && forceShowGui==gui)){
		if(selectedObjects.size()==0){
			editorError("Can't Edit Material, No Object Selected");
			forceShowGui="";
			forceHideGui="";
		}else if(selectedObjects.size()>1){
			editorError("Can't Edit Material, Multiple Objects Selected");
			forceShowGui="";
			forceHideGui="";
		}else{
			act=true;
		}
	}else if((gui=="new" && keyboard.ctrl && keyboard.nDown) || (forceShowGui=="new" && forceShowGui==gui)){
		act=true;
	}

	if(act){
		editorGui=gui;
		forceShowGui="";
		forceHideGui="";
		startStickyTimer();
		return true;
	}

	return false;
}

void Editor::forceGui(String gui, bool active){
	if(active){
		forceShowGui=gui;
	}else{
		forceHideGui=editorGui;
	}
}

bool Editor::deactivateGui(String gui){

	if(hideGUIs){
		return true;
	}

	if(editorGui!=gui){return false;}
	if(console().visible){return false;}

	bool act=false;

	if(gui=="uv" && keyboard.ctrl && keyboard.uUp){
		act=true;
	}else if(gui=="script" && keyboard.ctrl && keyboard.rUp){
		act=true;
	}else if(gui=="explorer" && keyboard.ctrl && keyboard.eUp){
		act=true;
	}else if(gui=="properties" && keyboard.ctrl && keyboard.dUp){
		act=true;
	}else if(gui=="material" && keyboard.ctrl && keyboard.mUp){
		act=true;
	}else if(gui=="new" && keyboard.ctrl && keyboard.nUp){
		act=true;
	}

	if(act){
		if(unstick()){
			editorGui="main";
			return true;
		}
	}

	act=false;

	if(gui=="uv" && keyboard.ctrl && keyboard.uDown){
		act=true;
	}else if(gui=="explorer" && keyboard.ctrl && keyboard.eDown){
		act=true;	
	}else if(gui=="script" && keyboard.ctrl && keyboard.rDown){
		act=true;	
	}else if(gui=="properties" && keyboard.ctrl && keyboard.dDown){
		act=true;	
	}else if(gui=="material" && keyboard.ctrl && keyboard.mDown){
		act=true;	
	}else if(gui=="new" && keyboard.ctrl && keyboard.nDown){
		act=true;	
	}

	if(act){
		editorGui="main";
		return true;
	}

	return false;
}

void Editor::startStickyTimer(){
	engineTime.startTimerSingle("sticky");
}

bool Editor::unstick(){
	double t=engineTime.getTimerSingle("sticky");

	if(t>0.4){
		return true;
	}

	return false;
}


void saveexitcallback(){
	editor.saveLevel();
	exit(0);
}

void exitcallback(){

	exit(0);
}

void Editor::selectAll(){

	if(editor.toolActive("selectpolygons","main")){
		for(int i=0; i<selectedObjects.size(); i++){
			selectedPolygons[i].clear();

			for(int j=0; j*3<selectedObjects[i]->getVertexCount(); j++){
				selectedPolygons[i].pushBack(j);
			}
		}
	}else if(editor.toolActive("selectvertices","main")){
		for(int i=0; i<selectedObjects.size(); i++){
			selectedVertices[i].clear();

			for(int j=0; j<selectedObjects[i]->getVertexCount(); j++){
				selectedVertices[i].pushBack(j);
			}
		}
	}else if(editor.toolActive("selectedges","main")){
		for(int i=0; i<selectedObjects.size(); i++){
			selectedEdges[i].clear();

			for(int j=0; j<selectedObjects[i]->getVertexCount(); j++){
				selectedEdges[i].pushBack(j);
			}
		}
	}else{
		selectedObjects.clear();

		for(int i=0; i<level->objects.size(); i++){
			selectedObjects.pushBack(level->objects[i]);
		}

		while(selectedPolygons.size()<selectedObjects.size()){
			selectedPolygons.pushBack();
		}

		while(selectedEdges.size()<=selectedObjects.size()){
			selectedEdges.pushBack();
		}

		while(selectedVertices.size()<=selectedObjects.size()){
			selectedVertices.pushBack();
		}

		selectedLights.clear();

		for(int i=0; i<level->lights.size(); i++){
			selectedLights.pushBack(level->lights[i]);
		}
	}
}


void Editor::quit(){
	if(level->file.set() && levelList.size()>0 && projectList.size()>0){
		
		yesnocancel("Save level '"+levelList[loadedProject][loadedLevel]+"' in project '"+projectList[loadedProject]+"'?",saveexitcallback,exitcallback);
		//exit(0);

	}else{
		exit(0);
	}
}


FloatVector2d Editor::remapScreenWorld(float x, float y){

	if(editor.cameraMode=="top"){

		FloatVector2d f;
		f.x=x/float(conf->sizeX);

		f.x*=level->camera->fov*2;

		f.y=y;

		if(conf->widescreen){

			f.y/=conf->sizeX/conf->widescreenRatio;
		}else{
			f.y/=float(conf->sizeY);
		}

		f.y*=level->camera->fov*float(2)/conf->widescreenRatio;

		return f;
		
	}
	
	return FloatVector2d();
}

void Editor::tools(){
	if(toolActive("translate","main") && mouse.m){
	
		
		for(int i=0; i<selectedObjects.size(); i++){

			if(selectedPolygons[i].size()>0){

				movePolygons(i,0,mouse.percent_change_y*50,0);
			}else if(selectedEdges[i].size()>0){
				moveEdges(i,0,mouse.percent_change_y*50,0);
			}else if(selectedVertices[i].size()>0){
				moveVertices(i,0,mouse.percent_change_y*50,0);
			}else{
				selectedObjects[i]->pos.y+=mouse.percent_change_y*50;
			}
		}

		for(int i=0; i<selectedLights.size(); i++){
			selectedLights[i]->pos.y+=mouse.percent_change_y*50;
		}
	}

	if(toolActive("translate","main") && (mouse.r || mouse.l && cameraMode=="top")){

		float change=mouse.percent_change_x*50;

		

		for(int i=0; i<selectedObjects.size(); i++){

			if(cameraMode=="top"){
		
				change=-(remapScreenWorld(mouse.change_x,mouse.change_y)).y;
				change/=selectedObjects[i]->scale.z;
			}

			if(selectedPolygons[i].size()>0){

				movePolygons(i,0,0,change);

				
			}else if(selectedEdges[i].size()>0){
				moveEdges(i,0,0,change);
			}else if(selectedVertices[i].size()>0){
				moveVertices(i,0,0,change);
			}else{
				selectedObjects[i]->pos.z+=change;
			}
		}

		for(int i=0; i<selectedLights.size(); i++){
			selectedLights[i]->pos.z+=change;
		}
	}

	if(toolActive("translate","main") && mouse.l){

		float change=mouse.percent_change_x*50;

		
		for(int i=0; i<selectedObjects.size(); i++){

			if(cameraMode=="top"){
			
				change=(remapScreenWorld(mouse.change_x,mouse.change_y)).x;
				change/=selectedObjects[i]->scale.x;
			}


			if(selectedPolygons[i].size()>0){

				movePolygons(i,change,0,0);

				
			}else if(selectedEdges[i].size()>0){
				moveEdges(i,change,0,0);
			}else if(selectedVertices[i].size()>0){
				moveVertices(i,change,0,0);
			}else{
				selectedObjects[i]->pos.x+=change;
			}
		}

		for(int i=0; i<selectedLights.size(); i++){
			selectedLights[i]->pos.x+=change;
		}
	}

	if(toolActive("translate","main") && (mouse.lDown || mouse.mDown || mouse.rDown)){
		if(!undo.storeActive){
			undo.storeActive=true;

			for(int i=0; i<selectedObjects.size(); i++){

				if(selectedPolygons[i].size()>0){

					
				}else if(selectedEdges[i].size()>0){
					
				}else if(selectedVertices[i].size()>0){
					
				}else{
					undo.storeFloatVector3d.pushBack(selectedObjects[i]->pos);
				}
			}
		}
	}

	if(toolActive("translate","main") && (mouse.lUp || mouse.mUp || mouse.rUp)){
		if(undo.storeActive){
			undo.storeActive=false;

			for(int i=0; i<selectedObjects.size(); i++){

				if(selectedPolygons[i].size()>0){

					
				}else if(selectedEdges[i].size()>0){
					
				}else if(selectedVertices[i].size()>0){
					
				}else{
					String redos=String("obj[\"")+selectedObjects[i]->name+"\"].pos.x="+String(selectedObjects[i]->pos.x)+";";
					redos+=String("obj[\"")+selectedObjects[i]->name+"\"].pos.y="+String(selectedObjects[i]->pos.y)+";";
					redos+=String("obj[\"")+selectedObjects[i]->name+"\"].pos.z="+String(selectedObjects[i]->pos.z)+";";

					String undos=String("obj[\"")+selectedObjects[i]->name+"\"].pos.x="+String(undo.storeFloatVector3d[i].x)+";";
					undos+=String("obj[\"")+selectedObjects[i]->name+"\"].pos.y="+String(undo.storeFloatVector3d[i].y)+";";
					undos+=String("obj[\"")+selectedObjects[i]->name+"\"].pos.z="+String(undo.storeFloatVector3d[i].z)+";";

					undo.add(undos,redos);
				}
			}

			undo.storeFloatVector3d.clear();
		}
	}

	if(toolActive("scale","main") && mouse.m){
		for(int i=0; i<selectedObjects.size(); i++){

			if(selectedPolygons[i].size()>0){

				scalePolygons(i,1,1.0f+mouse.percent_change_y*50,1);

			}else{
				selectedObjects[i]->scale.y+=mouse.percent_change_y*50;

				if(keyboard.shift){
					selectedObjects[i]->scale.x+=mouse.percent_change_y*50;
					selectedObjects[i]->scale.z+=mouse.percent_change_y*50;
				}
			}
		}
	}

	if(toolActive("scale","main") && mouse.r){
		for(int i=0; i<selectedObjects.size(); i++){

			if(selectedPolygons[i].size()>0){

				scalePolygons(i,1,1,1.0f+mouse.percent_change_x*50);

			}else{
				selectedObjects[i]->scale.z+=mouse.percent_change_x*50;

				if(keyboard.shift){
					selectedObjects[i]->scale.x+=mouse.percent_change_x*50;
					selectedObjects[i]->scale.y+=mouse.percent_change_x*50;
				}
			}
		}
	}

	if(toolActive("scale","main") && mouse.l){
		for(int i=0; i<selectedObjects.size(); i++){

			if(selectedPolygons[i].size()>0){

				scalePolygons(i,1.0f+mouse.percent_change_x*50,1,1);

			}else{
				selectedObjects[i]->scale.x+=mouse.percent_change_x*50;

				if(keyboard.shift){
					selectedObjects[i]->scale.y+=mouse.percent_change_x*50;
					selectedObjects[i]->scale.z+=mouse.percent_change_x*50;
				}
			}
		}
	}

	for(int i=0; i<3; i++){

		int snapto=15;
		snapto=45;

		bool act=false;
		float change=0;
		float* target;
		bool resetRotHack=false;

		if(i==0 && toolActive("rotate","main") && mouse.m){
			act=true;
			change=mouse.percent_change_x*150;
		}else if(i==1 && toolActive("rotate","main") && mouse.r){
			act=true;
			change=mouse.percent_change_y*150;
		}else if(i==2 && toolActive("rotate","main") && mouse.l){
			act=true;
			change=mouse.percent_change_x*150;
		}

		if(keyboard.shift_down){rotHack=0;}
		if(keyboard.shift_up){rotHack=0;}

		if(keyboard.shift){
			rotHack+=change;
		}

		if(act){
			for(int j=0; j<2; j++){

				int count=0;

				if(j==0){
					count=selectedObjects.size();
				}else if(j==1){
					count=selectedLights.size();
				}

				for(int k=0; k<count; k++){

					if(j==0){
						if(i==0){
							target=&selectedObjects[k]->rot.y;
						}else if(i==1){
							target=&selectedObjects[k]->rot.z;
						}else if(i==2){
							target=&selectedObjects[k]->rot.x;
						}
					}else{	
						if(i==0){
							target=&selectedLights[k]->rot.y;
						}else if(i==1){
							target=&selectedLights[k]->rot.z;
						}else if(i==2){
							target=&selectedLights[k]->rot.x;
						}
					}

					if(keyboard.shift){

						int tint=int(*target+rotHack);

						while(tint<0){tint+=snapto;}

						int mod=tint % snapto;

						if(mod> (snapto/2)){
							if(rotHack>0){
								resetRotHack=true;
								*target=int(*target)+snapto-mod;
							}
						}else{
							if(rotHack<0){
								resetRotHack=true;
								*target=int(*target)-mod;
							}
						}
					}else{
						*target+=change;
					}
				}
			}
		}

		if(resetRotHack){
			rotHack=0;
		}
	}

	if(activateEvent("i",true,"main")){
		expertMode=!expertMode;
	}

	if(mouse.l || mouse.r || mouse.m){
		bool rd=false;

		for(int i=0; i<selectedObjects.size(); i++){
			if(selectedObjects[i]->flags&OBJECT_STATIC){
				selectedObjects[i]->update();	//just in case
				rd=true;
			}
		}

		if(rd){
			drawRefreshAll();
		}
		
	}

	if(activateTool("translate","main")){
		toolCommandsStart="Translate Tool\n\nmouse-l : Translate X\nmouse-m : Translate Y\nmouse-r : Translate Z";
	}

	if(activateTool("rotate","main")){
		toolCommandsStart="Rotate Tool\n\nmouse-l : Rotate X\nmouse-m : Rotate Y\nmouse-r : Rotate Z";
	}

	if(activateTool("scale","main")){
		toolCommandsStart="Scale Tool\n\nmouse-l : Scale X\nmouse-m : Scale Y\nmouse-r : Scale Z";
	}

	if(activateTool("selectobjects","main")){

		if(!wasSticky()){
			selectedObjects.clear();
			selectedLights.clear();
			selectedPolygons.clear();
			selectedEdges.clear();
			selectedVertices.clear();
		}
		
		toolCommandsStart="Select Tool\n\nmouse-l : Select Objects\nmouse-r : Marquee";
	}

	if(activateTool("selectpolygons","main")){
		toolCommandsStart="Select Tool\n\nmouse-l : Select Polygons\nmouse-r : Marquee\n\ni : Invert Polygon\nd : Duplicate Polygon\nb : Bridge Polygon";
	}

	int sp=0;

	for(int i=0; i<selectedPolygons.size(); i++){
		sp+=selectedPolygons[i].size();
	}

	if(toolActive("selectpolygons","main") && sp>0){
		toolCommandsStart="Select Tool\n\nmouse-l : Select Polygons\nmouse-l : Marquee\n\nd : Duplicate Polygon\ni : Invert Polygon\nb : Bridge Polygon\nn : Inset Polygon";

		for(int i=0; i<selectedEdges.size(); i++){
			selectedEdges[i].clear();
		}

		for(int i=0; i<selectedVertices.size(); i++){
			selectedVertices[i].clear();
		}
	}else if(toolActive("selectpolygons","main")){
		toolCommandsStart="Select Tool\n\nmouse-l : Select Polygons";

		for(int i=0; i<selectedEdges.size(); i++){
			selectedEdges[i].clear();
		}

		for(int i=0; i<selectedVertices.size(); i++){
			selectedVertices[i].clear();
		}
	}

	if(activateTool("selectvertices","main")){
		toolCommandsStart="Select Tool\n\nmouse-l : Select Vertices";

		for(int i=0; i<selectedPolygons.size(); i++){
			selectedPolygons[i].clear();
		}

		for(int i=0; i<selectedEdges.size(); i++){
			selectedEdges[i].clear();
		}
	}

	if(activateTool("selectedges","main")){
		toolCommandsStart="Select Tool\n\nmouse-l : Select Edges";

		for(int i=0; i<selectedPolygons.size(); i++){
			selectedPolygons[i].clear();
		}

		for(int i=0; i<selectedVertices.size(); i++){
			selectedVertices[i].clear();
		}
	}

	if(activateEvent("l",false,"main")){
		editorLocal=true;
	}

	if(activateEvent("z",true,"main")){
		undo.undo();
	}

	if(activateEvent("y",true,"main")){
		undo.redo();
	}

	if(activateEvent("g",false,"main")){
		editorLocal=false;
	}

	if(activateEvent("h",false,"main")){
		hideSelection();
	}

	if(activateEvent("a",true,"main")){
		selectAll();
	}

	if(activateEvent("s",true,"main")){

		saveLevel();
	}

	if(activateEvent("b",true,"main")){
		yescancel("Really build release version of project?",buildFinal);
	}
	
	solomode.textpanel->pos.y=-conf->sizeY+20;

	if(activateEvent("o",false,"main")){
		toggleSolo();
	}

	if(activateEvent("a",false,"main")){
		centerAll();
	}

	if(activateEvent("f",false,"main")){
		centerSelected();
	}

	if(activateEvent("b",false,"main")){
		renderLMMode=true;
	}

	if(activateEvent("n",false,"main")){
		conf->normals=!conf->normals;
	}

	if(activateEvent("i",false,"main")){
		invertNormals();
	}

	bool polysaresel=false;
	for(int i=0; i<selectedObjects.size(); i++){
		if(selectedPolygons[i].size()>0){
			polysaresel=true;
			break;
		}
	}

	if(toolActive("selectpolygons","main") || 
		(toolActive("translate","main") || toolActive("scale","main") || toolActive("rotate","main")) && polysaresel){
		if(activateEvent("d",false,"main")){
			duplicatePolygon();
		}

		if(activateEvent("r",false,"main")){
			bridgePolygons();
		}
	}

	bool edgesaresel=false;
	for(int i=0; i<selectedObjects.size(); i++){
		if(selectedEdges[i].size()>0){
			edgesaresel=true;
			break;
		}
	}

	if(toolActive("selectedges","main") || 
		(toolActive("translate","main") || toolActive("scale","main") || toolActive("rotate","main")) && edgesaresel){
		if(activateEvent("d",false,"main")){
			duplicateEdges();
		}

	}

	if(conf->normals){

		if(selectedObjects.size()>0){
			conf->normalsSolo=selectedObjects[0]->name;
		}else{
			conf->normalsSolo="";
		}
	}


	if(activateEvent("escape",false,"main")){
		quit();
	}

	


	if(activateTool("navigate","main")){
		toolCommandsStart="Navigate Tool\n\nmouse-l : Move\nmouse-m : Zoom\nmouse-z : Orbit";
	}

	if(toolActive("navigate","main") && mouse.r){

		
		if(cameraMode=="top"){
			
		}else{
			level->camera->rot.x+=mouse.change_y*1*engineTime.time;
			level->camera->rot.y-=mouse.change_x*1*engineTime.time;

			FloatVector3d d=-math.degreesToNormal(level->camera->rot);

			level->camera->pos=cameraCenter+d*cameraDistance;
		}
	}

	if(toolActive("navigate","main") && mouse.l){

		if(cameraMode=="top"){
			FloatVector2d c=remapScreenWorld(mouse.change_x,mouse.change_y);

			level->camera->pos.x-=c.x;
			level->camera->pos.z+=c.y;
		}else{

			FloatVector3d ax1=math.degreesToNormal(FloatVector3d(level->camera->rot.x+1,level->camera->rot.y,level->camera->rot.z))-math.degreesToNormal(level->camera->rot);
			ax1=math.normalize(ax1);
			
			FloatVector3d ax2=math.degreesToNormal(FloatVector3d(level->camera->rot.x,level->camera->rot.y-1,level->camera->rot.z))-math.degreesToNormal(level->camera->rot);
			ax2=math.normalize(ax2);

			float cd=cameraDistance;
			if(cd<1){cd=1;}

			cameraCenter-=ax2*mouse.change_x*0.005*cd;
			cameraCenter-=ax1*mouse.change_y*0.005*cd;

			FloatVector3d d=-math.degreesToNormal(level->camera->rot);
			level->camera->pos=cameraCenter+d*cameraDistance;
		}
	}

	if(toolActive("navigate","main") && cameraMode=="top"){
		if(mouse.wheelUp){
			topZoom+=2;
		}

		if(mouse.wheelDown){
			topZoom-=2;
		}

		if(topZoom<1){topZoom=1;}

		level->camera->fov=topZoom;
	}

	if(toolActive("navigate","main") && mouse.m){

		if(cameraMode=="top"){
			topZoom+=mouse.change_y*(0.1f+topZoom*0.01f);

			if(topZoom<1){topZoom=1;}

			level->camera->fov=topZoom;
		}else{
			cameraDistance+=mouse.change_y*(cameraDistance*0.01);

			if(cameraDistance<1){cameraDistance=1;}

			FloatVector3d d=-math.degreesToNormal(level->camera->rot);

			level->camera->pos=cameraCenter+d*cameraDistance;
		}
	}

	if(activateEvent("delete",false,"main")  ){
		deleteSelection();

	}

	if(mouse.rUp){
		marquee=false;
		marqueeBkg.panel->visible=false;
	}

	if(toolActive("selectobjects","main")){
		if(mouse.rDown){
			marquee=true;
			marqueeBkg.panel->pos.x=mouse.x;
			marqueeBkg.panel->pos.y=mouse.y;
			marqueeBkg.panel->visible=true;
		}
	}

	if(marquee){
		marqueeBkg.panel->scale.x=mouse.x-marqueeBkg.panel->pos.x;
		marqueeBkg.panel->scale.y=-mouse.y+marqueeBkg.panel->pos.y;
	}

	if(activateEvent("1",false,"main")){
		freeCamera();
	}

	if(activateEvent("2",false,"main")){
		freeOrthographicCamera();
	}

	if(activateEvent("3",false,"main")){
		topCamera();
	}

	if(activateEvent("4",false,"main")){
		frontCamera();
	}

	if(activateEvent("5",false,"main")){
		rightCamera();
	}
}

void Editor::hideSelection(){
	
	for(int i=0; i<selectedObjects.size(); i++){
		selectedObjects[i]->visible=!selectedObjects[i]->visible;
	}

	selectedObjects.clear();
}

void Editor::freeCamera(){
	if(cameraMode!="free"){
		cameraMode="free";
		level->camera->orthographic=false;
		level->camera->centerOrtho=false;

		level->camera->fov=freePerspectiveFov;
		level->camera->pos=freePerspectivePos;
		level->camera->rot=freePerspectiveRot;

		viewTitle.textpanel->setText("Free Camera");
	}
}

void Editor::freeOrthographicCamera(){
	if(cameraMode=="free"){
		freePerspectiveFov=level->camera->fov;
		freePerspectivePos=level->camera->pos;
		freePerspectiveRot=level->camera->rot;
	}

	cameraMode="freeortho";
	level->camera->orthographic=true;
	viewTitle.textpanel->setText("Free Orthographic Camera");
}

void Editor::topCamera(){
	if(cameraMode=="free"){
		freePerspectiveFov=level->camera->fov;
		freePerspectivePos=level->camera->pos;
		freePerspectiveRot=level->camera->rot;
	}

	cameraMode="top";
	level->camera->orthographic=true;
	level->camera->centerOrtho=true;
	level->camera->rot.x=-90;
	level->camera->rot.y=0;
	level->camera->rot.z=0;

	level->camera->fov=topZoom;

	level->camera->pos.x=0;
	level->camera->pos.y=10;
	level->camera->pos.z=0;
	
	viewTitle.textpanel->setText("Top");
}

void Editor::frontCamera(){
	if(cameraMode=="free"){
		freePerspectiveFov=level->camera->fov;
		freePerspectivePos=level->camera->pos;
		freePerspectiveRot=level->camera->rot;
	}

	cameraMode="front";
	level->camera->orthographic=true;
	level->camera->rot.x=0;
	level->camera->rot.y=0;
	level->camera->rot.z=0;

	level->camera->pos.x=0;
	level->camera->pos.y=0;
	level->camera->pos.z=20;
	viewTitle.textpanel->setText("Front");
}

void Editor::rightCamera(){
	if(cameraMode=="free"){
		freePerspectiveFov=level->camera->fov;
		freePerspectivePos=level->camera->pos;
		freePerspectiveRot=level->camera->rot;
	}

	cameraMode="right";
	level->camera->orthographic=true;
	viewTitle.textpanel->setText("Right");
}

void Editor::updateCamera(){
	if(cameraMode=="top"){
			
	}else{
		FloatVector3d d=-math.degreesToNormal(level->camera->rot);
		level->camera->pos=cameraCenter+d*cameraDistance;
	}
}

#endif
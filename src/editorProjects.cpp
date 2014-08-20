#ifdef DEVELOPER

#include "editor.h"
#include "keyboard.h"
#include "mouse.h"
#include <fstream>
#include "os.h"
#include "conf.h"

ObjectAddress projectsFade;
ObjectAddress projectsBkg;
ObjectAddress projectsWindowTitle;
ObjectAddress projectsTitle;
ObjectAddress projectsOK;
ObjectAddress projectsOKB;
ObjectAddress projectsCancel;
ObjectAddress projectsCancelB;
ObjectAddress projectsNewProject;
ObjectAddress projectsNewProjectB;
ObjectAddress projectsNewProjectE;
ObjectAddress projectsNewProjectT;
ObjectAddress projectsNewLevel;
ObjectAddress projectsNewLevelB;
ObjectAddress projectsNewLevelE;
ObjectAddress projectsNewLevelT;
ObjectAddress projectsList;
ObjectAddress levelListBkg;
Array<ObjectAddress> projectListT;
Array<ObjectAddress> projectListB;
Array<ObjectAddress> levelListT;
Array<ObjectAddress> levelListB;
bool projectsVisible=false;

bool projectListLoaded=false;

int selectedLevel=-1;
int selectedProject=-1;

bool enterProject=false;
bool enterLevel=false;
String enterProjectString;
String enterLevelString;

void Editor::saveProjectDatabase(){
	std::ofstream out;
	
	String fname="projects.dat";

	out.open(fname.c_str(),std::ios::out);

	for(int i=0; i<projectList.size(); i++){
		String p="project:"+projectList[i]+"\n";
		out.write(p.c_str(),p.size());

		for(int j=0; j<levelList[i].size(); j++){
			String s="level:"+levelList[i][j]+"\n";
			out.write(s.cStr(),s.size());
		}

		p="mainscript:"+mainScriptFiles[i]+"\n";
		out.write(p.c_str(),p.size());

		out.write("\n",1);
	}

	out.close();

}

void Editor::loadProjectDatabase(){
	std::ifstream in;

	String fname="projects.dat";

	in.open(fname.c_str(),std::ios::in);

	projectList.clear();
	levelList.clear();

	std::string line;
	String s;
	while(getline(in, line)){
		s=line;
		if(s.substrIndex(0,8)=="project:"){
			projectList.pushBack(s.substrIndex(8,s.size()));
			levelList.pushBack();
		}else if(s.substrIndex(0,6)=="level:"){
			levelList[projectList.size()-1].pushBack(s.substrIndex(6,s.size()));
		}else if(s.substrIndex(0,11)=="mainscript:"){
			mainScriptFiles.pushBack(s.substrIndex(11,s.size()));
			scriptFiles.pushBack(s.substrIndex(11,s.size()));
		}else if(s.substrIndex(0,7)=="script:"){
			scriptFiles.pushBack(s.substrIndex(7,s.size()));
		}else if(s.substrIndex(0,10)=="lmtpacker:"){
			lmtpacker.pushBack(s.substrIndex(10,s.size()));
		}else if(s.substrIndex(0,5)=="misc:"){
			misc.pushBack(s.substrIndex(5,s.size()));
		}
	}


	in.close();
}


void Editor::drawProjects(){
	if(keyboard.ctrl && keyboard.oDown){
		showProjects();
		
	}
	
	if(projectsVisible){

		if(enterProject){
			enterProjectString+=keyboard.currentStringDown;
			projectsNewProjectT.textpanel->setText(enterProjectString);
			projectsNewProjectT.textpanel->color.w=1;
		}

		if(enterLevel){
			enterLevelString+=keyboard.currentStringDown;
			projectsNewLevelT.textpanel->setText(enterLevelString);
			projectsNewLevelT.textpanel->color.w=1;
		}

		if(mouse.lDown){
			enterProject=false;
			enterLevel=false;
		}

		projectsFade.panel->visible=true;
		projectsBkg.panel->visible=true;
		projectsWindowTitle.textpanel->visible=true;
		projectsTitle.textpanel->visible=true;
		projectsOK.textpanel->visible=true;
		projectsOKB.panel->visible=true;
		projectsCancel.textpanel->visible=true;
		projectsCancelB.panel->visible=true;
		projectsNewProject.textpanel->visible=true;
		projectsNewProjectB.panel->visible=true;
		projectsNewProjectE.panel->visible=true;
		projectsNewProjectT.textpanel->visible=true;
		projectsNewLevel.textpanel->visible=true;
		projectsNewLevelB.panel->visible=true;
		projectsNewLevelE.panel->visible=true;
		projectsNewLevelT.textpanel->visible=true;
		projectsList.panel->visible=true;
		levelListBkg.panel->visible=true;

		if(!level->file.set()){
			projectsCancel.textpanel->color.w=0.5;
			projectsCancelB.panel->color.w=0.5;
		}else{	
			projectsCancel.textpanel->color.w=1;
			projectsCancelB.panel->color.w=1;
		}

		if(selectedProject!=-1 && selectedLevel!=-1){
			projectsOK.textpanel->color.w=1;
			projectsOKB.panel->color.w=1;
		}else{
			projectsOK.textpanel->color.w=0.5;
			projectsOKB.panel->color.w=0.5;
		}

		for(int i=0; i<projectList.size(); i++){

			if(i>=projectListT.size()){
				ObjectAddress temp=add("projectsList"+String(i),"textpanel");
				temp.textpanel->pos.x=120;
				temp.textpanel->pos.y=projectListT[i-1].textpanel->pos.y-text.leading();
				temp.textpanel->pos.z=93;
				temp.textpanel->color.x=1;
				temp.textpanel->color.y=1;
				temp.textpanel->color.z=1;
				temp.textpanel->color.w=1;
				temp.textpanel->setText(projectList[i]);
				temp.textpanel->percent=false;
				temp.textpanel->visible=false;

				projectListT.pushBack(temp);
			}

			
			projectListT[i].textpanel->visible=true;
			
			if(selectedProject==i){
				projectListT[i].textpanel->color.y=0;
				projectListT[i].textpanel->color.z=0;
			}else{
				projectListT[i].textpanel->color.y=1;
				projectListT[i].textpanel->color.z=1;
			}

			if(projectListT[i].textpanel->mouseover && mouse.lUp){
				selectedProject=i;
				
				selectedLevel=-1;

				levelListT[0].textpanel->setText("[None]");

				for(int v=1; v<levelListT.size(); v++){
					levelListT[v].textpanel->visible=false;
				}
			}

			if(selectedProject!=-1){
				for(int v=0; v<levelList[selectedProject].size(); v++){

					if(v>=levelListT.size()){
						ObjectAddress temp=add("levelList"+String(levelListT.size()-1),"textpanel");
						temp.textpanel->pos.x=420;
						temp.textpanel->pos.y=-200;
						temp.textpanel->pos.z=93;
						temp.textpanel->color.x=1;
						temp.textpanel->color.y=1;
						temp.textpanel->color.z=1;
						temp.textpanel->color.w=1;
						temp.textpanel->setText("[None]");
						temp.textpanel->percent=false;
						temp.textpanel->visible=false;

						levelListT.pushBack(temp);
					}

					levelListT[v].textpanel->setText(levelList[selectedProject][v]);
					levelListT[v].textpanel->pos.y=-200-text.leading()*v;
				}
			}
		}

		for(int i=0; i<levelListT.size(); i++){
			levelListT[i].textpanel->visible=true;

			if(selectedLevel==i){
				levelListT[i].textpanel->color.y=0;
				levelListT[i].textpanel->color.z=0;
			}else{
				levelListT[i].textpanel->color.y=1;
				levelListT[i].textpanel->color.z=1;
			}

			if(levelListT[i].textpanel->mouseover && mouse.lUp){
				if(selectedProject!=-1){
					if(levelList[selectedProject].size()>0){
						selectedLevel=i;
					}
				}
			}
		}


		projectsBkg.panel->pos.x=conf->sizeX/2-300;
		projectsBkg.panel->pos.y=-conf->sizeY/2+220;

		projectsList.panel->pos.x=projectsBkg.panel->pos.x+10;
		projectsList.panel->pos.y=projectsBkg.panel->pos.y-60;

		levelListBkg.panel->pos.x=projectsBkg.panel->pos.x+310;
		levelListBkg.panel->pos.y=projectsBkg.panel->pos.y-60;

		projectsWindowTitle.textpanel->pos.x=projectsBkg.panel->pos.x+10;
		projectsWindowTitle.textpanel->pos.y=projectsBkg.panel->pos.y-10;

		projectsTitle.textpanel->pos.x=projectsBkg.panel->pos.x+10;
		projectsTitle.textpanel->pos.y=projectsBkg.panel->pos.y-35;

		projectsOK.textpanel->pos.x=projectsBkg.panel->pos.x+540+19-13;
		projectsOK.textpanel->pos.y=projectsBkg.panel->pos.y-410;

		projectsOKB.panel->pos.x=projectsOK.textpanel->pos.x-10;
		projectsOKB.panel->pos.y=projectsOK.textpanel->pos.y+10;

		projectsCancel.textpanel->pos.x=projectsBkg.panel->pos.x+20;
		projectsCancel.textpanel->pos.y=projectsBkg.panel->pos.y-410;

		projectsCancelB.panel->pos.x=projectsCancel.textpanel->pos.x-10;
		projectsCancelB.panel->pos.y=projectsCancel.textpanel->pos.y+10;

		projectsNewProject.textpanel->pos.x=projectsBkg.panel->pos.x+180+17;
		projectsNewProject.textpanel->pos.y=projectsBkg.panel->pos.y-370;

		projectsNewProjectB.panel->pos.x=projectsNewProject.textpanel->pos.x-10;
		projectsNewProjectB.panel->pos.y=projectsNewProject.textpanel->pos.y+10;

		projectsNewProjectE.panel->pos.x=projectsBkg.panel->pos.x+10;
		projectsNewProjectE.panel->pos.y=projectsBkg.panel->pos.y-360;

		projectsNewProjectT.textpanel->pos.x=projectsBkg.panel->pos.x+20;
		projectsNewProjectT.textpanel->pos.y=projectsBkg.panel->pos.y-370;

		projectsNewLevel.textpanel->pos.x=projectsBkg.panel->pos.x+510;
		projectsNewLevel.textpanel->pos.y=projectsBkg.panel->pos.y-370;

		projectsNewLevelB.panel->pos.x=projectsNewLevel.textpanel->pos.x-10;
		projectsNewLevelB.panel->pos.y=projectsNewLevel.textpanel->pos.y+10;

		projectsNewLevelE.panel->pos.x=projectsBkg.panel->pos.x+310;
		projectsNewLevelE.panel->pos.y=projectsBkg.panel->pos.y-360;

		projectsNewLevelT.textpanel->pos.x=projectsBkg.panel->pos.x+320;
		projectsNewLevelT.textpanel->pos.y=projectsBkg.panel->pos.y-370;

		float lp=projectsBkg.panel->pos.y-70;

		for(int i=0; i<levelListT.size(); i++){
			levelListT[i].textpanel->pos.x=projectsBkg.panel->pos.x+320;
			levelListT[i].textpanel->pos.y=lp;
			lp-=text.leading();
		}

		lp=projectsBkg.panel->pos.y-70;
		for(int i=0; i<projectListT.size(); i++){
			projectListT[i].textpanel->pos.x=projectsBkg.panel->pos.x+20;
			projectListT[i].textpanel->pos.y=lp;
			lp-=text.leading();
		}
	}else{	
		projectsFade.panel->visible=false;
		projectsBkg.panel->visible=false;
		projectsWindowTitle.textpanel->visible=false;
		projectsTitle.textpanel->visible=false;
		projectsOK.textpanel->visible=false;
		projectsOKB.panel->visible=false;
		projectsCancel.textpanel->visible=false;
		projectsCancelB.panel->visible=false;
		projectsNewProject.textpanel->visible=false;
		projectsNewProjectB.panel->visible=false;
		projectsNewProjectE.panel->visible=false;
		projectsNewProjectT.textpanel->visible=false;
		projectsNewLevel.textpanel->visible=false;
		projectsNewLevelB.panel->visible=false;
		projectsNewLevelE.panel->visible=false;
		projectsNewLevelT.textpanel->visible=false;
		projectsList.panel->visible=false;
		levelListBkg.panel->visible=false;

		for(int i=0; i<projectListT.size(); i++){
			projectListT[i].textpanel->visible=false;
		}

		for(int i=0; i<levelListT.size(); i++){
			levelListT[i].textpanel->visible=false;
		}
	}
}

void activateProject(){
	enterProject=true;
}

void activateLevel(){
	enterLevel=true;
}

void hideProjects(){
	if(!level->file.set()){return;}

	editor.forceGui("projects",false);
	projectsVisible=false;
}

void loadcallback();

void saveloadcallback(){
	editor.saveLevel();
	loadcallback();
}

void loadLevelInner(){

	String levelname=editor.levelList[selectedProject][selectedLevel];

	String file=editor.projectList[selectedProject]+"/"+levelname+"/"+levelname+".lml";

	if(os().fileExists(file)){
		bool succ=false;

		bool tc=conf->loadProgressive;
		conf->loadProgressive=false;

		if(!level->file.set()){
			succ=level->load(file);
		}else{
			newLevel();
			succ=level->load(file);
		}
		conf->loadProgressive=tc;

		if(!succ){
			editor.editorError("Error loading level, see main.log and renderer.log");
		}

	}else{
		editor.editorWarning("Created new level "+levelname);
		level->file=editor.projectList[selectedProject]+"/"+levelname+"/"+levelname+".lml";
	}

	editor.loadedLevel=selectedLevel;
	editor.loadedProject=selectedProject;
	editor.mainScriptFile=editor.mainScriptFiles[selectedProject];

	editor.loadedMainScript="";
	String fi=editor.projectList[selectedProject]+"/"+editor.mainScriptFile;

	if(os().fileExists(fi)){
		std::ifstream in;
		
		in.open(fi.c_str(),std::ios::in);
		std::string line;
		while(getline(in, line)){
			editor.loadedMainScript+=line+String("\n");
		}
		in.close();

		script().run(editor.loadedMainScript);
	}

	editor.forceGui("projects",false);
	projectsVisible=false;

	editor.cameraCenter=FloatVector3d();
	editor.cameraDistance=10;
	level->camera->rot.x=-45;
	level->camera->rot.y=45;
	editor.updateCamera();
}


void loadLevel(){
	if(selectedProject!=-1 && selectedLevel!=-1){
		if(!level->file.set()){	//no level loaded yet at all
			loadLevelInner();
		}else{
			//there's already a level loaded
			editor.yesnocancel("Save level '"+editor.levelList[editor.loadedProject][editor.loadedLevel]+"' in project '"+editor.projectList[editor.loadedProject]+"'?",saveloadcallback,loadcallback);
		}
	}
}

void loadcallback(){
	delete level;
	level=new Level;
	loadLevelInner();
}

void makeNewProject(){
	editor.projectList.pushBack(enterProjectString);
	editor.levelList.pushBack();
	editor.mainScriptFiles.pushBack("main.lua");
	editor.mainScriptFile="main.lua";

	editor.saveProjectDatabase();

	os().mkdir(enterProjectString);

	String defaultScript="function perframe()\n"
"	if keyboard.escape then\n" 
"		os.exit()\n"
"	end\n"
"end\n" 
"\n"
"function main()\n";

	defaultScript+="\tlevel.load()\n";
	defaultScript+="end\n";

	std::ofstream out;
	
	String scriptfile=editor.projectList[editor.projectList.size()-1]+"/"+editor.mainScriptFile;;

	out.open(scriptfile.c_str(),std::ios::out);
	out.write(defaultScript.c_str(),defaultScript.size());

	out.close();

	enterProjectString="";
}

void makeNewLevel(){
	
	if(selectedProject==-1){
		return;
	}

	editor.levelList[selectedProject].pushBack(enterLevelString);

	editor.saveProjectDatabase();

	enterLevelString="";

}

void Editor::showProjects(){
	projectsVisible=true;
	forceGui("projects",true);
}

void Editor::projectsInit(){
	projectsFade=add("projectsFade","panel");
	projectsFade.panel->pos.x=0;
	projectsFade.panel->pos.y=0;
	projectsFade.panel->pos.z=90;
	projectsFade.panel->color.x=0;
	projectsFade.panel->color.y=0;
	projectsFade.panel->color.z=0;
	projectsFade.panel->color.w=0.5;
	projectsFade.panel->percent=true;
	projectsFade.panel->scale.x=1;
	projectsFade.panel->scale.y=1;
	projectsFade.panel->visible=false;

	projectsBkg=add("projectsBkg","panel");
	projectsBkg.panel->pos.x=100;
	projectsBkg.panel->pos.y=-100;
	projectsBkg.panel->pos.z=91;
	projectsBkg.panel->color.x=0;
	projectsBkg.panel->color.y=0;
	projectsBkg.panel->color.z=0.5;
	projectsBkg.panel->color.w=1;
	projectsBkg.panel->percent=false;
	projectsBkg.panel->scale.x=600;
	projectsBkg.panel->scale.y=440;
	projectsBkg.panel->visible=false;

	projectsList=add("projectsList","panel");
	projectsList.panel->pos.x=110;
	projectsList.panel->pos.y=-160;
	projectsList.panel->pos.z=92;
	projectsList.panel->color.x=0;
	projectsList.panel->color.y=0;
	projectsList.panel->color.z=0.75;
	projectsList.panel->color.w=1;
	projectsList.panel->percent=false;
	projectsList.panel->scale.x=280;
	projectsList.panel->scale.y=290;
	projectsList.panel->visible=false;

	levelListBkg=add("levelListBkg","panel");
	levelListBkg.panel->pos.x=410;
	levelListBkg.panel->pos.y=-160;
	levelListBkg.panel->pos.z=92;
	levelListBkg.panel->color.x=0;
	levelListBkg.panel->color.y=0;
	levelListBkg.panel->color.z=0.75;
	levelListBkg.panel->color.w=1;
	levelListBkg.panel->percent=false;
	levelListBkg.panel->scale.x=280;
	levelListBkg.panel->scale.y=290;
	levelListBkg.panel->visible=false;

	projectsWindowTitle=add("projectsWindowTitle","textpanel");
	projectsWindowTitle.textpanel->pos.x=110;
	projectsWindowTitle.textpanel->pos.y=-110;
	projectsWindowTitle.textpanel->pos.z=92;
	projectsWindowTitle.textpanel->color.x=1;
	projectsWindowTitle.textpanel->color.y=1;
	projectsWindowTitle.textpanel->color.z=1;
	projectsWindowTitle.textpanel->color.w=1;
	projectsWindowTitle.textpanel->setText("Choose Level To Edit");
	projectsWindowTitle.textpanel->percent=false;
	projectsWindowTitle.textpanel->visible=false;

	projectsTitle=add("projectsTitle","textpanel");
	projectsTitle.textpanel->pos.x=110;
	projectsTitle.textpanel->pos.y=-135;
	projectsTitle.textpanel->pos.z=92;
	projectsTitle.textpanel->color.x=1;
	projectsTitle.textpanel->color.y=1;
	projectsTitle.textpanel->color.z=1;
	projectsTitle.textpanel->color.w=1;
	projectsTitle.textpanel->setText("Projects                                                             Level");
	projectsTitle.textpanel->percent=false;
	projectsTitle.textpanel->visible=false;

	projectsOK=add("projectsOK","textpanel");
	projectsOK.textpanel->pos.x=640+19;
	projectsOK.textpanel->pos.y=-510;
	projectsOK.textpanel->pos.z=93;
	projectsOK.textpanel->color.x=1;
	projectsOK.textpanel->color.y=1;
	projectsOK.textpanel->color.z=1;
	projectsOK.textpanel->color.w=1;
	projectsOK.textpanel->setText("Load");
	projectsOK.textpanel->percent=false;
	projectsOK.textpanel->visible=false;
	projectsOK.textpanel->clickable=false;

	projectsOKB=add("projectsOKB","panel");
	projectsOKB.panel->pos.x=projectsOK.textpanel->pos.x-10;
	projectsOKB.panel->pos.y=projectsOK.textpanel->pos.y+10;
	projectsOKB.panel->pos.z=92;
	projectsOKB.panel->color.x=0;
	projectsOKB.panel->color.y=0;
	projectsOKB.panel->color.z=0.75;
	projectsOKB.panel->color.w=1;
	projectsOKB.panel->scale.x=projectsOK.textpanel->scale.x+20;
	projectsOKB.panel->scale.y=projectsOK.textpanel->scale.y+20;
	projectsOKB.panel->percent=false;
	projectsOKB.panel->visible=false;
	projectsOKB.panel->onMouseLUp=loadLevel;

	projectsCancel=add("projectsCancel","textpanel");
	projectsCancel.textpanel->pos.x=120;
	projectsCancel.textpanel->pos.y=-510;
	projectsCancel.textpanel->pos.z=93;
	projectsCancel.textpanel->color.x=1;
	projectsCancel.textpanel->color.y=1;
	projectsCancel.textpanel->color.z=1;
	projectsCancel.textpanel->color.w=1;
	projectsCancel.textpanel->setText("Cancel");
	projectsCancel.textpanel->percent=false;
	projectsCancel.textpanel->visible=false;
	projectsCancel.textpanel->clickable=false;

	projectsCancelB=add("projectsCancelB","panel");
	projectsCancelB.panel->pos.x=projectsCancel.textpanel->pos.x-10;
	projectsCancelB.panel->pos.y=projectsCancel.textpanel->pos.y+10;
	projectsCancelB.panel->pos.z=92;
	projectsCancelB.panel->color.x=0;
	projectsCancelB.panel->color.y=0;
	projectsCancelB.panel->color.z=0.75;
	projectsCancelB.panel->color.w=1;
	projectsCancelB.panel->scale.x=projectsCancel.textpanel->scale.x+20;
	projectsCancelB.panel->scale.y=projectsCancel.textpanel->scale.y+20;
	projectsCancelB.panel->percent=false;
	projectsCancelB.panel->visible=false;
	projectsCancelB.panel->onMouseLUp=hideProjects;

	projectsNewProject=add("projectsNewProject","textpanel");
	projectsNewProject.textpanel->pos.x=280+17;
	projectsNewProject.textpanel->pos.y=-470;
	projectsNewProject.textpanel->pos.z=93;
	projectsNewProject.textpanel->color.x=1;
	projectsNewProject.textpanel->color.y=1;
	projectsNewProject.textpanel->color.z=1;
	projectsNewProject.textpanel->color.w=1;
	projectsNewProject.textpanel->setText("New Project");
	projectsNewProject.textpanel->percent=false;
	projectsNewProject.textpanel->visible=false;
	projectsNewProject.textpanel->clickable=false;

	projectsNewProjectB=add("projectsNewProjectB","panel");
	projectsNewProjectB.panel->pos.x=projectsNewProject.textpanel->pos.x-10;
	projectsNewProjectB.panel->pos.y=projectsNewProject.textpanel->pos.y+10;
	projectsNewProjectB.panel->pos.z=92;
	projectsNewProjectB.panel->color.x=0;
	projectsNewProjectB.panel->color.y=0;
	projectsNewProjectB.panel->color.z=0.75;
	projectsNewProjectB.panel->color.w=1;
	projectsNewProjectB.panel->scale.x=projectsNewProject.textpanel->scale.x+20;
	projectsNewProjectB.panel->scale.y=projectsNewProject.textpanel->scale.y+20;
	projectsNewProjectB.panel->percent=false;
	projectsNewProjectB.panel->visible=false;
	projectsNewProjectB.panel->onMouseLUp=makeNewProject;

	projectsNewProjectE=add("projectsNewProjectE","panel");
	projectsNewProjectE.panel->pos.x=110;
	projectsNewProjectE.panel->pos.y=-460;
	projectsNewProjectE.panel->pos.z=92;
	projectsNewProjectE.panel->color.x=0;
	projectsNewProjectE.panel->color.y=0;
	projectsNewProjectE.panel->color.z=0.75;
	projectsNewProjectE.panel->color.w=1;
	projectsNewProjectE.panel->scale.x=110+62;
	projectsNewProjectE.panel->scale.y=projectsNewProject.textpanel->scale.y+20;
	projectsNewProjectE.panel->percent=false;
	projectsNewProjectE.panel->visible=false;
	projectsNewProjectE.panel->onMouseLUp=activateProject;

	projectsNewProjectT=add("projectsNewProjectT","textpanel");
	projectsNewProjectT.textpanel->pos.x=120;
	projectsNewProjectT.textpanel->pos.y=-470;
	projectsNewProjectT.textpanel->pos.z=93;
	projectsNewProjectT.textpanel->color.x=1;
	projectsNewProjectT.textpanel->color.y=1;
	projectsNewProjectT.textpanel->color.z=1;
	projectsNewProjectT.textpanel->color.w=0.5;
	projectsNewProjectT.textpanel->setText("Enter Name Here");
	projectsNewProjectT.textpanel->percent=false;
	projectsNewProjectT.textpanel->visible=false;
	projectsNewProjectT.textpanel->clickable=false;

	projectsNewLevel=add("projectsNewLevel","textpanel");
	projectsNewLevel.textpanel->pos.x=610;
	projectsNewLevel.textpanel->pos.y=-470;
	projectsNewLevel.textpanel->pos.z=93;
	projectsNewLevel.textpanel->color.x=1;
	projectsNewLevel.textpanel->color.y=1;
	projectsNewLevel.textpanel->color.z=1;
	projectsNewLevel.textpanel->color.w=1;
	projectsNewLevel.textpanel->setText("New Level");
	projectsNewLevel.textpanel->percent=false;
	projectsNewLevel.textpanel->visible=false;
	projectsNewLevel.textpanel->clickable=false;

	projectsNewLevelB=add("projectsNewLevelB","panel");
	projectsNewLevelB.panel->pos.x=projectsNewLevel.textpanel->pos.x-10;
	projectsNewLevelB.panel->pos.y=projectsNewLevel.textpanel->pos.y+10;
	projectsNewLevelB.panel->pos.z=92;
	projectsNewLevelB.panel->color.x=0;
	projectsNewLevelB.panel->color.y=0;
	projectsNewLevelB.panel->color.z=0.75;
	projectsNewLevelB.panel->color.w=1;
	projectsNewLevelB.panel->scale.x=projectsNewLevel.textpanel->scale.x+20;
	projectsNewLevelB.panel->scale.y=projectsNewLevel.textpanel->scale.y+20;
	projectsNewLevelB.panel->percent=false;
	projectsNewLevelB.panel->visible=false;
	projectsNewLevelB.panel->onMouseLUp=makeNewLevel;

	projectsNewLevelE=add("projectsNewLevelE","panel");
	projectsNewLevelE.panel->pos.x=410;
	projectsNewLevelE.panel->pos.y=-460;
	projectsNewLevelE.panel->pos.z=92;
	projectsNewLevelE.panel->color.x=0;
	projectsNewLevelE.panel->color.y=0;
	projectsNewLevelE.panel->color.z=0.75;
	projectsNewLevelE.panel->color.w=1;
	projectsNewLevelE.panel->scale.x=185;
	projectsNewLevelE.panel->scale.y=projectsNewLevel.textpanel->scale.y+20;
	projectsNewLevelE.panel->percent=false;
	projectsNewLevelE.panel->visible=false;
	projectsNewLevelE.panel->onMouseLUp=activateLevel;

	projectsNewLevelT=add("projectsNewLevelT","textpanel");
	projectsNewLevelT.textpanel->pos.x=420;
	projectsNewLevelT.textpanel->pos.y=-470;
	projectsNewLevelT.textpanel->pos.z=93;
	projectsNewLevelT.textpanel->color.x=1;
	projectsNewLevelT.textpanel->color.y=1;
	projectsNewLevelT.textpanel->color.z=1;
	projectsNewLevelT.textpanel->color.w=0.5;
	projectsNewLevelT.textpanel->setText("Enter Name Here");
	projectsNewLevelT.textpanel->percent=false;
	projectsNewLevelT.textpanel->visible=false;
	projectsNewLevelT.textpanel->clickable=false;

	if(!projectListLoaded){
		loadProjectDatabase();
	}

	if(projectList.size()==0){
		ObjectAddress temp=add("projectsList0","textpanel");
		temp.textpanel->pos.x=120;
		temp.textpanel->pos.y=-200;
		temp.textpanel->pos.z=93;
		temp.textpanel->color.x=1;
		temp.textpanel->color.y=1;
		temp.textpanel->color.z=1;
		temp.textpanel->color.w=1;
		temp.textpanel->setText("[None]");
		temp.textpanel->percent=false;
		temp.textpanel->visible=false;

		projectListT.pushBack(temp);
	}

	for(int i=0; i<projectList.size(); i++){
		ObjectAddress temp=add("projectsList"+String(i),"textpanel");
		temp.textpanel->pos.x=120;
		temp.textpanel->pos.y=-200;
		if(i>0){temp.textpanel->pos.y=projectListT[i-1].textpanel->pos.y-text.leading();}
		temp.textpanel->pos.z=93;
		temp.textpanel->color.x=1;
		temp.textpanel->color.y=1;
		temp.textpanel->color.z=1;
		temp.textpanel->color.w=1;
		temp.textpanel->setText(projectList[i]);
		temp.textpanel->percent=false;
		temp.textpanel->visible=false;

		projectListT.pushBack(temp);
	}

	ObjectAddress temp=add("levelList0","textpanel");
	temp.textpanel->pos.x=420;
	temp.textpanel->pos.y=-200;
	temp.textpanel->pos.z=93;
	temp.textpanel->color.x=1;
	temp.textpanel->color.y=1;
	temp.textpanel->color.z=1;
	temp.textpanel->color.w=1;
	temp.textpanel->setText("[None]");
	temp.textpanel->percent=false;
	temp.textpanel->visible=false;

	levelListT.pushBack(temp);
}

#endif

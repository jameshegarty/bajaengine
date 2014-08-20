#ifdef DEVELOPER

#include "editor.h"
#include "conf.h"
#include "Helperlib/HelperLibMath.hpp"
#include "timeing.hpp"
#include "os.h"
#include "text.hpp"
#include "mouse.h"

ObjectAddress alertsFade;
ObjectAddress alertBkg;
ObjectAddress alertText;
ObjectAddress alertYes;
ObjectAddress alertNo;
ObjectAddress alertCancel;
ObjectAddress editorErrorT;

ObjectAddress openTitle;
ObjectAddress openDir;

ObjectAddress openOK;
ObjectAddress openOKB;
ObjectAddress openCancel;
ObjectAddress openCancelB;

void (*yescallback)();
void (*nocallback)();
void (*opencallback)(String);

Array<String> editorErrorBuffer;
Array<int> editorErrorType;

Array<ObjectAddress> openFiles;

String selectedFile;

void openCancelCallback(){
	alertsFade.panel->visible=false;
	openTitle.textpanel->visible=false;
	openDir.textpanel->visible=false;

	openOK.textpanel->visible=false;
	openOKB.panel->visible=false;
	openCancel.textpanel->visible=false;
	openCancelB.panel->visible=false;

	for(int f=0; f<openFiles.size(); f++){
		openFiles[f].textpanel->visible=false;
	}
}

void openOpen(){
	opencallback(selectedFile);

	alertsFade.panel->visible=false;
	openTitle.textpanel->visible=false;
	openDir.textpanel->visible=false;

	openOK.textpanel->visible=false;
	openOKB.panel->visible=false;
	openCancel.textpanel->visible=false;
	openCancelB.panel->visible=false;

	for(int f=0; f<openFiles.size(); f++){
		openFiles[f].textpanel->visible=false;
	}
}

void Editor::openfile(String dir,String type,void (*oc)(String)){
	opencallback=oc;

	openOK.textpanel->visible=true;
	openOKB.panel->visible=true;
	openCancel.textpanel->visible=true;
	openCancelB.panel->visible=true;

	alertsFade.panel->visible=true;
	openTitle.textpanel->visible=true;
	openDir.textpanel->visible=true;
	openDir.textpanel->setText(dir);

	Array<Path> files=os().folderContents(dir);

	int fc=0;

	for(int i=0; i<files.size(); i++){

		bool addf=false;

		if(type=="textures"){
			if(files[i].getExtension()=="bmp"){
				addf=true;
			}else if(files[i].getExtension()=="tga"){
				addf=true;
			}else if(files[i].getExtension()=="jpg"){
				addf=true;
			}
		}

		if(addf){

			if(openFiles.size()<=fc){
				ObjectAddress fp=add("fileText","textpanel");
				fp.textpanel->pos.x=300;
				fp.textpanel->pos.z=99.1;
				openFiles.pushBack(fp);
			}

			
			
			openFiles[fc].textpanel->pos.y=-150-i*text.leading(0);
			
			openFiles[fc].textpanel->color.x=1;
			openFiles[fc].textpanel->color.y=1;
			openFiles[fc].textpanel->color.z=1;
			openFiles[fc].textpanel->color.w=1;
			openFiles[fc].textpanel->percent=false;
			openFiles[fc].textpanel->setText(files[i].getRelative());
			openFiles[fc].textpanel->visible=true;
			
			fc++;
		}
	}
}

void Editor::yesnocancel(String text,void (*yc)(),void (*nc)()){
	alertsFade.panel->visible=true;
	alertBkg.panel->visible=true;

	alertText.textpanel->visible=true;
	alertText.textpanel->setText(text);

	alertYes.textpanel->visible=true;
	alertNo.textpanel->visible=true;
	alertCancel.textpanel->visible=true;

	yescallback=yc;
	nocallback=nc;
}

void Editor::yescancel(String text,void (*yc)()){
	alertsFade.panel->visible=true;
	alertBkg.panel->visible=true;

	alertText.textpanel->visible=true;
	alertText.textpanel->setText(text);

	alertYes.textpanel->visible=true;
	alertNo.textpanel->visible=false;
	alertCancel.textpanel->visible=true;

	yescallback=yc;
}


void cancelCallback(){
	alertsFade.panel->visible=false;
	alertBkg.panel->visible=false;

	alertText.textpanel->visible=false;

	alertYes.textpanel->visible=false;
	alertNo.textpanel->visible=false;
	alertCancel.textpanel->visible=false;
}

void yesCallback(){
	yescallback();

	alertsFade.panel->visible=false;
	alertBkg.panel->visible=false;

	alertText.textpanel->visible=false;

	alertYes.textpanel->visible=false;
	alertNo.textpanel->visible=false;
	alertCancel.textpanel->visible=false;
}

void noCallback(){
	nocallback();

	alertsFade.panel->visible=false;
	alertBkg.panel->visible=false;

	alertText.textpanel->visible=false;

	alertYes.textpanel->visible=false;
	alertNo.textpanel->visible=false;
	alertCancel.textpanel->visible=false;
}

void Editor::editorError(String s){
	console().write("Editor Error: "+s);
	editorErrorBuffer.pushBack(s);
	editorErrorType.pushBack(1);
}

void Editor::editorWarning(String s){
	console().write("Editor Warning: "+s);
	editorErrorBuffer.pushBack(s);
	editorErrorType.pushBack(0);
}

float errcnt=2;

void Editor::drawAlerts(){
	if(editorErrorBuffer.size()>0){
		editorErrorT.textpanel->visible=true;
		editorErrorT.textpanel->pos.y=-conf->sizeY+30;
		editorErrorT.textpanel->pos.x=conf->sizeX-60;

		errcnt-=engineTime.time*0.02;

		editorErrorT.textpanel->color.w=errcnt;
		editorErrorT.textpanel->setText(editorErrorBuffer[0]);

		if(editorErrorType[0]){
			editorErrorT.textpanel->color.x=1;
			editorErrorT.textpanel->color.y=0;
			editorErrorT.textpanel->color.z=0;
		}else{
			editorErrorT.textpanel->color.x=1;
			editorErrorT.textpanel->color.y=1;
			editorErrorT.textpanel->color.z=1;
		}

		if(math.sin(errcnt)<0){
			editorErrorBuffer.erase(0);
			editorErrorType.erase(0);
			errcnt=2;
		}	
	}else{
		editorErrorT.textpanel->visible=false;
	}

	if(openTitle.textpanel->visible){
		for(int i=0; i<openFiles.size(); i++){
			if(openFiles[i].textpanel->visible){
				if(openFiles[i].textpanel->mouseover && mouse.lUp){
					selectedFile=openDir.textpanel->storedtext+openFiles[i].textpanel->storedtext;

					for(int f=0; f<openFiles.size(); f++){
						openFiles[f].textpanel->color.x=1;
						openFiles[f].textpanel->color.y=1;
						openFiles[f].textpanel->color.z=1;
					}

					openFiles[i].textpanel->color.x=1;
					openFiles[i].textpanel->color.y=0;
					openFiles[i].textpanel->color.z=0;

				}else{

				}
			}
		}
	}
}

void Editor::initAlerts(){
	alertsFade=add("alertsFade","panel");
	alertsFade.panel->pos.x=0;
	alertsFade.panel->pos.y=0;
	alertsFade.panel->pos.z=99;
	alertsFade.panel->color.x=0;
	alertsFade.panel->color.y=0;
	alertsFade.panel->color.z=0;
	alertsFade.panel->color.w=0.5;
	alertsFade.panel->percent=true;
	alertsFade.panel->scale.x=1;
	alertsFade.panel->scale.y=1;
	alertsFade.panel->visible=false;

	alertBkg=add("alertBkg","panel");
	alertBkg.panel->pos.x=100;
	alertBkg.panel->pos.y=-100;
	alertBkg.panel->pos.z=99.1;
	alertBkg.panel->color.x=0.25;
	alertBkg.panel->color.y=0.25;
	alertBkg.panel->color.z=0.5;
	alertBkg.panel->color.w=1;
	alertBkg.panel->percent=false;
	alertBkg.panel->scale.x=300;
	alertBkg.panel->scale.y=100;
	alertBkg.panel->visible=false;

	alertText=add("alertText","textpanel");
	alertText.textpanel->pos.x=120;
	alertText.textpanel->pos.y=-120;
	alertText.textpanel->pos.z=99.2;
	alertText.textpanel->color.x=1;
	alertText.textpanel->color.y=1;
	alertText.textpanel->color.z=1;
	alertText.textpanel->color.w=1;
	alertText.textpanel->percent=false;
	alertText.textpanel->setText("[none]");
	alertText.textpanel->visible=false;

	alertYes=add("alertYes","textpanel");
	alertYes.textpanel->pos.x=300;
	alertYes.textpanel->pos.y=-150;
	alertYes.textpanel->pos.z=99.2;
	alertYes.textpanel->color.x=1;
	alertYes.textpanel->color.y=1;
	alertYes.textpanel->color.z=1;
	alertYes.textpanel->color.w=1;
	alertYes.textpanel->percent=false;
	alertYes.textpanel->setText("Yes");
	alertYes.textpanel->visible=false;
	alertYes.textpanel->onMouseLUp=yesCallback;

	alertNo=add("alertNo","textpanel");
	alertNo.textpanel->pos.x=350;
	alertNo.textpanel->pos.y=-150;
	alertNo.textpanel->pos.z=99.2;
	alertNo.textpanel->color.x=1;
	alertNo.textpanel->color.y=1;
	alertNo.textpanel->color.z=1;
	alertNo.textpanel->color.w=1;
	alertNo.textpanel->percent=false;
	alertNo.textpanel->setText("No");
	alertNo.textpanel->visible=false;
	alertNo.textpanel->onMouseLUp=noCallback;

	alertCancel=add("alertCancel","textpanel");
	alertCancel.textpanel->pos.x=110;
	alertCancel.textpanel->pos.y=-150;
	alertCancel.textpanel->pos.z=99.2;
	alertCancel.textpanel->color.x=1;
	alertCancel.textpanel->color.y=1;
	alertCancel.textpanel->color.z=1;
	alertCancel.textpanel->color.w=1;
	alertCancel.textpanel->percent=false;
	alertCancel.textpanel->setText("Cancel");
	alertCancel.textpanel->visible=false;
	alertCancel.textpanel->onMouseLUp=cancelCallback;

	editorErrorT=add("editorErrorT","textpanel");
	editorErrorT.textpanel->pos.x=400;
	editorErrorT.textpanel->pos.y=-200;
	editorErrorT.textpanel->pos.z=99.2;
	editorErrorT.textpanel->color.x=1;
	editorErrorT.textpanel->color.y=0;
	editorErrorT.textpanel->color.z=0;
	editorErrorT.textpanel->color.w=1;
	editorErrorT.textpanel->percent=false;
	editorErrorT.textpanel->setText("Cancel");
	editorErrorT.textpanel->visible=false;
	editorErrorT.textpanel->align="right";

	openTitle=add("openTitle","textpanel");
	openTitle.textpanel->pos.x=110;
	openTitle.textpanel->pos.y=-150;
	openTitle.textpanel->pos.z=99.2;
	openTitle.textpanel->color.x=1;
	openTitle.textpanel->color.y=1;
	openTitle.textpanel->color.z=1;
	openTitle.textpanel->color.w=1;
	openTitle.textpanel->percent=false;
	openTitle.textpanel->setText("Open");
	openTitle.textpanel->visible=false;
	openTitle.textpanel->onMouseLUp=cancelCallback;

	openDir=add("openDir","textpanel");
	openDir.textpanel->pos.x=110;
	openDir.textpanel->pos.y=-180;
	openDir.textpanel->pos.z=99.2;
	openDir.textpanel->color.x=1;
	openDir.textpanel->color.y=1;
	openDir.textpanel->color.z=1;
	openDir.textpanel->color.w=1;
	openDir.textpanel->percent=false;
	openDir.textpanel->setText("Open");
	openDir.textpanel->visible=false;
	openDir.textpanel->onMouseLUp=cancelCallback;

	openOK=add("openOK","textpanel");
	openOK.textpanel->pos.x=640+19;
	openOK.textpanel->pos.y=-510;
	openOK.textpanel->pos.z=99.3;
	openOK.textpanel->color.x=1;
	openOK.textpanel->color.y=1;
	openOK.textpanel->color.z=1;
	openOK.textpanel->color.w=1;
	openOK.textpanel->setText("Open");
	openOK.textpanel->percent=false;
	openOK.textpanel->visible=false;
	openOK.textpanel->clickable=false;

	openOKB=add("openOKB","panel");
	openOKB.panel->pos.x=openOK.textpanel->pos.x-10;
	openOKB.panel->pos.y=openOK.textpanel->pos.y+10;
	openOKB.panel->pos.z=99.2;
	openOKB.panel->color.x=0;
	openOKB.panel->color.y=0;
	openOKB.panel->color.z=0.75;
	openOKB.panel->color.w=1;
	openOKB.panel->scale.x=openOK.textpanel->scale.x+20;
	openOKB.panel->scale.y=openOK.textpanel->scale.y+20;
	openOKB.panel->percent=false;
	openOKB.panel->visible=false;
	openOKB.panel->onMouseLUp=openOpen;

	openCancel=add("openCancel","textpanel");
	openCancel.textpanel->pos.x=120;
	openCancel.textpanel->pos.y=-510;
	openCancel.textpanel->pos.z=99.3;
	openCancel.textpanel->color.x=1;
	openCancel.textpanel->color.y=1;
	openCancel.textpanel->color.z=1;
	openCancel.textpanel->color.w=1;
	openCancel.textpanel->setText("Cancel");
	openCancel.textpanel->percent=false;
	openCancel.textpanel->visible=false;
	openCancel.textpanel->clickable=false;

	openCancelB=add("openCancelB","panel");
	openCancelB.panel->pos.x=openCancel.textpanel->pos.x-10;
	openCancelB.panel->pos.y=openCancel.textpanel->pos.y+10;
	openCancelB.panel->pos.z=99.2;
	openCancelB.panel->color.x=0;
	openCancelB.panel->color.y=0;
	openCancelB.panel->color.z=0.75;
	openCancelB.panel->color.w=1;
	openCancelB.panel->scale.x=openCancel.textpanel->scale.x+20;
	openCancelB.panel->scale.y=openCancel.textpanel->scale.y+20;
	openCancelB.panel->percent=false;
	openCancelB.panel->visible=false;
	openCancelB.panel->onMouseLUp=openCancelCallback;
}

#endif
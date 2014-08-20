#ifdef DEVELOPER

#include "editor.h"
#include "keyboard.h"
#include "EditorTextbox.h"
#include "script.hpp"
#include "os.h"

ObjectAddress scriptFade;
ObjectAddress mainScriptSelector;
ObjectAddress saveAndRun;
bool scriptVisible=false;
EditorTextbox textbox;


void saveAndRunCallback(){
	console().write("SAVEANDRUN");

	std::ofstream out;
	out.open(editor.mainScriptFile.c_str(),std::ios::out);
	out.write(editor.loadedMainScript.c_str(),editor.loadedMainScript.size());
	out.close();

	script().run(editor.loadedMainScript);
}

void Editor::drawScript(){

	if(deactivateGui("script")){
		scriptVisible=false;
	}else if(activateGui("script")){
		scriptVisible=true;
		textbox.string=&loadedMainScript;
	}

	if(scriptVisible){
		if(activateEvent("+",true,"script")){
			if(textbox.font==editor.monoLarge){
				textbox.font=editor.monoSmall;
			}else{
				textbox.font=editor.monoLarge;
			}
		}

		mainScriptSelector.textpanel->visible=true;
		mainScriptSelector.textpanel->setText(mainScriptFile);
		scriptFade.panel->visible=true;
		textbox.visible=true;
		textbox.perframe();
		textbox.color.x=1;
		textbox.color.y=1;
		textbox.color.z=1;
		textbox.color.w=1;
		saveAndRun.textpanel->visible=true;
	}else{
		mainScriptSelector.textpanel->visible=false;
		scriptFade.panel->visible=false;
		textbox.visible=false;
		textbox.perframe();
		saveAndRun.textpanel->visible=false;
	}
}

void Editor::initScript(){

	scriptFade=add("scriptFade","panel");
	scriptFade.panel->pos.x=0;
	scriptFade.panel->pos.y=0;
	scriptFade.panel->pos.z=90;
	scriptFade.panel->color.x=0;
	scriptFade.panel->color.y=0;
	scriptFade.panel->color.z=0;
	scriptFade.panel->color.w=0.5;
	scriptFade.panel->percent=true;
	scriptFade.panel->scale.x=1;
	scriptFade.panel->scale.y=1;
	scriptFade.panel->visible=false;

	mainScriptSelector=add("mainScriptSelector","textpanel");
	mainScriptSelector.textpanel->pos.x=50;
	mainScriptSelector.textpanel->pos.y=-30;
	mainScriptSelector.textpanel->pos.z=91;
	mainScriptSelector.textpanel->color.x=1;
	mainScriptSelector.textpanel->color.y=0;
	mainScriptSelector.textpanel->color.z=0;
	mainScriptSelector.textpanel->color.w=1;
	mainScriptSelector.textpanel->percent=false;
	mainScriptSelector.textpanel->visible=false;

	saveAndRun=add("saveAndRun","textpanel");
	saveAndRun.textpanel->pos.x=150;
	saveAndRun.textpanel->pos.y=-10;
	saveAndRun.textpanel->pos.z=91;
	saveAndRun.textpanel->color.x=0;
	saveAndRun.textpanel->color.y=1;
	saveAndRun.textpanel->color.z=0;
	saveAndRun.textpanel->color.w=1;
	saveAndRun.textpanel->setText("Save And Run");
	saveAndRun.textpanel->percent=false;
	saveAndRun.textpanel->visible=false;
	saveAndRun.textpanel->onMouseLUp=saveAndRunCallback;


	if(monoSmall==-1){
		Path smonoSmall;
		smonoSmall.setRelative("editorMonoSmall.lmf");

		bool fnd=true;

		if(os().fileExists(smonoSmall)){

		}else if(os().fileExists(String("../tools/editorMonoSmall.lmf"))){
			smonoSmall.setRelative("../tools/editorMonoSmall.lmf");
		}else if(os().fileExists(String("../../tools/editorMonoSmall.lmf"))){
			smonoSmall.setRelative("../../tools/editorMonoSmall.lmf");
		}else{
			fnd=false;
		}

		if(fnd){
			monoSmall=text.addFont(smonoSmall);
		}else{
			editorError("Small Monospaced font (editorMonoSmall.lmf) not found!");
		}
	}

	if(monoLarge==-1){
		Path smonoLarge;
		smonoLarge.setRelative("editorMonoLarge.lmf");

		bool fnd=true;

		if(os().fileExists(smonoLarge)){
			
		}else if(os().fileExists(String("../tools/editorMonoLarge.lmf"))){
			smonoLarge.setRelative("../tools/editorMonoLarge.lmf");
		}else if(os().fileExists(String("../../tools/editorMonoLarge.lmf"))){
			smonoLarge.setRelative("../../tools/editorMonoLarge.lmf");
		}else{
			fnd=false;
		}

		if(fnd){
			monoLarge=text.addFont(smonoLarge);
		}else{
			editorError("Large Monospaced font (editorMonoLarge.lmf) not found!");
		}
	}

	textbox.init();
	textbox.pos.z=91;
	textbox.pos.x=150;
	textbox.pos.y=-30;

	if(editor.monoLarge){
		textbox.font=editor.monoLarge;
	}
}

#endif
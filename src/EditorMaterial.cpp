#ifdef DEVELOPER

#include "editor.h"
#include "keyboard.h"
#include "EditorTextbox.h"
#include "script.hpp"
#include "level.hpp"
#include "mouse.h"

ObjectAddress materialFade;
bool materialVisible=false;

Array<String> shaderOptions;
Array<ObjectAddress> soPanel;
Map<String,Array<ObjectAddress> > optTitle;
Map<String,Array<ObjectAddress> > optBitmaps;
Map<String,Array<String> > subOptions;
Map<String,Array<String> > subOptType;

Array<ObjectAddress> matList;

/*
Standard 
	diffuse
	spec+diffuse
	diffuse+displacement
	diffuse+spec+bump+displacement
	Reflection
Multitexturing
Environment Map
Water
Custom
*/

void setStandard(){
	Material* mat=&level->materials[selectedObjects[0]->material];
	mat->shader=0;
	selectedObjects[0]->dirty=true;
}

void setMultitexturing(){
	Material* mat=&level->materials[selectedObjects[0]->material];
	mat->shader=MATERIAL_SHADER_MIXER;
	selectedObjects[0]->dirty=true;
}

void setEnvironmentMap(){
	Material* mat=&level->materials[selectedObjects[0]->material];
	mat->shader=MATERIAL_SHADER_ENVIRONMENT;
	selectedObjects[0]->dirty=true;
}

void setWater(){
	level->water=selectedObjects[0];
}

Array<int> getStandardTexids(){
	Array<int> final;

	Material* mat=&level->materials[selectedObjects[0]->material];

	final.pushBack(mat->textureDiffuse);

	if(final[0]!=-1){
		final[0]=level->textures[final[0]].glName();
	}

	final.pushBack(mat->textureSpecular);

	if(final[1]!=-1){
		final[1]=level->textures[final[1]].glName();
	}

	final.pushBack(mat->textureDisplacement);

	if(final[2]!=-1){
		final[2]=level->textures[final[2]].glName();
	}

	final.pushBack(mat->textureBump);
	
	if(final[3]!=-1){
		final[3]=level->textures[final[3]].glName();
	}

	final.pushBack(mat->textureDiffuse);
	final.pushBack(mat->textureDiffuse);

	return final;
}

Array<int> getMultitexturingTexids(){
	Array<int> final;

	Material* mat=&level->materials[selectedObjects[0]->material];

	final.pushBack(mat->textureDiffuse);
	final.pushBack(mat->textureEnvironmentFront);
	final.pushBack(mat->textureEnvironmentBack);
	final.pushBack(mat->textureEnvironmentLeft);
	final.pushBack(mat->textureEnvironmentRight);
	final.pushBack(mat->textureEnvironmentTop);
	final.pushBack(mat->textureEnvironmentBottom);

	for(int i=0; i<final.size(); i++){
		if(final[i]!=-1){
			final[i]=level->textures[final[i]].glName();
		}
	}

	return final;
}


Array<int> getEnvironmentMapTexids(){
	Array<int> final;

	Material* mat=&level->materials[selectedObjects[0]->material];

	final.pushBack(mat->textureDiffuse);
	final.pushBack(mat->textureEnvironmentFront);
	final.pushBack(mat->textureEnvironmentBack);
	final.pushBack(mat->textureEnvironmentLeft);
	final.pushBack(mat->textureEnvironmentRight);
	final.pushBack(mat->textureEnvironmentTop);
	final.pushBack(mat->textureEnvironmentBottom);

	for(int i=0; i<final.size(); i++){
		if(final[i]!=-1){
			final[i]=level->textures[final[i]].glName();
		}
	}

	return final;
}

Array<int> getWaterTexids(){
	Array<int> final;

	Material* mat=&level->materials[selectedObjects[0]->material];

	final.pushBack(mat->textureDiffuse);
	final.pushBack(mat->textureBump);
	final.pushBack(mat->textureDisplacement);

	for(int i=0; i<final.size(); i++){
		if(final[i]!=-1){
			final[i]=level->textures[final[i]].glName();
		}
	}

	return final;

}
int applyMaterial=-1;
String applyType;

bool dirty;

void openCallback(String s){
	console().write("apply "+s);

	level->materials[applyMaterial].addTexture(applyType,s);

	unsigned int* shader=&level->materials[applyMaterial].shader;

	if(level->materials[applyMaterial].textureDisplacement!=-1){
		
		if( !(*shader & MATERIAL_SHADER_DISPLACEMENT) && !(*shader & MATERIAL_SHADER_MIXER)){
			*shader|=MATERIAL_SHADER_DISPLACEMENT;
		}	
	}

	if(level->materials[applyMaterial].textureSpecular!=-1){
		
		if( !(*shader & MATERIAL_SHADER_SPECULAR) && !(*shader & MATERIAL_SHADER_MIXER) ){
			*shader|=MATERIAL_SHADER_SPECULAR;
		}	
	}


	dirty=true;
}

void getShader(Material* mat, String* SelShader, Array<int>* texids){
	if(mat->shader==MATERIAL_SHADER_MIXER){
		*SelShader="Multitexturing";
		*texids=getMultitexturingTexids();
	}else if(mat->shader==MATERIAL_SHADER_ENVIRONMENT){
		*SelShader="Environment Map";
		*texids=getEnvironmentMapTexids();
	}else if(level->water==selectedObjects[0]){
		*SelShader="Water";
		*texids=getWaterTexids();
	}else{
		*SelShader="Standard";
		*texids=getStandardTexids();
	}
}

void Editor::drawMaterial(){

	String SelShader="Standard";

	Array<int> texids;
	Material* mat;

	if(materialVisible){
		mat=&level->materials[selectedObjects[0]->material];

		getShader(mat,&SelShader,&texids);

		for(int i=0; i<soPanel.size(); i++){
			if(soPanel[i].textpanel->mouseover && mouse.lUp){

				if(i==0){
					setStandard();
					dirty=true;
				}else if(i==1){
					setMultitexturing();
					dirty=true;
				}else if(i==2){
					setEnvironmentMap();
					dirty=true;
				}else if(i==3){
					setWater();
					dirty=true;
				}
			}
		}


		for(int i=0; i<optBitmaps[SelShader].size(); i++){
			if(optBitmaps[SelShader][i].panel->mouseover && mouse.lUp){
				String dir=projectList[loadedProject]+String("/")+levelList[loadedProject][loadedLevel]+String("/");
				applyMaterial=selectedObjects[0]->material;
				applyType=optTitle[SelShader][i].textpanel->storedtext;
				openfile(dir,"textures",openCallback);
			}
		}

		for(int i=0; i<level->materialCount; i++){
			if(i>matList.size()-1){
				ObjectAddress temp=add("mat"+i,"textpanel");
				temp.textpanel->pos.x=150;
				temp.textpanel->pos.y=-40-i*text.leading();
				temp.textpanel->pos.z=91;
				temp.textpanel->color.x=1;
				temp.textpanel->color.y=1;
				temp.textpanel->color.z=1;
				temp.textpanel->color.w=1;
				temp.textpanel->font=0;
				temp.textpanel->percent=false;
				temp.textpanel->visible=true;
				temp.textpanel->align="right";
				temp.textpanel->setText(level->materials[i].name);
				

				matList.pushBack(temp);
			}

			matList[i].textpanel->visible=true;

			if(selectedObjects[0]->material==i){
				matList[i].textpanel->color.y=0;
				matList[i].textpanel->color.z=0;
			}else{
				matList[i].textpanel->color.y=1;
				matList[i].textpanel->color.z=1;
			}

			if(matList[i].textpanel->mouseover && mouse.lUp){
				level->applyMaterial(selectedObjects[0]->name,level->materials[i].name);
				dirty=true;
			}
		}
	}

	if(activateGui("material") || dirty){
		materialVisible=true;

		materialFade.panel->visible=true;

		mat=&level->materials[selectedObjects[0]->material];

		getShader(mat,&SelShader,&texids);

		for(int i=0; i<soPanel.size(); i++){
			bool as=false;

			if(shaderOptions[i]==SelShader){
				as=true;
			}

			if(as){
				soPanel[i].textpanel->color.y=0;
				soPanel[i].textpanel->color.z=0;
			}else{
				soPanel[i].textpanel->color.y=1;
				soPanel[i].textpanel->color.z=1;
			}

			soPanel[i].textpanel->visible=true;

			for(int m=0; m<optTitle[shaderOptions[i]].size(); m++){
				optTitle[shaderOptions[i]][m].textpanel->visible=false;
			}

			for(int m=0; m<optBitmaps[shaderOptions[i]].size(); m++){
				optBitmaps[shaderOptions[i]][m].panel->visible=false;
			}
		}

		for(int i=0; i<optTitle[SelShader].size(); i++){
			optTitle[SelShader][i].textpanel->visible=true;
		}

		for(int i=0; i<optBitmaps[SelShader].size(); i++){
			optBitmaps[SelShader][i].panel->visible=true;
			optBitmaps[SelShader][i].panel->forceTexture(texids[i]);
		}


		

	}else if(deactivateGui("material")){
		materialVisible=false;

		materialFade.panel->visible=false;

		
		for(int i=0; i<soPanel.size(); i++){
			soPanel[i].textpanel->visible=false;

			for(int m=0; m<optTitle[shaderOptions[i]].size(); m++){
				optTitle[shaderOptions[i]][m].textpanel->visible=false;
			}

			for(int m=0; m<optBitmaps[shaderOptions[i]].size(); m++){
				optBitmaps[shaderOptions[i]][m].panel->visible=false;
			}
		}

		for(int i=0; i<matList.size(); i++){
			matList[i].textpanel->visible=false;
		}
	}

	dirty=false;
}

void Editor::initMaterial(){

	materialFade=add("materialFade","panel");
	materialFade.panel->pos.x=0;
	materialFade.panel->pos.y=0;
	materialFade.panel->pos.z=90;
	materialFade.panel->color.x=0;
	materialFade.panel->color.y=0;
	materialFade.panel->color.z=0;
	materialFade.panel->color.w=0.5;
	materialFade.panel->percent=true;
	materialFade.panel->scale.x=1;
	materialFade.panel->scale.y=1;
	materialFade.panel->visible=false;

	shaderOptions.pushBack("Standard");
		subOptions["Standard"].pushBack("Diffuse");
		subOptType["Standard"].pushBack("Bitmap");

		subOptions["Standard"].pushBack("Specular");
		subOptType["Standard"].pushBack("Bitmap");

		subOptions["Standard"].pushBack("Displacement");
		subOptType["Standard"].pushBack("Bitmap");

		subOptions["Standard"].pushBack("Bump");
		subOptType["Standard"].pushBack("Bitmap");

		subOptions["Standard"].pushBack("Reflection");
		subOptType["Standard"].pushBack("Bitmap");

		subOptions["Standard"].pushBack("Shinyness");
		subOptType["Standard"].pushBack("Bitmap");

	shaderOptions.pushBack("Multitexturing");
		subOptions["Multitexturing"].pushBack("Base");
		subOptType["Multitexturing"].pushBack("Bitmap");

		subOptions["Multitexturing"].pushBack("Color 1");
		subOptType["Multitexturing"].pushBack("Bitmap");

		subOptions["Multitexturing"].pushBack("Mask 1");
		subOptType["Multitexturing"].pushBack("Bitmap");

		subOptions["Multitexturing"].pushBack("Color 2");
		subOptType["Multitexturing"].pushBack("Bitmap");

		subOptions["Multitexturing"].pushBack("Mask 2");
		subOptType["Multitexturing"].pushBack("Bitmap");

		subOptions["Multitexturing"].pushBack("Color 3");
		subOptType["Multitexturing"].pushBack("Bitmap");

		subOptions["Multitexturing"].pushBack("Mask 3");
		subOptType["Multitexturing"].pushBack("Bitmap");

	shaderOptions.pushBack("Environment Map");
		subOptions["Environment Map"].pushBack("Diffuse");
		subOptType["Environment Map"].pushBack("Bitmap");

		subOptions["Environment Map"].pushBack("Front");
		subOptType["Environment Map"].pushBack("Bitmap");

		subOptions["Environment Map"].pushBack("Back");
		subOptType["Environment Map"].pushBack("Bitmap");

		subOptions["Environment Map"].pushBack("Left");
		subOptType["Environment Map"].pushBack("Bitmap");

		subOptions["Environment Map"].pushBack("Right");
		subOptType["Environment Map"].pushBack("Bitmap");

		subOptions["Environment Map"].pushBack("Top");
		subOptType["Environment Map"].pushBack("Bitmap");

		subOptions["Environment Map"].pushBack("Bottom");
		subOptType["Environment Map"].pushBack("Bitmap");

	shaderOptions.pushBack("Water");
		subOptions["Water"].pushBack("Diffuse");
		subOptType["Water"].pushBack("Bitmap");

		subOptions["Water"].pushBack("Bump");
		subOptType["Water"].pushBack("Bitmap");

		subOptions["Water"].pushBack("Displacement");
		subOptType["Water"].pushBack("Bitmap");

	shaderOptions.pushBack("Custom");
		subOptions["Custom"].pushBack("Shader");
		subOptType["Custom"].pushBack("Bitmap");

		subOptions["Custom"].pushBack("Texture 1");
		subOptType["Custom"].pushBack("Bitmap");

		subOptions["Custom"].pushBack("Texture 2");
		subOptType["Custom"].pushBack("Bitmap");

		subOptions["Custom"].pushBack("Texture 3");
		subOptType["Custom"].pushBack("Bitmap");

		subOptions["Custom"].pushBack("Texture 4");
		subOptType["Custom"].pushBack("Bitmap");

		subOptions["Custom"].pushBack("Texture 5");
		subOptType["Custom"].pushBack("Bitmap");

		subOptions["Custom"].pushBack("Texture 6");
		subOptType["Custom"].pushBack("Bitmap");


	for(int i=0; i<shaderOptions.size(); i++){
		ObjectAddress oa=add("shaderOptions"+i,"textpanel");
		float past=170;

		if(i>0){
			past=soPanel[i-1].textpanel->pos.x+soPanel[i-1].textpanel->scale.x+40;
		}

		oa.textpanel->pos.x=past;
		oa.textpanel->pos.y=-15;
		oa.textpanel->pos.z=91;
		oa.textpanel->color.x=1;
		oa.textpanel->color.y=1;
		oa.textpanel->color.z=1;
		oa.textpanel->color.w=1;
		oa.textpanel->percent=false;
		oa.textpanel->visible=false;
		oa.textpanel->align="left";
		oa.textpanel->font=0;
		oa.textpanel->setText(shaderOptions[i]);

		float adv=0;

		for(int j=0; j<subOptions[shaderOptions[i]].size(); j++){
			ObjectAddress opt=add("subShaderOptions"+j,"textpanel");
			opt.textpanel->pos.x=170;
			opt.textpanel->pos.y=-40-adv;
			opt.textpanel->pos.z=91;
			opt.textpanel->color.x=1;
			opt.textpanel->color.y=1;
			opt.textpanel->color.z=1;
			opt.textpanel->color.w=1;
			opt.textpanel->percent=false;
			opt.textpanel->visible=false;
			opt.textpanel->align="left";
			opt.textpanel->font=0;
			opt.textpanel->setText(subOptions[shaderOptions[i]][j]);

			if(subOptType[shaderOptions[i]][j]=="Bitmap"){

				ObjectAddress optb=add("subShaderOptionsBitmap"+String(j),"panel");
				optb.panel->pos.x=190;
				optb.panel->pos.y=-40-adv-30;
				optb.panel->pos.z=91;
				optb.panel->color.x=1;
				optb.panel->color.y=1;
				optb.panel->color.z=1;
				optb.panel->color.w=1;
				optb.panel->scale.x=60;
				optb.panel->scale.y=60;
				optb.panel->percent=false;
				optb.panel->visible=false;

				optBitmaps[shaderOptions[i]].pushBack(optb);

				adv+=100;
			}

			optTitle[shaderOptions[i]].pushBack(opt);
		}

		soPanel.pushBack(oa);
	}

	
}

#endif
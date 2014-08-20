#include "GlHeader.hpp"

#include "mouse.h"
#include "level.hpp"
#include "picking.h"

#include "conf.h"

#include "picker_matrix.h"
#include "Helperlib/sort.hpp"

GlPickerMatrix pickerMatrix;

bool pointInRectangleR(FloatVector2d point, float negativeX, float positiveX, float negativeY, float positiveY){
	if(point.x>positiveX){return false;}
	if(point.x<negativeX){return false;}
	if(point.y>positiveY){return false;}
	if(point.y<negativeY){return false;}

	return true;
}

void GlPickerMatrix::clear(){
	objects.clear();
	ids.clear();
}


void GlPickerMatrix::add(int id, Object* obj){
	objects.pushBack(obj);
	ids.pushBack(id);
}


Array<int> GlPickerMatrix::pick(){

	GLint viewport[4];							// Space For Viewport Data
	GLdouble mvmatrix[16], projmatrix[16];					// Space For Transform Matrix
	GLdouble winx, winy, winz;						// Space For Returned Projected Coords

	glGetIntegerv (GL_VIEWPORT, viewport);					// Get Actual Viewport
	glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);				// Get Actual Model View Matrix
	glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);			// Get Actual Projection Matrix


	float oNx=0;
	float oNy=0;
	float oPx=0;
	float oPy=0;
	
	bool oNxSet;
	bool oNySet;
	bool oPxSet;
	bool oPySet;

	float px,py,pz;
	float nZ;

	bool nZSet;

	Array<Object*> finalObjs;
	Array<int> finalIds;
	Array<float> finalpz;

	for(int i=0; i<objects.size(); i++){
		
		oNxSet=false;
		oNySet=false;
		oPxSet=false;
		oPySet=false;
		nZSet=false;

		for(int r=0; r<8; r++){

			if(r==0){
				px=objects[i]->box.px;
				py=objects[i]->box.py;
				pz=objects[i]->box.nz;
			}else if(r==1){
				px=objects[i]->box.nx;
				py=objects[i]->box.py;
				pz=objects[i]->box.nz;
			}else if(r==2){
				px=objects[i]->box.nx;
				py=objects[i]->box.py;
				pz=objects[i]->box.pz;
			}else if(r==3){
				px=objects[i]->box.px;
				py=objects[i]->box.py;
				pz=objects[i]->box.pz;
			}else if(r==4){
				px=objects[i]->box.px;
				py=objects[i]->box.ny;
				pz=objects[i]->box.nz;
			}else if(r==5){
				px=objects[i]->box.nx;
				py=objects[i]->box.ny;
				pz=objects[i]->box.nz;
			}else if(r==6){
				px=objects[i]->box.nx;
				py=objects[i]->box.ny;
				pz=objects[i]->box.pz;
			}else if(r==7){
				px=objects[i]->box.px;
				py=objects[i]->box.ny;
				pz=objects[i]->box.pz;
			}

			gluProject(px,py,pz, mvmatrix, projmatrix, viewport, &winx, &winy, &winz);

			if(winx>oPx || !oPxSet){oPx=winx;oPxSet=true;}
			if(winx<oNx || !oNxSet){oNx=winx;oNxSet=true;}
			if(winy<oPy || !oPySet){oPy=winy;oPySet=true;}
			if(winy>oNy || !oNySet){oNy=winy;oNySet=true;}

			if(winz<nZ || !nZSet){nZ=winz;nZSet=true;}
		}

		if(pointInRectangleR(FloatVector2d(posX,posY),oNx,oPx,-oNy,-oPy)){
			finalObjs.pushBack(objects[i]);
			finalIds.pushBack(ids[i]);
			finalpz.pushBack(nZ);
		}
	}

	Array<SortItem> si;

	for(int i=0; i<finalObjs.size(); i++){
		si.pushBack(SortItem(finalIds[i],finalpz[i]));
	}

	Array<int> ff=sort.sort(si,true);
	
	return ff;
}
#include "picking.h"

#ifdef LEGACY_PICKER


#ifdef _WIN32
#include <windows.h>	// Header File For Windows
#endif

#include "GlHeader.hpp"
#include "mouse.h"
#include "level.hpp"


#include "conf.h"


GlPicker glPicker;

GlPicker::GlPicker(){
	sizeX=1;
	sizeY=1;
	posLevel=0;
	debug=false;
	sort=PICKER_INDEX_ASC;
	sort=PICKER_DEPTH;

	clipNear=0;
	clipFar=100;

}

bool GlPicker::pushId(int id){
	#ifdef DEBUG10
	logs().main.write("Pricker - Push Id: "+stringFunctions.toString(id)+" Level: "+stringFunctions.toString(level));
	#endif
	
	glPushName(id);

	return true;
}

int GlPicker::push(){

	pos.pushBack();
	pos[posLevel]=0;
	posLevel++;
	
	#ifdef DEBUG10
	logs().main.write("picker push "+stringFunctions.toString(pos[posLevel]));
	#endif
	
	glPushName(pos[posLevel]);
	
	return 0;
}

int GlPicker::change(){
	return 0;
}

bool GlPicker::changeId(int id){
	glLoadName(id);
	return true;
}



bool GlPicker::pop(){
	glPopName();
	return true;
}

bool GlPicker::clear(){
	for(int i=0; i<posLevel; i++){
		pop();
	}
	
	result.clear();
	pos.clear();
	
	posLevel=0;

	return true;
}

GLint	viewportSTORE[4];

bool GlPicker::start(){
	// Set Up A Selection Buffer
	active=true;
	
	// The Size Of The Viewport. [0] Is <x>, [1] Is <y>, [2] Is <length>, [3] Is <width>
	GLint	viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewportSTORE);

	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	// This Sets The Array <viewport> To The Size And Location Of The Screen Relative To The Window
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(512, buffer);						// Tell OpenGL To Use Our Array For Selection

	// Puts OpenGL In Selection Mode. Nothing Will Be Drawn.  Object ID's and Extents Are Stored In The Buffer.
	// COMMENT NEXT LINE TO DEBUG

	if(!debug){
		glRenderMode(GL_SELECT);
	}
	
	glInitNames();								// Initializes The Name Stack

	shader.reset();	//this IS important, it does fix some bugs lol

	glMatrixMode(GL_TEXTURE);	
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);						// Selects The Projection Matrix
	glPushMatrix();								// Push The Projection Matrix
	glLoadIdentity();							// Resets The Matrix

	gluPickMatrix((GLdouble) posX, (GLdouble) viewport[3]+posY, sizeX, sizeY, viewport);


	if(perspective=="ortho"){
		glOrtho(0,conf->sizeX,-conf->sizeY,0,clipFar,clipNear);
	}else{

		tf=level->camera->clipFar;
		tn=level->camera->clipNear;

		float ttn=clipNear;

		if(ttn<1){ttn=1;}

		level->camera->clipFar=clipFar;
		level->camera->clipNear=ttn;

		//the "depth" values in the result buffer SHOULD be xbox huger, don't flip the clips to correct them
		/*The second and third fields represent the minimum and maximum depth of the hit. Note that only the vertices of the primitives that intersect the 
		viewing volume are taken into account. For polygons that get clipped OpenGL reconstructs the vertices. So basically you get the maximum and minimum 
		depths of the rendered segment of the primitives that intersects the viewing volume, not the minimum and maximum depths of the entire primitive. The 
		depth is taken from the Z buffer (where it lies in the range [0,1]), it gets multiplied by 2^32 -1 and it is rounded to the nearest integer. Note that 
		the depths you get are not linearly proportional to the distance to the viewpoint due to the nonlinear nature of the z buffer.*/

		glMatrixMode(GL_PROJECTION);
		level->camera->projectionMatrix();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		level->camera->transform();
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);		//this is really freakin important!!!!

		level->camera->clipFar=tf;
		level->camera->clipNear=tn;
	}
	
	return true;
}
	
bool GlPicker::stop(){
	
	
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();								// Pop The Projection Matrix
	glMatrixMode(GL_MODELVIEW);						// Select The Modelview Matrix.
	//glPopMatrix(); //we don't want to do this
	
	glViewport (viewportSTORE[0],viewportSTORE[1],viewportSTORE[2],viewportSTORE[3]);


	GLint hits=glRenderMode(GL_RENDER);						// Switch To Render Mode, Find Out How Many hits
 	
 	
	active=false;
	
	clear();
	
	if (hits > 0){
		// hits is the number of hit "groups"
		// [0] is the number of names
		// [1] min depth
		// [2] max depth
		// [3+] name of object (the number of sections is the number in [0])
		// and then it repeats
		
		if(sort==PICKER_INDEX_ASC || sort==PICKER_INDEX_DESC || sort==PICKER_INDEX2_DESC){
			//sorts based on smallest id (would be easy to make it choose bigest)
			
			//prefetch
			int levels=int(buffer[0]);
			int bufferPos=0;  //holds the start of each hit
			
			while(levels>result.size()){
					result.pushBack();
					result[result.size()-1]=0;
				}
				
			for(int tempIndex=0; tempIndex<levels; tempIndex++){
				result[tempIndex]=int(buffer[bufferPos+3+tempIndex]);
			}
					
     		//loop	
     		bool replace=false;
     		
			for( int i=0; i<hits; i++){

				
				replace=false;
				
				//correct
				while(levels>result.size()){
					result.pushBack();
					result[result.size()-1]=0;
					replace=true;
				}
				
				//see if we should replace, ie sort algorithm
				
				if(sort==PICKER_INDEX_ASC){
					for(int tempIndex=0; tempIndex<levels; tempIndex++){
					
						if(result[tempIndex]>buffer[bufferPos+3+tempIndex]){  //CHANGE THIS TO SORT DESC
     						replace=true;
          				}

					}
				}else if(sort==PICKER_INDEX_DESC){
					for(int tempIndex=0; tempIndex<levels; tempIndex++){
						if(result[tempIndex]<buffer[bufferPos+3+tempIndex]){  //CHANGE THIS TO SORT ASC
     						replace=true;
          				}
      				}
				}else if(sort==PICKER_INDEX2_DESC){
					if(levels>=2){
						if(result[1]<buffer[bufferPos+3+1]){ 
     						replace=true;
          				}
          			}
				}
				
				if(replace){

					
					for(int tempIndex=0; tempIndex<levels; tempIndex++){
						result[tempIndex]=int(buffer[bufferPos+3+tempIndex]);
					}
				}

				bufferPos=bufferPos+3+levels;
				
				levels=int(buffer[bufferPos]);
			}
			
		}else if(sort==PICKER_DEPTH){
			//sorts based on smallest id (would be easy to make it choose bigest)
			
			//prefetch
			int levels=int(buffer[0]);
			int bufferPos=0;  //holds the start of each hit
			Array<int> depth;
			
			while(levels>result.size()){
					result.pushBack();
					result[result.size()-1]=0;
					
					depth.pushBack();
					depth[depth.size()-1]=0;
				}
				
			for(int tempIndex=0; tempIndex<levels; tempIndex++){
				result[tempIndex]=int(buffer[bufferPos+3+tempIndex]);
				depth[tempIndex]=int(buffer[bufferPos+1]);
			}
					
     		//loop	
     		bool replace=false;
     		
			for( int i=0; i<hits; i++){

				
				replace=false;
				
				//correct
				while(levels>result.size()){
					result.pushBack();
					result[result.size()-1]=0;
					
					depth.pushBack();
					depth[depth.size()-1]=0;
					
					replace=true;
				}
				
				//see if we should replace
				
				
				for(int tempIndex=0; tempIndex<levels; tempIndex++){
					/*if(result[tempIndex]>buffer[bufferPos+3+tempIndex]){  //CHANGES THIS TO SORT DESC
     					replace=true;
          			}*/
          			
          			if(depth[tempIndex]>buffer[bufferPos+1]){  //CHANGES THIS TO SORT DESC
     					replace=true;
          			}

					//because the clip near/far is flipped (for some reason) we have to sort this ASC
					/*if(depth[tempIndex]<buffer[bufferPos+1]){  //SORT ASC
     					replace=true;
          			}*/
					
				}
				
				if(replace){

					
					for(int tempIndex=0; tempIndex<levels; tempIndex++){
						result[tempIndex]=int(buffer[bufferPos+3+tempIndex]);
					}
				}

				bufferPos=bufferPos+3+levels;
				
				levels=int(buffer[bufferPos]);
			}
			
			int lol=0;
		}	
	}
	
 	#ifdef DEBUG10	
	if(result.size()==0){
		logs().main.write("Pick Result - None");
	}

  	#endif
	
  	for(int i=0; i<512; i++){  //clear buffer (probably inefficiant)
 		buffer[i]=0;
  	}
  	

	if(perspective=="ortho"){
		
	}else{
		
	}

	return true;
}


#endif
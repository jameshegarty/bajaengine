#include "GlHeader.hpp"
										
#include "draw.hpp"
#include "DrawDynamic.h"
#include "level.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "mouse.h"
#include "picking.h"
#include "hardware.hpp"
#include "random.h"

#include "Log.hpp"
#include "sort.hpp"
#include "HelperLibMath.hpp"

#include "lightmap.h"

#ifdef _WIN32
	#include <math.h>
#endif

#include "picker_matrix.h"

Array<Object*> v;

void buildTransparentList(){

	Array<Object*> a;
	a.pushBack();

	for(int i=0; i<level->transparentObjects.size(); i++){
 
		if( !(level->transparentObjects[i]->flags & OBJECT_STATIC) || !level->transparentObjects[i]->listBuilt){	//don't bother updating nonstatic objects

			if(!level->transparentObjects[i]->listBuilt || level->transparentObjects[i]->dirty){

				/*Object* object = level->transparentObjects[i];
				int vcount=object->getVertexCount();
				
				std::vector<GLfloat> ver(0);
				//ver.reserve(vcount*3);
				for(unsigned int v=0; v<vcount; v++){			
					Vertex* vertex=&(object->getVertexRaw(v));
					
					ver.push_back(vertex->pos.x);
					ver.push_back(vertex->pos.y);
					
					ver.push_back(vertex->pos.z);
					
				}
				
				assert(ver.size() == vcount*3);
				
				glGenBuffers(1, &(object->vboId));
				glBindBuffer(GL_ARRAY_BUFFER, object->vboId);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vcount, &(ver[0]), GL_STATIC_DRAW);
		*/
				/////////
				
				glDeleteLists(level->transparentObjects[i]->glListName,1);

				level->transparentObjects[i]->glListName=glGenLists(1);
				glNewList(level->transparentObjects[i]->glListName,GL_COMPILE);
				a[0]=level->transparentObjects[i];

				drawArray(&a,false);

				glEndList();	
 
				level->transparentObjects[i]->listBuilt=true;

			}
		}
	}


}


void drawTransparent(float yvalue,bool above,bool nowater){
	Array<Object*> final;

	v.clear();

	Array<SortItem> si;

	float d=0;

	Plane3d p;
	p.x=level->camera->pos.x;
	p.y=level->camera->pos.y;
	p.z=level->camera->pos.z;
	p.normal=math.degreesToNormal(level->camera->rot);

	if(nowater){	

		for(int i=0;  i<level->transparentObjects.size(); i++){

			if(level->transparentObjects[i]->visible && 
				level->materials[level->transparentObjects[i]->material].diffuse.w>0  && 
				!(level->transparentObjects[i]->flags&OBJECT_WATER)){

				v.pushBack(level->transparentObjects[i]);
				d=level->transparentObjects[i]->box.closestDistanceSigned(p);
				si.pushBack(SortItem(v.size()-1,d));
			}
		}
	}else{
		for(int i=0;  i<level->transparentObjects.size(); i++){

			if(level->transparentObjects[i]->visible && 
				level->materials[level->transparentObjects[i]->material].diffuse.w>0  && 
				!(level->transparentObjects[i]->flags&OBJECT_WATER)){

					if(above && level->transparentObjects[i]->pos.y>=yvalue){
		

						d=level->transparentObjects[i]->box.closestDistanceSigned(p);

						#ifdef _WIN32
						if(d>0){
						#else
						if(true){
						#endif
							v.pushBack(level->transparentObjects[i]);

							si.pushBack(SortItem(v.size()-1,d));

				
						}

					}else if(!above && level->transparentObjects[i]->pos.y<yvalue){
						

						d=level->transparentObjects[i]->box.closestDistanceSigned(p);

						v.pushBack(level->transparentObjects[i]);

						si.pushBack(SortItem(v.size()-1,d));

					}else{

					}
			}
		}
	}

	if(si.size()==0){
		return;
	}

	Array<int> f=sort.sort(si,false);


	Array<SortItem> si2;

	for(int r=0; r<f.size(); r++){
		si2.pushBack(SortItem(f[r],2));
	}

	Array<String> s;
	for( int i=0; i<si2.size(); i++){
		s.pushBack(v[si2[i].key]->name);
	}	


	for(int i=0; i<f.size(); i++){
		final.pushBack(v[f[i]]);
	}

	Array<String> s2;
	for( int i=0; i<final.size(); i++){
		s2.pushBack(final[i]->name);
	}	

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

	//it's better for stuff to get drawn in the wrong order than not drawn at all...
	glDepthMask(0);

	shader.reset();

	if(conf->optimize=="none"){


		for(int i=0; i<final.size(); i++){
			Array<Object*> t;
			t.pushBack(final[i]);

			if(level->materials[final[i]->material].shader & MATERIAL_SHADER_ENVIRONMENT){

				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);

				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);

				environmentMapSetup(final[i]);
			}else{

				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);

				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);
			}

			drawArray(&t,true);
		}

	}else if(conf->optimize=="list" || conf->optimize=="modern"){
		buildTransparentList();
		
		for(int i=0; i<final.size(); i++){

			if(level->materials[final[i]->material].shader & MATERIAL_SHADER_ENVIRONMENT &&
#ifdef FEATURE_CUBIC_ENVIRONMENT_MAPS
true
#else
false
#endif
			   ){

				assert(false);

				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);

				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);

				environmentMapSetup(final[i]);
			}else{

				

				glActiveTextureARB(GL_TEXTURE0_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);

				glActiveTextureARB(GL_TEXTURE2_ARB);
				glDisable(GL_TEXTURE_CUBE_MAP_ARB);

				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();

				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);
			}

			#ifndef SOFTWARE_TRANSFORMS
				glMatrixMode(GL_MODELVIEW);
				glPushMatrix();

				

				glTranslatef(final[i]->pos.x,final[i]->pos.y,final[i]->pos.z);
				
				
				glRotatef(final[i]->rot.z,0.0f,0.0f,1.0f);
				glRotatef(final[i]->rot.y,0.0f,1.0f,0.0f);
				glRotatef(final[i]->rot.x,1.0f,0.0f,0.0f);

				glScalef(final[i]->scale.x,final[i]->scale.y,final[i]->scale.z);

			#endif

			if(final[i]->noclip){

				float of=level->camera->clipFar;
				float on=level->camera->clipNear;

				float cr=(final[i]->pos-level->camera->pos).magnitude();
				float br=final[i]->box.magnitude();
				level->camera->clipFar=cr+br;

				level->camera->clipNear=(final[i]->pos-level->camera->pos).magnitude()-final[i]->box.magnitude();
				if(level->camera->clipNear<1){level->camera->clipNear=1;}

				glMatrixMode(GL_PROJECTION);
				glPushMatrix();
				glLoadIdentity();

				level->camera->projectionMatrix();

				glMatrixMode(GL_MODELVIEW);
				
				level->camera->clipFar=of;
				level->camera->clipNear=on;

			}

			glCallList(final[i]->glListName);

			if(final[i]->noclip){
				glMatrixMode(GL_PROJECTION);
				glPopMatrix();
				glMatrixMode(GL_MODELVIEW);
			}

			#ifndef SOFTWARE_TRANSFORMS
				glPopMatrix();
			#endif
		}

	}
	glDepthMask(1);

}

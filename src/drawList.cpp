#include "GlHeader.hpp"
										
#include "draw.hpp"
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
#include "SceneGraph.hpp"

#ifdef _WIN32
	#include <math.h>
#endif

#include "picker_matrix.h"


void buildNonstaticList(){

	Array<Object*> a;
	a.pushBack();

	for(int i=0; i<level->nonstaticObjects.size(); i++){

		if(!level->nonstaticObjects[i]->listBuilt || level->nonstaticObjects[i]->dirty){

			if(level->nonstaticObjects[i]->envelopeVerticesCount>1){

				if(level->nonstaticObjects[i]->glListNameEnvelope==NULL){
					level->nonstaticObjects[i]->glListNameEnvelope=new unsigned int[level->nonstaticObjects[i]->envelopeVerticesCount];
				}

				for(int j=0; j<level->nonstaticObjects[i]->envelopeVerticesCount; j++){
					glDeleteLists(level->nonstaticObjects[i]->glListNameEnvelope[j],1);

					level->nonstaticObjects[i]->glListNameEnvelope[j]=glGenLists(1);
					glNewList(level->nonstaticObjects[i]->glListNameEnvelope[j],GL_COMPILE);

					level->nonstaticObjects[i]->drawCurrentEnvelope=j;

					a[0]=level->nonstaticObjects[i];

					drawArray(&a,false);

					glEndList();	
				}

			}else{
				/*Object* object = level->nonstaticObjects[i];
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
				
				GLfloat lol = ver[vcount*3-1];
				
				glGenBuffers(1, &(object->vboId));
				glBindBuffer(GL_ARRAY_BUFFER, object->vboId);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vcount, &(ver[0]), GL_STATIC_DRAW);
				
				*/
				//////////////////////////
				
				glDeleteLists(level->nonstaticObjects[i]->glListName,1);

				level->nonstaticObjects[i]->glListName=glGenLists(1);
				glNewList(level->nonstaticObjects[i]->glListName,GL_COMPILE);
				a[0]=level->nonstaticObjects[i];

				drawArray(&a,false);

				glEndList();	
			}

			level->nonstaticObjects[i]->listBuilt=true;

		}

	}

}


void environmentMapSetup(Object* obj){
	
	//hack to deal w/environment maps
	//if(level->materials[level->nonstaticObjects[i]->material].shader & MATERIAL_SHADER_ENVIRONMENT){

	glMatrixMode(GL_TEXTURE);

	if(level->materials[obj->material].textureDiffuse!=-1){
		glActiveTextureARB(GL_TEXTURE2_ARB);
	}else if(obj->lightmap==-1){
		glActiveTextureARB(GL_TEXTURE0_ARB);
	}else{
		glActiveTextureARB(GL_TEXTURE2_ARB);
	}


	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
    
	//Enable auto generation of tex-coords
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	glRotatef(180,1.0f,0,0);

	level->camera->reverseTransformRot();

}

FloatVector3d rotX(FloatVector3d f,float Da){

        float dist=math.sqrt(f.y*f.y+f.z*f.z);
        float theta=math.vectorToAngle(FloatVector2d(0,-1),FloatVector2d(f.y,-f.z))+Da;
        f.z=-math.cosDegrees(theta)*dist;
        f.y=-math.sinDegrees(theta)*dist;

        return f;
}

FloatVector3d rotY(FloatVector3d f,float Da){

        float dist=math.sqrt(f.x*f.x+f.z*f.z);
        float theta=math.vectorToAngle(FloatVector2d(0,-1),FloatVector2d(-f.z,f.x))+Da;
        f.x=math.cosDegrees(theta)*dist;
        f.z=-math.sinDegrees(theta)*dist;

        return f;
}

void drawNonstaticInner(Object* object){
	
	//trippy... hides stuff randomly.  lets ya see if any object draw calls are affecting anything
	/*if(randomGen.Random()<0.1){
		return;
	}*/

	#ifndef SOFTWARE_TRANSFORMS
			
	bool disp=(conf->displacement  && 
			hardware.fragmentProgram && 
			level->materials[object->material].textureDisplacement!=-1  && 
			level->materials[object->material].textureSpecular==-1  && 
			(level->materials[object->material].shader & MATERIAL_SHADER_DISPLACEMENT) &&
			#ifdef FEATURE_DISPLACEMENT
			true
			#else
			false
			#endif
			);

		bool disp_spec=( conf->displacement  && 
			level->materials[object->material].textureSpecular!=-1  && 
			level->materials[object->material].textureDisplacement!=-1 && 
			hardware.fragmentProgram   && 
			(level->materials[object->material].shader & MATERIAL_SHADER_DISPLACEMENT) &&
			#ifdef FEATURE_DISPLACEMENT
			true
			#else
			false
			#endif
			);

		bool spec=(level->materials[object->material].textureSpecular!=-1  && 
			hardware.fragmentProgram && 
			level->materials[object->material].textureDisplacement==-1 &&
			(level->materials[object->material].shader & MATERIAL_SHADER_SPECULAR) &&
			#ifdef FEATURE_SPECULAR
			true
			#else
			false
			#endif
			);

	bool mix=( (level->materials[object->material].shader & MATERIAL_SHADER_MIXER)  && 
		level->materials[object->material].textureDiffuse!=-1 && 
		level->materials[object->material].textureEnvironmentFront!=-1 && 
		level->materials[object->material].textureEnvironmentBack!=-1  && 
		hardware.fragmentProgram &&
			#ifdef FEATURE_MULTITEXTURE
			true
			#else
			false
			#endif
			);

		if(!disp && !disp_spec && !spec){
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			glTranslatef(object->pos.x,object->pos.y,object->pos.z);
			
			if(object->rotMode=="local"){
				FloatVector3d yaxis(0,1,0);
				FloatVector3d zaxis(0,0,1);

				zaxis=rotX(zaxis,object->rot.x);
				yaxis=rotX(yaxis,object->rot.x);

				zaxis=rotY(zaxis,object->rot.y);

				glRotatef(object->rot.z,zaxis.x,zaxis.y,zaxis.z);
				glRotatef(object->rot.y,yaxis.x,yaxis.y,yaxis.z);
				glRotatef(object->rot.x,1.0f,0.0f,0.0f);


			}else{
				glRotatef(object->rot.z,0.0f,0.0f,1.0f);
				glRotatef(object->rot.y,0.0f,1.0f,0.0f);
				glRotatef(object->rot.x,1.0f,0.0f,0.0f);
			}

			glScalef(object->scale.x,object->scale.y,object->scale.z);
		}else{
			if(disp){
				shader.set(displaceFragmentShader);
				shader.set(displaceVertexShader);

			}else if(disp_spec){
				if(level->materials[object->material].textureBump!=-1){
					shader.set(displaceSpecularBumpFragmentShader);
					shader.set(displaceSpecularBumpVertexShader);
				}else{
					shader.set(displaceSpecularFragmentShader);
					shader.set(displaceSpecularVertexShader);

					glEnable(GL_ALPHA_TEST);
					glAlphaFunc(GL_GREATER,0.1);

				}
			}else if(spec){

				shader.set(specularFragmentShader);
				shader.set(specularVertexShader);

			}else if(mix){
				if(level->materials[object->material].textureEnvironmentTop!=-1 && level->materials[object->material].textureEnvironmentBottom!=-1){
					shader.set(mixer_3_v);
					shader.set(mixer_3_f);
				}else if(level->materials[object->material].textureEnvironmentLeft!=-1 && level->materials[object->material].textureEnvironmentRight!=-1){
					shader.set(mixer_2_v);
					shader.set(mixer_2_f);
				}else{
					shader.set(mixer_1_v);
					shader.set(mixer_1_f);
				}
			}


			double mat[16];
			glGetDoublev(GL_MODELVIEW_MATRIX,mat);

			Matrix4d4d m;
			m.loadIdentity();
			m.translate(object->pos.x,object->pos.y,object->pos.z);			
			m.rotateZ(object->rot.z);
			m.rotateY(object->rot.y);
			m.rotateX(object->rot.x);
			m.scale(object->scale.x,object->scale.y,object->scale.z);

			shader.localParameterVertex(0,m.column(0));
			shader.localParameterVertex(1,m.column(1));
			shader.localParameterVertex(2,m.column(2));
			shader.localParameterVertex(3,m.column(3));

			Matrix4d4d mm;
			mm.loadIdentity();
			mm.rotateZ(object->rot.z);
			mm.rotateY(object->rot.y);
			mm.rotateX(object->rot.x);

			shader.localParameterVertex(4,mm.column(0));
			shader.localParameterVertex(5,mm.column(1));
			shader.localParameterVertex(6,mm.column(2));

		}

	#endif

	//hack to deal w/environment maps
	if(level->materials[object->material].shader & MATERIAL_SHADER_ENVIRONMENT &&
#ifdef FEATURE_CUBIC_ENVIRONMENT_MAPS
	   true
#else
	   false 
#endif
	   ){

		environmentMapSetup(object);
	}

	if(object->noclip){

		float of=level->camera->clipFar;
		float on=level->camera->clipNear;

		float cr=(object->pos-level->camera->pos).magnitude();
		float br=object->box.magnitude();
		level->camera->clipFar=cr+br*2;

		level->camera->clipNear=(object->pos-level->camera->pos).magnitude()-object->box.magnitude();
		if(level->camera->clipNear<1){level->camera->clipNear=1;}

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		level->camera->projectionMatrix();

		glMatrixMode(GL_MODELVIEW);
		
		level->camera->clipFar=of;
		level->camera->clipNear=on;

	}

	if(object->visible){
		if(object->envelopeVerticesCount>1){
			glCallList(object->glListNameEnvelope[object->drawCurrentEnvelope]);
		}else{
			glCallList(object->glListName);
			
			/*
			Array<Object*> a;
		    a.pushBack(object);
			drawArray(&a,false);
			*/
			
			//drawArrayInner(object,false);
			
			/*
			glBindBuffer(GL_ARRAY_BUFFER, object->vboId);         // for vertex coordinates
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, 0, 0);
			glDrawArrays(GL_TRIANGLES, 0, object->getVertexCount());
			//glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, 0);
			glDisableClientState(GL_VERTEX_ARRAY);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			 */
		}
	}

	if(object->noclip){
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}


	#ifndef SOFTWARE_TRANSFORMS
		if(!disp && !disp_spec && !spec){
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}else{
			if(disp){
				

			}else if(disp_spec){
				if(level->materials[object->material].textureBump!=-1){
					
				}else{
					glDisable(GL_ALPHA_TEST);

				}
			}
		}
	#endif
}

void drawNonstaticCallback(ObjectAddress a){

	Object* obj=a.object;

	bool doDraw=true;

	if(a.type==INSTANCE){
		if(a.instance->object!=NULL){
			if(a.instance->object->type==OBJECT){
				obj=a.instance->object->object;
				a.instance->pre();
			}
		}else{
			doDraw=false;
		}
	}

	if(doDraw){
		drawNonstaticInner(obj);
	}

	if(a.type==INSTANCE){
		if(a.instance->object!=NULL){
			if(a.instance->object->type==OBJECT){
				a.instance->post();
			}
		}
	}
}

/* drawNonstaticList()
 * This draws the nonstatic objects in the scene by simply iterating over them
 * and calling drawNonstaticInner on each
 */
void drawNonstaticList(){

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

	glMatrixMode(GL_MODELVIEW);

	if(conf->sceneGraph){
		level->sceneGraph.clearDrawList();

		for(int i=0; i<level->nonstaticObjects.size(); i++){
			level->nonstaticObjects[i]->sceneGraphNode->draw=true;
		}

		for(int i=0; i<level->instances.size(); i++){
			level->instances[i]->sceneGraphNode->draw=true;
		}

		level->sceneGraph.draw(drawNonstaticCallback,false);
	}else{
		for(int i=0; i<level->nonstaticObjects.size(); i++){
			drawNonstaticInner(level->nonstaticObjects[i]);
		}

	}
}



void rebuildLists(){
	listBuild=false;
}

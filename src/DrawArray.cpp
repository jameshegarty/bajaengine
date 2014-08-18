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

#include "SceneGraph.hpp"
#include "lightmap.h"

#ifdef _WIN32
#include <math.h>
#endif

#include "picker_matrix.h"

void drawArrayQuick(Array<Object*>* objects){
	
	if(objects==NULL){
		console().write("drawarray NULL call");
		return;
	}
	
	unsigned int vcount;
	
	for(int i=0; i<objects->size(); i++){
		
		if((*objects)[i]==NULL){
			continue;
			
		}
		
		if((*objects)[i]->flags & OBJECT_WATER){
			continue;
		}
		
		
		if((*objects)[i]->type!=0  ){
			
			continue;
		}
		
		if(!(*objects)[i]->visible ){
			
			continue;
		}
		
		if((*objects)[i]->nofog && level->fog.on){
			glDisable(GL_FOG);
		}
		
		if((*objects)[i]->noclip){
			
			float of=level->camera->clipFar;
			
			level->camera->clipFar=((*objects)[i]->pos-level->camera->pos).magnitude()+(*objects)[i]->box.magnitude();
			
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			
			level->camera->projectionMatrix();
			
			glMatrixMode(GL_MODELVIEW);
			
			level->camera->clipFar=of;
			
		}
		
		vcount=(*objects)[i]->getVertexCount();
		
		for(unsigned int v=0; v<vcount; v++){
			
			if(conf->wireframe && (*objects)[i]->getVertexRaw(v).end==START_STRIP){
				
				glBegin(GL_LINE_STRIP);
				
			}else if(conf->wireframe && (*objects)[i]->getVertexRaw(v).end==START_TRI){
				
				glBegin(GL_LINES);
				
			}else if((*objects)[i]->getVertexRaw(v).end==START_TRI){
				
				glBegin(GL_TRIANGLES);
				
			}else if((*objects)[i]->getVertexRaw(v).end==START_STRIP){
				
				glBegin(GL_TRIANGLE_STRIP);
			}
			
			
			
			glVertex3f((*objects)[i]->getVertexRaw(v).pos.x,
					   (*objects)[i]->getVertexRaw(v).pos.y,
					   (*objects)[i]->getVertexRaw(v).pos.z);
			
			
			if((*objects)[i]->getVertexRaw(v).end==END_TRI){
				glEnd();
			}else if((*objects)[i]->getVertexRaw(v).end==END_STRIP){
				glEnd();
			}
			
		}
		
		if((*objects)[i]->nofog && level->fog.on){
			glEnable(GL_FOG);
		}
		
		if((*objects)[i]->noclip){
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
		}
		
	}
}


void drawArrayInner(Object* object,bool transform){
	
	assert(!transform);
	
	//workaround, due to having to adjust env map objects in nonstaticdraw
#ifdef FEATURE_CUBIC_ENVIRONMENT_MAPS
	if(!(level->materials[object->material].shader & MATERIAL_SHADER_ENVIRONMENT)){
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
	}
#endif
	
	//trippy... hides stuff randomly.  lets ya see if any object draw calls are affecting anything
	//if(randomGen.Random()<0.9){
	//continue;
	//}
	
	if(object==NULL){
		return;
		
	}
	
	if(object->flags & OBJECT_WATER){
		return;
	}
	
	
	if(object->type!=0){
		return;
	}
	
	if(!object->visible && transform){
		return;
	}
	
	
	
	
	if(object->nofog && level->fog.on){
		glDisable(GL_FOG);
	}
	
	if(object->noclip  && transform){
		
		float of=level->camera->clipFar;
		float on=level->camera->clipNear;
		
		float cr=(object->pos-level->camera->pos).magnitude();
		float br=object->box.magnitude();
		level->camera->clipFar=cr+br;
		
		level->camera->clipNear=(object->pos-level->camera->pos).magnitude()-object->box.magnitude();
		if(level->camera->clipNear<1){level->camera->clipNear=1;}
		level->camera->clipNear=1;
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		
		level->camera->projectionMatrix();
		
		glMatrixMode(GL_MODELVIEW);
		
		level->camera->clipFar=of;
		level->camera->clipNear=on;
	}
	
	
	
	updateLights(object);
	updateMaterial(object,transform);
	
	
	bool disp=(conf->displacement  && 
			   hardware.fragmentProgram && 
			   level->materials[object->material].textureDisplacement!=-1  && 
			   level->materials[object->material].textureSpecular==-1  && 
			   (level->materials[object->material].shader & MATERIAL_SHADER_DISPLACEMENT));
	
	bool disp_spec=( conf->displacement  && 
					level->materials[object->material].textureSpecular!=-1  && 
					level->materials[object->material].textureDisplacement!=-1 && 
					hardware.fragmentProgram   && 
					(level->materials[object->material].shader & MATERIAL_SHADER_DISPLACEMENT));
	
	bool spec=(level->materials[object->material].textureSpecular!=-1  && 
			   hardware.fragmentProgram && 
			   level->materials[object->material].textureDisplacement==-1 &&
			   (level->materials[object->material].shader & MATERIAL_SHADER_SPECULAR));
	
	bool mix=( (level->materials[object->material].shader & MATERIAL_SHADER_MIXER)  && 
			  level->materials[object->material].textureDiffuse!=-1 && 
			  level->materials[object->material].textureEnvironmentFront!=-1 && 
			  level->materials[object->material].textureEnvironmentBack!=-1  && 
			  hardware.fragmentProgram);
	
	
#ifndef SOFTWARE_TRANSFORMS
	
	if(transform){
		
		if(!disp && !disp_spec && !spec){
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			glTranslatef(object->pos.x,object->pos.y,object->pos.z);
			glRotatef(object->rot.z,0.0f,0.0f,1.0f);
			glRotatef(object->rot.y,0.0f,1.0f,0.0f);
			glRotatef(object->rot.x,1.0f,0.0f,0.0f);
			glScalef(object->scale.x,object->scale.y,object->scale.z);
			
		}
		
		if(mix){
			Matrix4d4d mlol;
			glGetFloatv(GL_MODELVIEW_MATRIX,mlol);
		}
		
		
		//texcoords
#ifndef SOFTWARE_TRANSFORM_TEXCOORDS
		glMatrixMode(GL_TEXTURE);
		
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex0pos.x,level->materials[object->material].tex0pos.y,0);
		glRotatef(level->materials[object->material].tex0rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex0rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex0scale.x,level->materials[object->material].tex0scale.y,0);
		
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex1pos.x,level->materials[object->material].tex1pos.y,0);
		glRotatef(level->materials[object->material].tex1rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex1rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex1scale.x,level->materials[object->material].tex1scale.y,0);
		
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex2pos.x,level->materials[object->material].tex2pos.y,0);
		glRotatef(level->materials[object->material].tex2rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex2rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex2scale.x,level->materials[object->material].tex2scale.y,0);
		
		glActiveTextureARB(GL_TEXTURE3_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex3pos.x,level->materials[object->material].tex3pos.y,0);
		glRotatef(level->materials[object->material].tex3rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex3rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex3scale.x,level->materials[object->material].tex3scale.y,0);
		
		glActiveTextureARB(GL_TEXTURE4_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex4pos.x,level->materials[object->material].tex4pos.y,0);
		glRotatef(level->materials[object->material].tex4rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex4rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex4scale.x,level->materials[object->material].tex4scale.y,0);
		
		glActiveTextureARB(GL_TEXTURE5_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex5pos.x,level->materials[object->material].tex5pos.y,0);
		glRotatef(level->materials[object->material].tex5rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex5rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex5scale.x,level->materials[object->material].tex5scale.y,0);
		
		
		glActiveTextureARB(GL_TEXTURE6_ARB);
		glPushMatrix();
		glTranslatef(level->materials[object->material].tex6pos.x,level->materials[object->material].tex6pos.y,0);
		glRotatef(level->materials[object->material].tex6rot.y,0.0f,1.0f,0.0f);
		glRotatef(level->materials[object->material].tex6rot.x,1.0f,0.0f,0.0f);
		glScalef(level->materials[object->material].tex6scale.x,level->materials[object->material].tex6scale.y,0);
#endif
		
		glMatrixMode(GL_MODELVIEW);
	}
	
	
#else
	
#endif
	
	
	int vcount=object->getVertexCount();
	
	if(object->envelopeVerticesCount>1){
		vcount=object->envelopeVertices[object->drawCurrentEnvelope].size();
	}
	
	// trippy... uncomment this to hide half the polygons
	//vcount = vcount / 2 - ( (vcount/2) % 3);
	//assert(vcount % 3 == 0);

	/*std::vector<GLfloat> ver(0);
	//ver.reserve(vcount*3);
	for(unsigned int v=0; v<vcount; v++){			
		Vertex* vertex=&(object->getVertexRaw(v));

		ver.push_back(vertex->pos.x);
		ver.push_back(vertex->pos.y);

		ver.push_back(vertex->pos.z);

	}
	
	assert(ver.size() == vcount*3);
	
	GLfloat lol = ver[vcount*3-1];
	printf("%f\n",lol);
	
	GLuint vboId;
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*3*vcount, &(ver[0]), GL_STATIC_DRAW);
	*/
	
	//////////
	/*glBindBuffer(GL_ARRAY_BUFFER, object->vboId);         // for vertex coordinates
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, vcount);
	//glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
	//int vcount=object->getVertexCount();

	bool ended = false;
	
	for(unsigned int v=0; v<vcount; v++){
		
		Vertex* vertex;
		
		if(object->envelopeVerticesCount>1){
			vertex=object->envelopeVertices[object->drawCurrentEnvelope][v];
		}else{
			vertex=&object->getVertexRaw(v);
		}
		
		if(conf->wireframe && vertex->end==START_STRIP){
			glBegin(GL_LINE_STRIP);
		}else if(conf->wireframe && vertex->end==START_TRI){
			glBegin(GL_LINES);
		}else if(vertex->end==START_TRI){
			glBegin(GL_TRIANGLES);
		}else if(vertex->end==START_STRIP){
			glBegin(GL_TRIANGLE_STRIP);
		}
		
		
		glNormal3f(vertex->normal.x, 
				   vertex->normal.y, 
				   vertex->normal.z);
		
		
		glMultiTexCoord2fARB(GL_TEXTURE0_ARB, vertex->uv0.x, vertex->uv0.y);
		glMultiTexCoord2fARB(GL_TEXTURE1_ARB, vertex->uvLightmap.x,vertex->uvLightmap.y);
		
    if(level->materials[object->material].shader & MATERIAL_SHADER_MIXER){
			glMultiTexCoord2fARB(GL_TEXTURE2_ARB, vertex->uv1.x, vertex->uv1.y);
			glMultiTexCoord2fARB(GL_TEXTURE3_ARB, vertex->uv2.x, vertex->uv2.y);
			glMultiTexCoord2fARB(GL_TEXTURE4_ARB, vertex->uv3.x, vertex->uv3.y);
			glMultiTexCoord2fARB(GL_TEXTURE5_ARB, vertex->uv4.x, vertex->uv4.y);
			glMultiTexCoord2fARB(GL_TEXTURE6_ARB, vertex->uv5.x, vertex->uv5.y);
			glMultiTexCoord2fARB(GL_TEXTURE7_ARB, vertex->uv6.x, vertex->uv6.y);
		}else{
			glMultiTexCoord3fARB(GL_TEXTURE2_ARB,vertex->tangent.x , vertex->tangent.y,vertex->tangent.z);
			glMultiTexCoord3fARB(GL_TEXTURE3_ARB,vertex->binormal.x,vertex->binormal.y,vertex->binormal.z);
			
			if(level->materials[object->material].textureSpecular!=-1){
				glMultiTexCoord2fARB(GL_TEXTURE4_ARB,vertex->uv1.x,vertex->uv1.y);
			}
		}
		
		
		FloatVector3d p=vertex->pos;
		
		glVertex3f(vertex->pos.x,
				   vertex->pos.y,
				   vertex->pos.z);
		
		
		if(vertex->end==END_TRI){
			glEnd();
			ended = true;
		}else if(vertex->end==END_STRIP){
			glEnd();
			ended = true;
		}
		
	}
	
	if(!ended){glEnd();}
	
#ifndef SOFTWARE_TRANSFORMS
	
	if(transform){
		if(!disp && !disp_spec && !spec){
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
		}
		
#ifndef SOFTWARE_TRANSFORM_TEXCOORDS
		glMatrixMode(GL_TEXTURE);
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glPopMatrix();
		glActiveTextureARB(GL_TEXTURE1_ARB);
		glPopMatrix();
		glActiveTextureARB(GL_TEXTURE2_ARB);
		glPopMatrix();
		glActiveTextureARB(GL_TEXTURE3_ARB);
		glPopMatrix();
		glActiveTextureARB(GL_TEXTURE4_ARB);
		glPopMatrix();
		glActiveTextureARB(GL_TEXTURE5_ARB);
		glPopMatrix();
		glActiveTextureARB(GL_TEXTURE6_ARB);
		glPopMatrix();
		
		glMatrixMode(GL_MODELVIEW);
#endif
		
		
	}
	
#else
	
#endif
	
	
	if(object->nofog && level->fog.on){
		glEnable(GL_FOG);
	}
	
	if(object->noclip && transform){
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
	}
	 
}

void drawArrayPlain(Array<Object*>* objects,bool transform){
	
	
	
	
	for(int i=0; i<objects->size(); i++){
		drawArrayInner((*objects)[i],transform);
	}
	
	
}


bool dTransform;



void drawArrayCallback(ObjectAddress a){
	drawArrayInner(a.object,dTransform);
}



void drawArraySceneGraph(Array<Object*>* objects,bool transform){
	level->sceneGraph.clearDrawList();
	dTransform=transform;
	
	for(int i=0; i<objects->size(); i++){
		(*objects)[i]->sceneGraphNode->draw=true;
	}
	
	level->sceneGraph.draw(drawArrayCallback,false);
}

void drawArray(Array<Object*>* objects,bool transform){
	
	if(objects==NULL){
		console().write("drawarray NULL call");
		return;
	}
	
	//get rid of any lingering state
	
#ifdef LOOP_DEBUG
	logs().main.write("drawArray texture DISABLE");
#endif
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE5_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE7_ARB);
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE8_ARB);
	glDisable(GL_TEXTURE_2D);
	
#ifdef LOOP_DEBUG
	logs().main.write("drawArray texture DISABLE END");
#endif
	
	if(conf->sceneGraph && transform){
		drawArraySceneGraph(objects,transform);
	}else{
		drawArrayPlain(objects,transform);
	}
}

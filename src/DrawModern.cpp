#include "level.hpp"
#include "draw.hpp"
#include "sort.hpp"
#include "HelperLibMath.hpp"
#include "timeing.hpp"

bool modernStaticBuilt=false;

void buildModernStatic(){
	Array<Object*> a;
	a.pushBack();


	for(int i=0; i<level->staticObjects.size(); i++){
		/*Object* object = level->staticObjects[i];
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
		///////////////////////
		
		level->staticObjects[i]->glListName=glGenLists(1);
		glNewList(level->staticObjects[i]->glListName,GL_COMPILE);
		a[0]=level->staticObjects[i];

		drawArray(&a,false);

		glEndList();	
	
		level->staticObjects[i]->listBuilt=true;

	}
}

extern Level* lastLevel;

#define RIGHT 0
#define LEFT 1
#define BOTTOM 2
#define TOP 3
#define PFAR 4
#define PNEAR 5

bool frozenFrustumStored=false;
Matrix4d4d matFrustumStored;

ObjectAddress pl[6];

void drawModern(){

	if(!modernStaticBuilt || level!=lastLevel){
		buildModernStatic();
		modernStaticBuilt=true;
		lastLevel=level;
	}

	buildNonstaticList();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	engineTime.startTimer("sort");

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

	//frustrum culling
	Matrix4d4d matModelView;
	Matrix4d4d matProjection;
	Matrix4d4d matFrustrum;
	Plane3d* p;
	Plane3d frustumPlane[6];

	GLdouble projMatrix[16];

	{
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		level->camera->projectionMatrix();
		glGetFloatv(GL_PROJECTION_MATRIX,matFrustrum.entries);
		glPopMatrix();
		
		glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		level->camera->transform();
		glGetFloatv(GL_MODELVIEW_MATRIX,matModelView.entries);
		glPopMatrix();

		matFrustrum=matFrustrum*matModelView;
	}

	if(level->camera->freezeFrustum){
		if(!frozenFrustumStored){
			matFrustumStored=matFrustrum;
			frozenFrustumStored=true;
		}

		matFrustrum=matFrustumStored;
	}else{
		frozenFrustumStored=false;
	}

	p=&frustumPlane[RIGHT];
	p->normal.x=matFrustrum.entries[3]-matFrustrum.entries[0];
	p->normal.y=matFrustrum.entries[7]-matFrustrum.entries[4];
	p->normal.z=matFrustrum.entries[11]-matFrustrum.entries[8];
	p->x=matFrustrum.entries[15]-matFrustrum.entries[12];

	p=&frustumPlane[LEFT];
	p->normal.x=matFrustrum.entries[3]+matFrustrum.entries[0];
	p->normal.y=matFrustrum.entries[7]+matFrustrum.entries[4];
	p->normal.z=matFrustrum.entries[11]+matFrustrum.entries[8];
	p->x=matFrustrum.entries[15]+matFrustrum.entries[12];

	p=&frustumPlane[BOTTOM];
	p->normal.x=matFrustrum.entries[3]+matFrustrum.entries[1];
	p->normal.y=matFrustrum.entries[7]+matFrustrum.entries[5];
	p->normal.z=matFrustrum.entries[11]+matFrustrum.entries[9];
	p->x=matFrustrum.entries[15]+matFrustrum.entries[13];

	p=&frustumPlane[TOP];
	p->normal.x=matFrustrum.entries[3]-matFrustrum.entries[1];
	p->normal.y=matFrustrum.entries[7]-matFrustrum.entries[5];
	p->normal.z=matFrustrum.entries[11]-matFrustrum.entries[9];
	p->x=matFrustrum.entries[15]-matFrustrum.entries[13];

	p=&frustumPlane[PFAR];
	p->normal.x=matFrustrum.entries[3]-matFrustrum.entries[2];
	p->normal.y=matFrustrum.entries[7]-matFrustrum.entries[6];
	p->normal.z=matFrustrum.entries[11]-matFrustrum.entries[10];
	p->x=matFrustrum.entries[15]-matFrustrum.entries[14];

	p=&frustumPlane[PNEAR];
	p->normal.x=matFrustrum.entries[3]+matFrustrum.entries[2];
	p->normal.y=matFrustrum.entries[7]+matFrustrum.entries[6];
	p->normal.z=matFrustrum.entries[11]+matFrustrum.entries[10];
	p->x=matFrustrum.entries[15]+matFrustrum.entries[14];

	for(int i=0;i<6;i++){
		p=&frustumPlane[i];
		float t=sqrt(p->normal.x*p->normal.x+p->normal.y*p->normal.y+p->normal.z*p->normal.z);
		p->normal.x/=t;
		p->normal.y/=t;
		p->normal.z/=t;
		p->x/=t;
	}

	if(conf->frustumDebug){
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glPointSize(10);

		glColor4f(0,1,0,1);

		{	
			glActiveTextureARB(GL_TEXTURE1_ARB);
			glDisable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE2_ARB);
			glDisable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE3_ARB);
			glDisable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE4_ARB);
			glDisable(GL_TEXTURE_2D);

			glActiveTextureARB(GL_TEXTURE0_ARB);

			glDisable(GL_BLEND);


			shader.reset();
			
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_LIGHTING);

			float amb[4];
			amb[0]=1;
			amb[1]=1;
			amb[2]=1;
			amb[3]=1;

			glLightModelfv(GL_LIGHT_MODEL_AMBIENT,amb);
			glMaterialfv(GL_FRONT,GL_DIFFUSE,amb);
			glMaterialfv(GL_FRONT,GL_SPECULAR,amb);
			glMaterialfv(GL_FRONT,GL_AMBIENT,amb);
		}

		for(int p=0; p<6; p++){

			glColor4f((p%7)/float(9),(p+1%7)/float(9),(p+2%7)/float(9),1);

			FloatVector3d cnt(float(1)/frustumPlane[p].normal.x,-float(1)/frustumPlane[p].normal.y,-frustumPlane[p].x/frustumPlane[p].normal.z);

			cnt=-frustumPlane[p].x*frustumPlane[p].normal;

			glBegin(GL_POINTS);
				glVertex3f(cnt.x,cnt.y,cnt.z);
			glEnd();

			FloatVector3d b1(float(2)/frustumPlane[p].normal.x,-float(1)/frustumPlane[p].normal.y,-float(1)/frustumPlane[p].normal.z);
			b1.normalize();

			FloatVector3d b2=math.crossProduct(b1,frustumPlane[p].normal);

			float si=10;

			glBegin(GL_TRIANGLES);
				glVertex3f(cnt.x,cnt.y,cnt.z);
				glVertex3f(cnt.x+si*b1.x,cnt.y+si*b1.y,cnt.z+si*b1.z);
				glVertex3f(cnt.x+si*b2.x,cnt.y+si*b2.y,cnt.z+si*b2.z);
			glEnd();

			glBegin(GL_TRIANGLES);
				glVertex3f(cnt.x,cnt.y,cnt.z);
				glVertex3f(cnt.x-si*b1.x,cnt.y-si*b1.y,cnt.z-si*b1.z);
				glVertex3f(cnt.x-si*b2.x,cnt.y-si*b2.y,cnt.z-si*b2.z);
			glEnd();

			glBegin(GL_TRIANGLES);
				glVertex3f(cnt.x,cnt.y,cnt.z);
				glVertex3f(cnt.x+si*b1.x,cnt.y+si*b1.y,cnt.z+si*b1.z);
				glVertex3f(cnt.x-si*b2.x,cnt.y-si*b2.y,cnt.z-si*b2.z);
			glEnd();

			glBegin(GL_TRIANGLES);
				glVertex3f(cnt.x,cnt.y,cnt.z);
				glVertex3f(cnt.x-si*b1.x,cnt.y-si*b1.y,cnt.z-si*b1.z);
				glVertex3f(cnt.x+si*b2.x,cnt.y+si*b2.y,cnt.z+si*b2.z);
			glEnd();

		}

		glPointSize(1);

		glEnable(GL_CULL_FACE);
	}
	
	//end

	if(conf->sceneGraph){
		level->sceneGraph.clearDrawList();

		for(int i=0; i<level->nonstaticObjects.size(); i++){
			level->nonstaticObjects[i]->sceneGraphNode->draw=true;
		}

		for(int i=0; i<level->staticObjects.size(); i++){
			level->staticObjects[i]->sceneGraphNode->draw=true;
		}

		for(int i=0; i<level->instances.size(); i++){
			level->instances[i]->sceneGraphNode->draw=true;
		}

		bool frustrum=false;
		FloatVector3d center;
		float radius;

		for(int i=0; i<level->objects.size(); i++){

			if(level->objects[i]->sceneGraphNode->draw){
				frustrum=true;

				center=level->objects[i]->box.center();
				radius=level->objects[i]->box.magnitude()/float(2);

				for(int j=0;j<6;j++)
				{
					p=&frustumPlane[j];
					if(p->normal.x*center.x+p->normal.y*center.y+p->normal.z*center.z+p->x <= -radius){
						frustrum=false;
						level->objects[i]->sceneGraphNode->draw=false;
					}
				}
				
				
			}
		}

		engineTime.endTimer("sort");

		level->sceneGraph.draw(drawNonstaticCallback,false);
	}else{
		Array<SortItem> items;
		FloatVector3d dist;
		FloatVector3d center;
		float radius;

		bool frustrum=false;

		for(int i=0; i<level->objects.size(); i++){

			if( !(level->objects[i]->flags & OBJECT_TRANSPARENT)  && level->objects[i]->visible){
				frustrum=true;

				center=level->objects[i]->box.center();
				radius=level->objects[i]->box.magnitude()/float(2);

				for(int j=0;j<6;j++)
				{
					p=&frustumPlane[j];
					if(p->normal.x*center.x+p->normal.y*center.y+p->normal.z*center.z+p->x <= -radius){
						frustrum=false;
					}
				}
				
				if(level->objects[i]->listBuilt && frustrum){
					dist=center-level->camera->pos;
					items.pushBack(SortItem(i,math.dotProduct(dist,dist)));
				}
			}
		}

		Array<int> sr=sort.sort(items,true);

		engineTime.endTimer("sort");

		//use this to debug the sort order
		//it SHOULD draw fucked up - the farthest objects should be on top
		//glDisable(GL_DEPTH_TEST);

		for(int i=0; i<sr.size(); i++){
			drawNonstaticInner(level->objects[sr[i]]);
		}
	}
}

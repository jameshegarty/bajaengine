#include "Polygon2d.hpp"
#include "GlHeader.hpp"
#include "shader.hpp"
#include "conf.h"

const GLubyte smask[128]={0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55,
0xAA,0xAA,0xAA,0xAA,0x55,0x55,0x55,0x55};

void Polygon2d::draw(){
	if(!visible){return;}

	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glColor4f(color.x,color.y,color.z,color.w);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE3_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE4_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE5_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE6_ARB);
	glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);

	shader.reset();
	
	if(mode==P2D_LINE){
		/*if(!vboset){
			glGenBuffersARB(1, (GLuint*)&buffer);
			vboset=true;
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer);
		//glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(data), data, GL_STATIC_DRAW_ARB);
		int i=sizeof(data);

		glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*numberR*6, data, GL_DYNAMIC_DRAW_ARB);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		//glVertexPointer(3, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);

		glDrawArrays(GL_LINES, 0, numberR);

		glDisableClientState(GL_VERTEX_ARRAY);*/

	}else if(mode==P2D_POINT){

		glPointSize(pointSize);

		if(!vboset){
			glGenBuffersARB(1, (GLuint*)&buffer);
			vboset=true;
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer);

		float data[6]={10,-10,99,100,-100,99};

		glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*3*2, data, GL_DYNAMIC_DRAW_ARB);

		glVertexPointer(3, GL_FLOAT, 0, 0);
		
		glEnableClientState(GL_VERTEX_ARRAY);

		glDrawArrays(GL_LINE, 0, 2);


		glDisableClientState(GL_VERTEX_ARRAY);

		glVertex3f(10,-10,99);
		glVertex3f(100,-100,99);


		glPointSize(0);

	}else{
		glEnable(GL_POLYGON_STIPPLE);
		glPolygonStipple(smask);

		glBegin(GL_TRIANGLES);

		for(int i=0; i<vertices.size(); i++){
			glVertex3f(vertices[i].x,vertices[i].y,vertices[i].z);
		}

		glEnd();

		glDisable(GL_POLYGON_STIPPLE);
	}
}

void Polygon2d::clearVertices(){
	vertices.clear();
}

void Polygon2d::addVertex(FloatVector3d v){
	vertices.pushBack(v);
}

Polygon2d::Polygon2d(){
	vertexList=NULL;
	vboset=false;
}


void Polygon2d::addLineVertexList(Array<Vertex>* v){
	mode=P2D_LINE;
	vertexList=v;
}

void Polygon2d::addPointVertexList(Array<Vertex>* v){
	mode=P2D_POINT;
	vertexList=v;
}
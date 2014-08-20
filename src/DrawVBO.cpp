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


bool initialVBOBuild=false;
bool initialSort=false;

class CVert									// Vertex Class
{
public:
	float x;								// X Component
	float y;								// Y Component
	float z;								// Z Component
};
typedef CVert CVec;								// The Definitions Are Synonymous

class CTexCoord									// Texture Coordinate Class
{
public:
	float u;								// U Component
	float v;								// V Component
};

CVert*		m_pVertices;						// Vertex Data
CTexCoord*	m_pTexCoords;						// Texture Coordinates

// Vertex Buffer Object Names
GLuint	m_nVBOVertices;						// Vertex VBO Name
GLuint	m_nVBOTexCoords;					// Texture Coordinate VBO Name
int m_nVertexCount=0;


void buildVBOs(){

	if(!initialSort){

		for(int i=0; i<level->objects.size(); i++){
			level->textureSorter[level->objects[i]->material].push_back(level->objects[i]);
		}

	}

	Array<int> indices=level->textureSorter.indices();

	for(int i=0; i<indices.size(); i++){

		// Generate And Bind The Vertex Buffer
		glGenBuffersARB( 1, &m_nVBOVertices );					// Get A Valid Name
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOVertices );			// Bind The Buffer
		// Load The Data
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, m_nVertexCount*3*sizeof(float), m_pVertices, GL_STATIC_DRAW_ARB );

		// Generate And Bind The Texture Coordinate Buffer
		glGenBuffersARB( 1, &m_nVBOTexCoords );					// Get A Valid Name
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOTexCoords );		// Bind The Buffer
		// Load The Data
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, m_nVertexCount*2*sizeof(float), m_pTexCoords, GL_STATIC_DRAW_ARB );

		// Our Copy Of The Data Is No Longer Necessary, It Is Safe In The Graphics Card
		delete [] m_pVertices; m_pVertices = NULL;
		delete [] m_pTexCoords; m_pTexCoords = NULL;
	}
}

void drawVBOs(){

	//glClientActiveTextureARB

	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOVertices );
	glVertexPointer( 3, GL_FLOAT, 0, (char *) NULL );		// Set The Vertex Pointer To The Vertex Buffer
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_nVBOTexCoords );
	glTexCoordPointer( 2, GL_FLOAT, 0, (char *) NULL );		// Set The TexCoord Pointer To The TexCoord Buffer

	// Render
	glDrawArrays( GL_TRIANGLES, 0, m_nVertexCount );		// Draw All Of The Triangles At Once

	// Disable Pointers
	glDisableClientState( GL_VERTEX_ARRAY );				// Disable Vertex Arrays
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );				// Disable Texture Coord Arrays
}

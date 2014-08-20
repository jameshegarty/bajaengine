/* FakeGl.c
 * If the engine is compiled headless (ie. console only) this file 
 * provides empty functions for all the OpenGL calls in the rest of the program
 * So that those sections of the program don't need to be rewritten.
 */

#ifdef HEADLESS

int gluUnProject(
  double winx,
  double winy,
  double winz,
  const double modelMatrix[16],
  const double projMatrix[16],
  const int viewport[4],
  double *objx,
  double *objy,
  double *objz
){
	return 1;
}

void glBegin(){

}

void glEnd(){

}

void glActiveTextureARB(){

}

void glBindTexture(){

}

void glClear(){

}

void glColor4f(){

}

void glEnable(){

}

void glDisable(){

}

void glGenLists(){

}

void glLoadIdentity(){

}

void glMatrixMode(){

}

void glBlendFunc(){

}

void glCallList(){

}

void glCullFace(){

}

void glEndList(){

}

void glLineWidth(){

}

void glNewList(){

}

void glScalef(){

}

void glRotatef(){

}

void glLightfv(){

} 

void glViewport(){

}

void glVertex3f(){

}

void glGenTextures(){

}

void glTranslatef(){

}

void glPushMatrix(){

}

void glPopMatrix(){

}

void glMultiTexCoord2fARB(){

}

void glMultiTexCoord3fARB(){

}

void glNormal3f(){

}

void glTexCoord2f(){

}

void glTexImage2D(){

}

void glTexParameterf(){

}

void glTexParameteri(){

}

void gluBuild2DMipmaps(){

}

void glDeleteLists(){

}

void glLightModelfv(){

}

void glOrtho(){

}

void glClearColor(){

}

void glTexCoord2d(){

}


void gluNewQuadric(){

}

void gluPerspective(){

}

void glGetString(){

}
void glMaterialfv(){

}

void glMateriali(){

}

void glDeleteTextures(){

}

void gluErrorString(){

}

void glAlphaFunc(){

}

void glFogf(){

}

void glFogfv(){

}
void glFogi(){

}

void glHint(){

}

void glGetIntegerv(){

}

void glInitNames(){

}

void glPopName(){

}
void glPushName(){

}

void glRenderMode(){

}

void glSelectBuffer(){

}

void gluPickMatrix(){

}

void glAttachObjectARB(){

}

void glBindProgramARB(){

}

void glCompileShaderARB(){

}

void glCreatProgramObjectARB(){

}

void glCreateShaderObjectARB(){

}

void glGenProgramsARB(){

}

void glGetInfoLogARB(){

}

void glGetObjectParameterivARB(){


}



void glProgramLocalParameter4fARB(){

}

void glLinkProgramARB(){

}

void glShaderSourceARB(){

}

void glCreateProgramObjectARB(){

}

void glProgramStringARB(){

}

void glScissor(){

}

void glCallLists(){

}

void glListBase(){


}

void glVertex2f(){

}

void glDepthFunc(){

}


void glShadeModel(){

}

void gluQuadricNormals(){

}

void gluQuadricTexture(){

}

void glReadPixels(){

}

void glUseProgramObjectARB(){

}

void glClearDepth(){

}

void glTexSubImage2D(){

}

void glGetFloatv(){

}

void glGetDoublev(){
	
}
void glBufferDataARB(){
	
}
void glStencilFunc(){
	
}
void glCopyTexImage2D(){
	
}

void glProgramLocalParameter4dvARB(){
	
}

void glTexGenfv(){
	
}

void glDrawArrays(){
	
}

void gluScaleImage(){
	
}

void glPointSize(){
	
}

void glClearStencil(){
	
}

void glTexGeni(){
	
}


void gluProject(){
	
}

void glColor3f(){
	
}

void glColor4d(){
	
}

void glEnableClientState(){
	
}

void glFrontFace(){
	
}

void glColorMask(){
	
}

void glProgramLocalParameter4dARB(){
	
}


void glGetError(){
	
}

void glStencilOp(){
	
}

void glBindBufferARB(){
	
}

void glTexCoordPointer(){
	
}

void glGenBuffersARB(){
	
}

void glDisableClientState(){
	
}

void glDepthMask(){
	
}

void glVertexPointer(){
	
}

void glCopyTexSubImage2D(){
}

#endif

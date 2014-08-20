#include "Text.hpp"
#include "Helperlib/Log.hpp"

#include "conf.h"
#include "Bmp.hpp"

#include "endian.hpp"
#include <iostream>



#include "os.h"

#include "rsa.h"


using namespace std;


///This function gets the first power of 2 >= the
///int that we pass it.
inline unsigned int next_p2 ( unsigned int a )
{
	unsigned int rval=1;
	while(rval<a) rval<<=1;
	
	if(rval==1){rval=2;} //1 corrupts text
	return rval;

}

class FakeRasterize{
	public:
		float largestX;
		float largestY;
		bool largestXSet;
		bool largestYSet;
		
		float smallestX;
		float smallestY;
		bool smallestXSet;
		bool smallestYSet;
		
		Array<float> pushedRX;
		Array<float> pushedRY;

		void clear(){
			largestX=0;
			largestY=0;
			largestXSet=false;
			largestYSet=false;

			smallestX=0;
			smallestY=0;
			smallestXSet=false;
			smallestYSet=false;
		}

		float rX;
		float rY;

		void translate(float x,float y, float z){
			rX+=x;
			rY+=y;
		}

		void loadIdentity(){
			rX=0;
			rY=0;
		}

		void pushMatrix(){
			pushedRX.pushBack(rX);
			pushedRY.pushBack(rY);
		}

		void popMatrix(){
			rX=pushedRX[pushedRX.size()-1];
			pushedRX.erase(pushedRX.size()-1);

			rY=pushedRY[pushedRY.size()-1];
			pushedRY.erase(pushedRY.size()-1);
		}

		void vertex(float x,float y, float z){

			if( (rX+x)>largestX || !largestXSet){largestX=rX+x;largestXSet=true;}
			if( (rY+y)>largestY || !largestYSet){largestY=rY+y;largestYSet=true;}

			if( (rX+x)<smallestX || !smallestXSet){smallestX=rX+x;smallestXSet=true;}
			if( (rY+y)<smallestY || !smallestYSet){smallestY=rY+y;smallestYSet=true;}
		}

};

FakeRasterize fr;

FloatVector2d Font::getSizeBase(String input,bool displacement){

	if(input==""){
		return FloatVector2d(0,0);
	}

	make();

	FloatVector2d final;
	
	Array<std::string> lines;
	std::string buffer;
	for(int c=0; c<input.size(); c++){
		if(input[c]=='\n'){
			lines.pushBack(buffer);
			buffer="";
		}else{
			buffer+=input[c];
		}
	}
	lines.pushBack(buffer);

	for(int i=0; i<lines.size(); i++){
		for(int j=0; j<lines[i].size(); j++){
			if(lines[i][j]=='\t'){

				if((j+1)%4==0){
					lines[i]=lines[i].substr(0,j)+" "+lines[i].substr(j+1);
				}else if((j+1)%4==1){
					lines[i]=lines[i].substr(0,j)+"    "+lines[i].substr(j+1);
				}else if((j+1)%4==2){
					lines[i]=lines[i].substr(0,j)+"   "+lines[i].substr(j+1);
				}else if((j+1)%4==3){
					lines[i]=lines[i].substr(0,j)+"  "+lines[i].substr(j+1);
				}
			}
		}
	}
		
	    
	if(lines.size()==0){
		return FloatVector2d(0,0);
	}

	while(lines[lines.size()-1]==""){
		lines.erase(lines.size()-1);

		if(lines.size()==0){
			return FloatVector2d(0,0);
		}
	}

	while(lines[0]==""){
		lines.erase(0);

		if(lines.size()==0){
			return FloatVector2d(0,0);
		}
	}

	fr.clear();

	float offsetY=0;


	for(int j=0; j<lines.size(); j++){
	
		fr.pushMatrix();
		fr.loadIdentity();
		fr.translate(0,int(-offsetY),0);

		for(int i=0; i<lines[j].size(); i++){

			fr.pushMatrix();
			fr.translate(glyphs[lines[j][i]].offset.x,glyphs[lines[j][i]].offset.y,0);

			if(i>(lines[j].size()-2) || j>(lines.size()-2) || j==0 || i==0){
				fr.vertex(0,-glyphs[lines[j][i]].bitmapSize.y,0);
				fr.vertex(0,0,0);
				fr.vertex(glyphs[lines[j][i]].bitmapSize.x,0,0);
				fr.vertex(glyphs[lines[j][i]].bitmapSize.x,-glyphs[lines[j][i]].bitmapSize.y,0);
			}
			
			fr.popMatrix();
			fr.translate(glyphs[lines[j][i]].size.x*spacing,0,0);
		}

		fr.popMatrix();
		offsetY+=h*leading;
		

	}

	if(displacement){
		final.x=fr.largestX;
		final.y=-fr.smallestY;
	}else{
		final.x=fr.largestX-fr.smallestX;
		final.y=-fr.smallestY+fr.largestY;
	}


	return final;
}



FloatVector2d Font::getSize(String input){
	return getSizeBase(input,false);
}

FloatVector2d Font::getDisplacement(String input){
	return getSizeBase(input,true);
}


FloatVector2d Font::getSizeSmart(String input){


	if(input==""){
		return FloatVector2d(0,0);
	}

	make();
	
	Array<std::string> lines;
	std::string buffer;
	for(int c=0; c<input.size(); c++){
		if(input[c]=='\n'){
			lines.pushBack(buffer);
			buffer="";
		}else{
			buffer+=input[c];
		}
	}


	lines.pushBack(buffer);

	//tab algorithm
	for(int i=0; i<lines.size(); i++){
		for(int j=0; j<lines[i].size(); j++){
			if(lines[i][j]=='\t'){

				if((j+1)%4==0){
					lines[i]=lines[i].substr(0,j)+" "+lines[i].substr(j+1);
				}else if((j+1)%4==1){
					lines[i]=lines[i].substr(0,j)+"    "+lines[i].substr(j+1);
				}else if((j+1)%4==2){
					lines[i]=lines[i].substr(0,j)+"   "+lines[i].substr(j+1);
				}else if((j+1)%4==3){
					lines[i]=lines[i].substr(0,j)+"  "+lines[i].substr(j+1);
				}
			}
		}
	}

	if(lines.size()==0){
		return FloatVector2d(0,0);
	}

	while(lines[lines.size()-1]==""){
		lines.erase(lines.size()-1);

		if(lines.size()==0){
			return FloatVector2d(0,0);
		}
	}

	FloatVector2d final;

	for(int i=0; i<lines.size(); i++){
		FloatVector2d temp;

		final.y+=h*leading;
		if(getSize(lines[i]).x>final.x){
			final.x=getSize(lines[i]).x;
		}

	}

	return final;

}


bool Font::remakeSpacing(){
	
	glListBase(0);

	for(unsigned char ch=0;ch<128;ch++){
		glDeleteLists(listBase+ch,1);
		glNewList(listBase+ch,GL_COMPILE); //list_base+

		glPushMatrix();

		glTranslatef(glyphs[ch].offset.x,glyphs[ch].offset.y,0);
		
		glBegin(GL_QUADS);
			glTexCoord2d(glyphs[ch].textureSize.x,glyphs[ch].textureSize1.y);	glVertex3f(0,-glyphs[ch].bitmapSize.y,0);
			glTexCoord2d(glyphs[ch].textureSize.x,glyphs[ch].textureSize.y);	glVertex3f(0,0,0);
			glTexCoord2d(glyphs[ch].textureSize1.x,glyphs[ch].textureSize.y);	glVertex3f(glyphs[ch].bitmapSize.x,0,0);
			glTexCoord2d(glyphs[ch].textureSize1.x,glyphs[ch].textureSize1.y);	glVertex3f(glyphs[ch].bitmapSize.x,-glyphs[ch].bitmapSize.y,0);
		glEnd();
	
		glPopMatrix();
		
		glTranslatef(glyphs[ch].size.x*spacing,0,0);

		glEndList();
	}

	return true;
}

#ifdef MANSION_FREETYPE
bool Font::remakeFontFreetype(){
	logs().text << "[Font::remakeFont] remake " << file.getRelative() << endLine;

	glListBase(0);
		
	this->h=size*1.63;
	
	FT_Library library; //handle to library
 	FT_Face face; //handle to face object
	
	int error = FT_Init_FreeType( &library );
	
	if ( error ){
		logs().text.write("failed createing libaray");
		return false;
	}

	error = FT_New_Face( library, file.getRelative().c_str(), 0, &face ); 

	if ( error == FT_Err_Unknown_File_Format ) {
		//... the font file could be opened and read, but it appears ... that its font format is unsupported 
		logs().text.write("font '"+file.getRelative()+"' could not be read");
                return false;
        } else if ( error ) { 
		//... another error code means that the font file could not ... be opened or read, or simply that it is broken... 
		logs().text.write("font '"+file.getRelative()+"' missing or broken");
                return false;
	}
	

	FT_Set_Char_Size( face, 0, size*64, 96, 96);  //mac..


	//delete old textures
	if(textureSet){
		glDeleteTextures(1,(GLuint*)&texture);
	}
	textureSet=true;
	glGenTextures(1,(GLuint*)&texture);
	
	for(unsigned char ch=0;ch<128;ch++){  //don't need up to 30

		FT_Glyph glyph;

		
		if(antialiased){
			if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT  )){
				logs().text.write("FT_Load_Glyph failed");
				return false;
			}
		}else{
			if(FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_MONOCHROME  )){
				logs().text.write("FT_Load_Glyph failed");
				return false;
			}
		}
	
    	if(FT_Get_Glyph( face->glyph, &glyph )){
			logs().text.write("FT_Get_Glyph failed");
		
			return false;
		}
	
 		if ( glyph->format==FT_GLYPH_FORMAT_BITMAP ) {    
			logs().text.write("BAD glyph format");
			return false;
		}
	
		if(antialiased){
			if(FT_Glyph_To_Bitmap( &glyph,  FT_RENDER_MODE_NORMAL , 0, 1 )){ //FT_RENDER_MODE_LIGHT //FT_RENDER_MODE_NORMAL //FT_RENDER_MODE_MONO
				FT_Done_Glyph(glyph);
				logs().text.write("error on FT_Glyph_to_bitmap");
				return false;
			}
		}else{
			if(FT_Glyph_To_Bitmap( &glyph,  FT_RENDER_MODE_MONO , 0, 1 )){ //FT_RENDER_MODE_LIGHT //FT_RENDER_MODE_NORMAL //FT_RENDER_MODE_MONO
				FT_Done_Glyph(glyph);
				logs().text.write("error on FT_Glyph_to_bitmap");
				return false;
			}
		}
		
		
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		FT_Bitmap* bitmap=&bitmap_glyph->bitmap;

		
		glyphs[ch].data=new unsigned char[bitmap->width*bitmap->rows];
		
		if(antialiased){
			for(int i=0; i<(bitmap->rows*bitmap->width); i++){
				glyphs[ch].data[i]=bitmap->buffer[i];
			}
		}else{

			int a=0;
			int b=0;
			int c=0;
			
			for(int h=0; h<bitmap->rows; h++){
			
				for(int w=0; w<bitmap->width; w++){
					a=w%8;
					

     				if(a==0 && (bitmap->buffer[c] & 0x80)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==1 && (bitmap->buffer[c] & 0x40)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==2 && (bitmap->buffer[c] & 0x20)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==3 && (bitmap->buffer[c] & 0x10)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==4 && (bitmap->buffer[c] & 0x08)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==5 && (bitmap->buffer[c] & 0x04)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==6 && (bitmap->buffer[c] & 0x02)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else if(a==7 && (bitmap->buffer[c] & 0x01)){
						glyphs[ch].data[h*bitmap->width+w]=0xFF;
					}else{
						glyphs[ch].data[h*bitmap->width+w]=0x00;
					}
    				
    				b++;
    				
    				if(8==b){
    					c++;
    					b=0;
    				}
    				
				}
				
				if(b>0){
					b=0;
					c++;
				}
				
			}
		}
		
		if(maxX<bitmap->width){maxX=bitmap->width;}
		if(maxY<bitmap->rows){maxY=bitmap->rows;}
		
		glyphs[ch].offset.x=bitmap_glyph->left;
		glyphs[ch].offset.y=bitmap_glyph->top;

		glyphs[ch].size.x=(face->glyph->advance.x >> 6);
		
		glyphs[ch].bitmapSize.x=bitmap->width;
		glyphs[ch].bitmapSize.y=bitmap->rows;
		FT_Done_Glyph( glyph );   
	}

	for(int i=0; i<128;i++){
		glyphs[i].offset.y=-maxY+glyphs[i].offset.y;
	}

	/////////////////////////////////////////////now combine them into a big map
	
	//the map is 8char wide X16char long, evenly spaced
	int w=next_p2(8*maxX);
	int h=next_p2(16*maxY);
	
	logs().text << "maxX" << maxX << endLine;
	logs().text << "maxY" << maxY << endLine;

	waste=(((8*float(maxX))*(16*float(maxY)))/float(h*w))*100;
	
	maxX=0;
	maxY=0;

	unsigned char* bigtex=new unsigned char[w*h];
	
	//width/height position
	int wp=0;
 	int hp=0;
 	
 	//real width/height for each character
	int wi=w/8;	//will always come out even because it's a power of 2
	int hi=h/16;
	
	for(unsigned char ch=0; ch<128; ch++){
	
		for(int j=0; j<hi; j++) {	//height
			for(int i=0; i<wi; i++){	//width
				if(i>=glyphs[ch].bitmapSize.x || j>=glyphs[ch].bitmapSize.y){
					bigtex[i+(j*w)+(hp*hi*w)+(wp*wi)]=0;
				}else{
					bigtex[i+(j*w)+(hp*hi*w)+(wp*wi)]=glyphs[ch].data[i + (j*glyphs[ch].bitmapSize.x)];
				}
			}
		}
		
		
		wp++;
		
		if(wp==8){
			wp=0;
			hp++;
		}
		
		
		delete []glyphs[ch].data;
	}

	logs().text << "wp" << wp << endLine;
	logs().text << "hp" << hp << endLine;
	
	logs().text << "w" << w << endLine;
	logs().text << "h" << h << endLine;
	
	glBindTexture( GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	//GL_LINEAR looks a lot better then GL_NEAREST
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);  //GL_LINEAR
	
	glTexImage2D( GL_TEXTURE_2D, 0, GL_ALPHA, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, bigtex );

	
	delete[] bigtex;
	
	////////////////////////////////////now generate display lists for each char
	logs().text << "generate display lists" << endLine;
	
	wp=0;
	hp=0;
	
	for(unsigned char ch=0; ch<128; ch++){
		glDeleteLists(listBase+ch,1);
  		glNewList(listBase+ch,GL_COMPILE);

		glPushMatrix();
		
		glTranslatef(glyphs[ch].offset.x,glyphs[ch].offset.y,0);
		
		
		float x=float(wp*wi)/float(w);
		float y=float(hp*hi)/float(h);
		
		float x1=float((wp*wi)+glyphs[ch].bitmapSize.x)/float(w);
		float y1=float((hp*hi)+glyphs[ch].bitmapSize.y)/float(h);
		
		glyphs[ch].textureSize.x=x;
		glyphs[ch].textureSize.y=y;
		glyphs[ch].textureSize1.x=x1;
		glyphs[ch].textureSize1.y=y1;
		
		glBegin(GL_QUADS);
			glTexCoord2d(x,y1); glVertex3f(0,float(-glyphs[ch].bitmapSize.y),0);
			glTexCoord2d(x,y); glVertex3f(0,0,0);
			glTexCoord2d(x1,y); glVertex3f(float(glyphs[ch].bitmapSize.x),0,0);
			glTexCoord2d(x1,y1); glVertex3f(float(glyphs[ch].bitmapSize.x),float(-glyphs[ch].bitmapSize.y),0);
		glEnd();
	
		glPopMatrix();
		
		glTranslatef(glyphs[ch].size.x*spacing ,0,0);
		
		wp++;
		
		if(wp==8){
			wp=0;
			hp++;
		}
		
		glEndList();

	}
	
	//done
	
	FT_Done_Face(face);

	FT_Done_FreeType(library);

	return true;
}

#endif

bool Font::remakeFontBitmap(){
	

	logs().text << "[Font::remakeFont] remake " << file.getRelative() << endLine;

	#ifdef RSA
		String rvs=rsa_verify_file(os().path(file).c_str());

		if(rvs==""){
			logs().text << "Sign Success" << endLine;
		}else{
			logs().text << "Sign Error: " << rvs << endLine;
			return false;
		}
	#endif


	ifstream in;

	in.open(file.getAbsolute().c_str(), ios::in|ios::binary);
	
	if(in==NULL){
		logs().text << "[Font::remakeFont] Error, font " << file.getAbsolute() << " doesn't exist" << endLine;
		return false;
	}

	in.seekg (0, ios::beg);

	char* header=new char[4];

	

	in.read (header, 4);
	
	type=FONT_TYPE_BITMAP;

	if(header[0]!='L' ||
		header[1]!='M' ||
		header[2]!='F' ||
		header[3]!='1' ||
		file.getExtension()!="lmf"){

		logs().main << String("unknown font format :") << String(header)  << endLine;

	}

	in.seekg(256,ios::cur);	//the sig

	int totalwidth;
	int totalheight;


	in.read ((char*)&totalwidth, 4);
	in.read ((char*)&totalheight, 4);
	totalwidth=endian(totalwidth);
	totalheight=endian(totalheight);

	float maxH=0;
	float maxY=0;

	for(unsigned char ch=0; ch<128; ch++){
		in.read((char*)&glyphs[ch].size.x,sizeof(glyphs[ch].size.x));
		glyphs[ch].size.x=endian(glyphs[ch].size.x);
		in.read((char*)&glyphs[ch].size.y,sizeof(glyphs[ch].size.y));
		glyphs[ch].size.y=endian(glyphs[ch].size.y);
		in.read((char*)&glyphs[ch].offset.x,sizeof(glyphs[ch].offset.x));
		glyphs[ch].offset.x=endian(glyphs[ch].offset.x);
		in.read((char*)&glyphs[ch].offset.y,sizeof(glyphs[ch].offset.y));
		glyphs[ch].offset.y=endian(glyphs[ch].offset.y);
		in.read((char*)&glyphs[ch].bitmapSize.x,sizeof(glyphs[ch].bitmapSize.x));
		glyphs[ch].bitmapSize.x=endian(glyphs[ch].bitmapSize.x);
		in.read((char*)&glyphs[ch].bitmapSize.y,sizeof(glyphs[ch].bitmapSize.y));
		glyphs[ch].bitmapSize.y=endian(glyphs[ch].bitmapSize.y);
		in.read((char*)&glyphs[ch].textureSize.x,sizeof(glyphs[ch].textureSize.x));
		glyphs[ch].textureSize.x=endian(glyphs[ch].textureSize.x);
		in.read((char*)&glyphs[ch].textureSize.y,sizeof(glyphs[ch].textureSize.y));
		glyphs[ch].textureSize.y=endian(glyphs[ch].textureSize.y);
		in.read((char*)&glyphs[ch].textureSize1.x,sizeof(glyphs[ch].textureSize1.x));
		glyphs[ch].textureSize1.x=endian(glyphs[ch].textureSize1.x);
		in.read((char*)&glyphs[ch].textureSize1.y,sizeof(glyphs[ch].textureSize1.y));
		glyphs[ch].textureSize1.y=endian(glyphs[ch].textureSize1.y);

		if(glyphs[ch].bitmapSize.y>maxH){maxH=glyphs[ch].bitmapSize.y;}
		if(glyphs[ch].bitmapSize.y>maxY){maxY=glyphs[ch].bitmapSize.y;}

	}

	int wp=0;
	int hp=0;
	
	int wi=totalwidth/8;	//will always come out even because it's a power of 2
	int hi=totalheight/16;

	

	long size=totalwidth*totalheight;

	this->h=maxH;

	unsigned char* bigtex=new unsigned char[size];

	in.read((char*)bigtex,size);

	if(textureSet){
		glDeleteTextures(1,(GLuint*)&texture);
	}
	textureSet=true;
	glGenTextures(1,(GLuint*)&texture);
	glBindTexture( GL_TEXTURE_2D, texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, totalwidth, totalheight, 0, GL_ALPHA , GL_UNSIGNED_BYTE, bigtex);
	
	delete[] bigtex;

	for(int i=0; i<128;i++){
		glyphs[i].offset.y=-maxY+glyphs[i].offset.y;
	}


	for(unsigned char ch=0; ch<128; ch++){
		glDeleteLists(listBase+ch,1);
  		glNewList(listBase+ch,GL_COMPILE);

		glPushMatrix();
		
		glTranslatef(glyphs[ch].offset.x,glyphs[ch].offset.y,0);
		
		float x=float(wp*wi)/float(totalwidth);
		float y=float(hp*hi)/float(totalheight);
		
		float x1=float((wp*wi)+glyphs[ch].bitmapSize.x)/float(totalwidth);
		float y1=float((hp*hi)+glyphs[ch].bitmapSize.y)/float(totalheight);

		
		glyphs[ch].textureSize.x=x;
		glyphs[ch].textureSize.y=y;
		glyphs[ch].textureSize1.x=x1;
		glyphs[ch].textureSize1.y=y1;
		
		glBegin(GL_QUADS);
			glTexCoord2d(x,y1); glVertex3f(0,float(-glyphs[ch].bitmapSize.y),0);
			glTexCoord2d(x,y); glVertex3f(0,0,0);
			glTexCoord2d(x1,y); glVertex3f(float(glyphs[ch].bitmapSize.x),0,0);
			glTexCoord2d(x1,y1); glVertex3f(float(glyphs[ch].bitmapSize.x),float(-glyphs[ch].bitmapSize.y),0);
		glEnd();
	
		glPopMatrix();
		
		glTranslatef(glyphs[ch].size.x*spacing ,0,0);
		
		wp++;
		
		if(wp==8){
			wp=0;
			hp++;
		}
		
		glEndList();
	}

	in.close();

	return true;
}

bool Font::remakeFont(){

	if(file.getFilenameNoExtension().size()<1){
		String t=file.getFilenameNoExtension();

		logs().text.write("Error, file "+file.getFilenameNoExtension());
		return false;
	}

	String ext=file.getExtension();

	if(file.getExtension()=="lmf"){
		return remakeFontBitmap();
	}else{
	
		#ifdef MANSION_FREETYPE
			return remakeFontFreetype();
		#else
			console().write("ERROR, could not make font '"+file.getRelative()+"' because freetype is not included with this build.");
			return false;
		#endif

	}


}
bool Font::make(){

	if((size!=lastSize) || (file!=lastFile) || (tb!=lastTb) || (lastAntialiased!=antialiased)){
		
		if(!remakeFont()){
			return false;
		}
		
		lastSize=size;
		lastFile=file;
		lastTb=tb;
		lastAntialiased=antialiased;
	}else if(spacing != lastSpacing){

		if(!remakeSpacing()){
			return false;
		}
		
		lastSpacing=spacing;
	}

	return true;
}

void Font::clean() {
	glListBase(0);
	
	glDeleteLists(listBase,128);
	if(textureSet){
		glDeleteTextures(1,(GLuint*)&texture);
	}
	
	//delete [] textures;  should probably have this but it causes crash?
}


void Font::print(float x, float y, String text)  {
	print(x,y,0,text);
}

void Font::print(float x, float y, float z, String text)  {
	make();		//remake the font if needed

	//SLOW - should optimize this code that splits string into vector on \n
	
	Array<std::string> lines;
	std::string buffer;
	
	for(int c=0; c<text.size(); c++){
		if(text[c]=='\n'){
			lines.pushBack(buffer);
			buffer="";
		}else{
			buffer+=text[c];
		}
	}
	
	lines.pushBack(buffer);

	//tab algorithm
	for(int i=0; i<lines.size(); i++){
		for(int j=0; j<lines[i].size(); j++){
			if(lines[i][j]=='\t'){

				if((j+1)%4==0){
					lines[i]=lines[i].substr(0,j)+" "+lines[i].substr(j+1);
				}else if((j+1)%4==1){
					lines[i]=lines[i].substr(0,j)+"    "+lines[i].substr(j+1);
				}else if((j+1)%4==2){
					lines[i]=lines[i].substr(0,j)+"   "+lines[i].substr(j+1);
				}else if((j+1)%4==3){
					lines[i]=lines[i].substr(0,j)+"  "+lines[i].substr(j+1);
				}
			}
		}
	}

	// We want a coordinate system where things coresponding to window pixels.
	glViewport (0, 0, (GLsizei)(conf->sizeX), (GLsizei)(conf->sizeY));

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();


	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPushMatrix();
	glLoadIdentity();							// Reset The Projection Matrix
	glOrtho(0,conf->sizeX,-conf->sizeY,0,-100,100);						// Set Up An Ortho Screen (left,right,bottom,top,znear,zfar)
              
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();


	glListBase(listBase);

#ifdef LOOP_DEBUG
	logs().main.write("text::print texture DISABLE");
#endif

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

	glActiveTextureARB(GL_TEXTURE2_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);

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

	glActiveTextureARB(GL_TEXTURE0_ARB);

	glDisable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_LIGHTING);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    

	glBindTexture(GL_TEXTURE_2D,texture);

	//This is where the text display actually happens.
	//For each line of text we reset the modelview matrix
	//so that the line's text will start in the correct position.
	//Notice that we need to reset the matrix, rather than just translating
	//down by h. This is because when each character is
	//draw it modifies the current matrix so that the next character
	//will be drawn immediatly after it.  
	
	float offsetY=0;	//the line height offset
	

	for(int i=0;i<lines.size();i++){


		glPushMatrix();
		glLoadIdentity();

		int rx=int(x);	//round (so taht it doesn't land at half pixel and look like shit)
		int ry=int(y-offsetY);	//round (so taht it doesn't land at half pixel and look like shit)

		glTranslatef(float(rx),float(ry),z);
		
		glScalef(rescaleFactor.x,rescaleFactor.y,1);

	//  The commented out raster position stuff can be useful if you need to
	//  know the length of the text that you are creating.
	//  If you decide to use it make sure to also uncomment the glBitmap command
	//  in make_dlist().
	//	glRasterPos2f(0,0);

		glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());


		glPopMatrix();

		offsetY+=h*leading*rescaleFactor.y;
	}

	glDisable(GL_TEXTURE_2D);
	
	
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
			
	glListBase(0);
}

bool Font::init(){
	glListBase(0);

	logs().text.write("[Font::Init] init");

	listBase=glGenLists(128);

	logs().text << "listBase:" << listBase << endLine;

	bool r=make();

	return r;
}



Font::Font(){
	listBase=0;
	
	size=10;
	lastSize=10;

	file="fonts/arial.ttf";
	lastFile=""; //do not set lastFile, so that it gets rebuilt at start
	leading=1;
	spacing=1;
	lastSpacing=1;
	tb=false;
	tb2=false;

	waste=0;
	antialiased=true;
 	lastAntialiased=true;
        maxX=0;
        maxY=0;
	rescaleFactor.x=1;
	rescaleFactor.y=1;

	type=FONT_TYPE_FREETYPE;
	
	texture=0;
	textureSet=false;
}


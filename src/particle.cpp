#include "particle.h"
#include "random.h"
#include "HelperLibMath.hpp"
#include "Log.hpp"
#include "timeing.hpp"

#include "level.hpp"
#include "texture.hpp"
#include "hardware.hpp"

#include "GlHeader.hpp"

ParticleState::ParticleState(){
	reSpawn=true;
	life=0;
	kill=0;
	visible=false;
	seed=0;
}


int luaParticleLoad(lua_State *L){
	//get panel name from script
	String name;
	String file;

	Array<LuaType> args=script().getArgs(L);


	if(args.size()!=2){
		console().write("Particle2d Error: Bad arguments");
		return 0;
	}

	if(args[0].type==SCRIPT_TABLE){

		for(int i=0; i<args[0].children.size(); i++){

			if(args[0].children[i].name=="name"){
				name=args[0].children[i].value.ss;
			}

		}

	}else{
		console().write("Particle2d error: bad arguments");
	}

	if(args[1].type==SCRIPT_STRING_STORED){

		file=args[1].value.ss;

	}else{
		console().write("Particle2d error: bad arguments");
	}


	if(name==""){
		console().write("Particle2d error (internal): no name given");
		return 0;
	}

	Particle* t=level->particleKey[name];

	t->load(file);

	return 0;
}



bool Particle::load(Path file){
	int g=texture.load(file);

	ImageData im=texture.info(g);

	if(g!=-1){
		texid=g;

		script().syncTo(NULL);

		return true;
	}

	console().write("Error loading file '"+file.getRelative()+"', "+im.error);
	return false;
}


void Particle::remake(ParticleState* p){
	p->visible=true;
	p->life=0;
	p->kill=life+(lifeRandom*randomGen.Random());
	p->seed=randomGen.Random();
	p->color.x=color.x+randomGen.Random()*colorRandom.x;
	p->color.y=color.y+randomGen.Random()*colorRandom.y;
	p->color.z=color.z+randomGen.Random()*colorRandom.z;
	p->color.w=color.w+randomGen.Random()*colorRandom.w;
	p->pos.x=pos.x+(randomGen.Random()*area.x)-(area.x/2);
	p->pos.y=pos.y+(randomGen.Random()*area.y)-(area.y/2);
	p->pos.z=pos.z+(randomGen.Random()*area.z)-(area.z/2);
	p->velocity.x=velocity.x+(velocityRandom.x*randomGen.Random());
	p->velocity.y=velocity.y+(velocityRandom.y*randomGen.Random());
	p->velocity.z=velocity.z+(velocityRandom.z*randomGen.Random());

	float rsc=scaleRandom.x*pow((float)randomGen.Random(),(float)scaleBias.x);
	p->scale.x=scale.x+rsc;

	rsc=scaleRandom.y*pow((float)randomGen.Random(),(float)scaleBias.y);
	p->scale.y=scale.y+rsc;

	rsc=scaleRandom.z*pow((float)randomGen.Random(),(float)scaleBias.z);
	p->scale.z=scale.z+rsc;

	if(ratio!=0){
		p->scale.y=p->scale.x*(1/ratio);
	}

	if(!p->reSpawn){p->visible=false;}
}

void Particle::draw(){
	
	int numberR=number;
	int oldNumberR=oldNumber;

	if(numberR!=oldNumberR){
		if(numberR<0){numberR=0;number=0;}

		if(numberR<oldNumberR){
			for(int i=numberR; i<oldNumberR; i++){
				particles[i].reSpawn=false;
			}

		}else if(numberR>oldNumberR){
			ParticleState* oldParticles=NULL;
			oldParticles=particles;

			particles=new ParticleState[numberR];

			for(int i=0; i<oldNumberR; i++){
				particles[i]=oldParticles[i];
				particles[i].reSpawn=true;
			}

			for(int i=oldNumberR; i<numberR; i++){
				remake(&particles[i]);
			}

			if(oldParticles!=NULL){
				delete[] oldParticles;
			}
		}

		oldNumber=number;
		oldNumberR=numberR;
	}else{

		//update the particles

		for(int i=0; i<numberR; i++){

			if(particles[i].life>particles[i].kill){
				if(on){
					remake(&particles[i]);
				}else{
					particles[i].visible=false;
				}

			}else{
				particles[i].life+=engineTime.time;

				float frameSeed=randomGen.Random();

				if( (particles[i].seed*frameSeed) <velocityJitterFreq.x){
					particles[i].velocity.x+=velocityJitterAmp.x*frameSeed-(velocityJitterAmp.x/2);
				}

				if( (particles[i].seed*frameSeed) <velocityJitterFreq.y){
					particles[i].velocity.y+=velocityJitterAmp.y*frameSeed-(velocityJitterAmp.y/2);
				}

				if( (particles[i].seed*frameSeed) <velocityJitterFreq.z){
					particles[i].velocity.z+=velocityJitterAmp.z*frameSeed-(velocityJitterAmp.z/2);
				}

				particles[i].pos.x+=particles[i].velocity.x*engineTime.time;
				particles[i].pos.y+=particles[i].velocity.y*engineTime.time;
				particles[i].pos.z+=particles[i].velocity.z*engineTime.time;
			}

		}
	}

	//now draw

	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

#ifdef LOOP_DEBUG
	logs().main.write("particle texture DISABLE");
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
	shader.reset();

	glColor4f(color.x,color.y,color.z,color.w);

	if(type=="lines"){

		if(hardware.vbo){
			glLineWidth(thickness);

			float* data=new float[numberR*6];

			for(int i=0; i<numberR; i++){
				data[i*6]=particles[i].pos.x;
				data[i*6+1]=particles[i].pos.y;
				data[i*6+2]=particles[i].pos.z;

				data[i*6+3]=particles[i].pos.x+particles[i].scale.x;
				data[i*6+4]=particles[i].pos.y+particles[i].scale.y;
				data[i*6+5]=particles[i].pos.z+particles[i].scale.z;

			}

			if(!vboset){
				glGenBuffersARB(1, (GLuint*)&buffer);
				vboset=true;
			}

			glBindBufferARB(GL_ARRAY_BUFFER_ARB, buffer);

			glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(float)*numberR*6, data, GL_DYNAMIC_DRAW_ARB);

			glVertexPointer(3, GL_FLOAT, 0, 0);
			glEnableClientState(GL_VERTEX_ARRAY);

			glDrawArrays(GL_LINES, 0, numberR);

			glDisableClientState(GL_VERTEX_ARRAY);
		}else{
			glLineWidth(thickness);

			glBegin(GL_LINES);

			for(int i=0; i<numberR; i++){

				
				if(particles[i].visible){
					glVertex3f(particles[i].pos.x,
						particles[i].pos.y,
						particles[i].pos.z);

					glVertex3f(particles[i].pos.x+particles[i].scale.x,
						particles[i].pos.y+particles[i].scale.y,
						particles[i].pos.z+particles[i].scale.z);
				}
			}

			glEnd();
		}
	}else if(type=="sprite" && texid!=-1){

		glEnable(GL_DEPTH_TEST);
		glDepthMask(false);

		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texid);

		glEnable(GL_BLEND);

		glBegin(GL_QUADS);

		for(int i=0; i<numberR; i++){

			glColor4f(particles[i].color.x,particles[i].color.y,particles[i].color.z,particles[i].color.w);
		

			float xzrot=0;
			xzrot=level->camera->rot.y;
			float c=math.cos(180-xzrot);
			float s=math.sin(180-xzrot);

			if(orient=="none" || orient==""){
				c=1;
				s=0;
			}

			glTexCoord2f(0,0);
			glVertex3f(particles[i].pos.x,
					particles[i].pos.y,
					particles[i].pos.z);

			glTexCoord2f(1,0);
			glVertex3f(particles[i].pos.x+c*particles[i].scale.x,
					particles[i].pos.y,
					particles[i].pos.z+s*particles[i].scale.x);

			glTexCoord2f(1,1);
			glVertex3f(particles[i].pos.x+c*particles[i].scale.x,
					particles[i].pos.y-particles[i].scale.y,
					particles[i].pos.z+s*particles[i].scale.x);

			glTexCoord2f(0,1);
			glVertex3f(particles[i].pos.x,
					particles[i].pos.y-particles[i].scale.y,
					particles[i].pos.z);

			
		}
		glEnd();

		glDepthMask(true);
		
		

	}else{
			
		glEnable(GL_BLEND);
		

		for(int i=0; i<numberR; i++){

			glPointSize(particles[i].scale.x);
	
			glColor4f(particles[i].color.x,particles[i].color.y,particles[i].color.z,particles[i].color.w);
		
			glBegin(GL_POINTS);

			glVertex3f(particles[i].pos.x,
					particles[i].pos.y,
					particles[i].pos.z);
			glEnd();
		}

		
	}

}

Particle::Particle(){
	color.x=0.2;
	color.y=0.2;
	color.z=0.2;
	color.w=1;

	color.x=1;
	color.y=0;
	color.z=0;

	gravity.x=0;
	gravity.y=0;
	gravity.z=0;

	pos.x=0;
	pos.y=0;
	pos.z=0;

	scale.x=0;
	scale.y=-1;
	scale.z=0;

	area.x=10;
	area.y=0;
	area.z=10;

	velocity.x=0;
	velocity.y=-1;
	velocity.z=0;

	velocityRandom.x=0;
	velocityRandom.y=-1;
	velocityRandom.z=0;
	

	visible=true;

	number=100;
	oldNumber=0;

	life=100;
	lifeRandom=100;

	
	type="lines";

	orient="xyz";

	particles=NULL;

	on=true;

	texid=-1;

	ratio=0;

	thickness=1;

	vboset=false;
}

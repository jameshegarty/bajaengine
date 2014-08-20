#include "particle2d.h"
#include "random.h"
#include "HelperLibMath.hpp"
#include "timeing.hpp"
#include "texture.hpp"
#include "script.hpp"
#include "level.hpp"

#include "GlHeader.hpp"

#include <math.h>

Particle2dState::Particle2dState(){

}

void Particle2d::remake(Particle2dState* p){
	p->color.x=color.x+randomGen.Random()*colorRandom.x;
	p->color.y=color.y+randomGen.Random()*colorRandom.y;
	p->color.z=color.z+randomGen.Random()*colorRandom.z;
	p->color.w=color.w+randomGen.Random()*colorRandom.w;

	p->seed=randomGen.Random();
	p->visible=true;
	p->life=0;
	p->kill=life+(lifeRandom*randomGen.Random());
	p->pos.x=pos.x+(randomGen.Random()*area.x);
	p->pos.y=pos.y+(randomGen.Random()*area.y);


	float rsc=scaleRandom.x*pow((float)randomGen.Random(),(float)scaleBias.x);
	p->scale.x=scale.x+rsc;

	rsc=scaleRandom.y*pow((float)randomGen.Random(),(float)scaleBias.x);
	p->scale.y=scale.y+rsc;

	if(ratio!=0){
		p->scale.y=p->scale.x*(1/ratio);
	}

	p->velocity.x=velocity.x+velocityRandom.x*randomGen.Random();
	p->velocity.y=velocity.y+velocityRandom.y*randomGen.Random();
}


int luaParticle2dLoad(lua_State *L){

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

	Particle2d* t=level->particle2dKey[name];

	t->load(file);

	return 0;
}



bool Particle2d::load(String file){
	int g=texture.load(file);

	ImageData im=texture.info(g);

	if(g!=-1){

		texid=g;
		

		script().syncTo(NULL);

		return true;
	}

	console().write("Error loading file '"+file+"', "+im.error);
	return false;
}


void Particle2d::draw(){
	
	if(number!=oldNumber){
		if(particles!=NULL){
			delete[] particles;
		}

		particles=new Particle2dState[number];

		

		for(int i=0; i<number; i++){
			remake(&particles[i]);
		}

		oldNumber=number;

	}else{

		//update the particles

		for(int i=0; i<number; i++){

			if(particles[i].life>particles[i].kill){
				if(on){
					remake(&particles[i]);
				}else{
					particles[i].visible=false;
				}

			}else{
				float frameSeed=randomGen.Random();
				bool cc=false;

				if( (particles[i].seed*frameSeed) <velocityJitterFreq.x){
					particles[i].velocity.x+=velocityJitterAmp.x*frameSeed-(velocityJitterAmp.x/2);
					cc=true;
				}

				if( (particles[i].seed*frameSeed) <velocityJitterFreq.y){
					particles[i].velocity.y+=velocityJitterAmp.y*frameSeed-(velocityJitterAmp.y/2);
					cc=true;
				}


				particles[i].life+=engineTime.time;
				particles[i].pos.x+=particles[i].velocity.x*engineTime.time;
				particles[i].pos.y+=particles[i].velocity.y*engineTime.time;
			}

		}
	}


	//now draw
	

	glDisable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	if(type=="lines"){
		glBegin(GL_LINES);

		for(int i=0; i<number; i++){
			glColor4f(particles[i].color.x,particles[i].color.y,particles[i].color.z,particles[i].color.w);

			
			if(particles[i].visible){
				glVertex3f(particles[i].pos.x,
					particles[i].pos.y,0);

				glVertex3f(particles[i].pos.x+scale.x,
					particles[i].pos.y+scale.y,0);
			}
		}

		glEnd();
	}else if(type=="sprite" && texid!=-1){
		glActiveTextureARB(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D,texid);
		glEnable(GL_BLEND);


		glBegin(GL_QUADS);

		for(int i=0; i<number; i++){
			glColor4f(particles[i].color.x,particles[i].color.y,particles[i].color.z,particles[i].color.w);


			if(particles[i].visible){
				glTexCoord2f(0,0);
				glVertex3f(particles[i].pos.x,
					particles[i].pos.y,0);

				glTexCoord2f(1,0);
				glVertex3f(particles[i].pos.x+particles[i].scale.x,
					particles[i].pos.y,0);

				glTexCoord2f(1,1);
				glVertex3f(particles[i].pos.x+particles[i].scale.x,
					particles[i].pos.y+particles[i].scale.y,0);

				glTexCoord2f(0,1);
				glVertex3f(particles[i].pos.x,
					particles[i].pos.y+particles[i].scale.y,0);
			}
		}

		glEnd();

	}else{

		glBegin(GL_QUADS);

		for(int i=0; i<number; i++){
			glColor4f(particles[i].color.x,particles[i].color.y,particles[i].color.z,particles[i].color.w);


			if(particles[i].visible){
			
				glVertex3f(particles[i].pos.x,
					particles[i].pos.y,0);

	
				glVertex3f(particles[i].pos.x+particles[i].scale.x,
					particles[i].pos.y,0);

	
				glVertex3f(particles[i].pos.x+particles[i].scale.x,
					particles[i].pos.y+particles[i].scale.y,0);

		
				glVertex3f(particles[i].pos.x,
					particles[i].pos.y+particles[i].scale.y,0);
			}
		}

		glEnd();
	}

}

Particle2d::Particle2d(){
	color.x=0.2;
	color.y=0.2;
	color.z=0.2;
	color.w=1;

	color.x=1;
	color.y=1;
	color.z=1;
	color.w=1;

	pos.x=0;
	pos.y=0;

	scale.x=0;
	scale.y=-1;

	area.x=10;
	area.y=10;

	velocity.x=1;
	velocity.y=1;
	velocityRandom.x=1;
	velocityRandom.y=1;

	number=100;
	oldNumber=0;

	life=100;
	lifeRandom=100;

	texid=-1;

	type="sprite";
	ratio=0;

	particles=NULL;

	on=true;
}

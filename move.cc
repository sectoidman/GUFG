#include "move.h"
#include <assert.h>


move::move()
{
	name = NULL;
}

move::move(char * n)
{
	build(n);
	init();
}

move::~move()
{
	if(!this) return;
/*	for(int i = 0; i < frames; i++){
		if(sprite[i] != NULL) SDL_FreeSurface(sprite[i]);
		if(fSprite[i] != NULL) SDL_FreeSurface(fSprite[i]);
	}*/
	if(sprite) delete [] sprite;
	if(fSprite) delete [] fSprite;
	if(collision) delete [] collision;
	if(hitbox) delete [] hitbox;
	if(hitreg) delete [] hitreg;
	if(hitComplexity) delete [] hitComplexity;
	if(regComplexity) delete [] regComplexity;
	if(deltaComplexity) delete [] deltaComplexity;
	if(delta) delete [] delta;
	if(state) delete [] state;
	if(blockMask) delete [] blockMask;
	if(lift) delete [] lift;
	if(damage) delete [] damage;
	if(stun) delete [] stun;
	if(totalStartup) delete [] totalStartup;
	if(push) delete [] push;
}

void move::build(char * n)
{
	ifstream read;
	int startup, recovery, countFrames = -1;
	char fname[40];
	char buffer[100];
	sprintf(fname, "%s.mv", n);
	read.open(fname);
	assert(!read.fail());
	
	while(read.get() != ':'); read.ignore();
	name = n;
	
	while(read.get() != ':'); read.ignore();
	read >> tolerance;
	while(read.get() != ':'); read.ignore();
	read >> activation;
	while(read.get() != ':'); read.ignore();
	read >> hits;

	state = new cancelField[hits+1];

	while(read.get() != ':'); read.ignore();
	read >> frames;
	
	if(hits > 0) {
		totalStartup = new int[hits];
		damage = new int[hits];
		stun = new int[hits]; 
		push = new int[hits]; 
		lift = new int[hits]; 
		blockMask = new blockField[hits]; 
	} else {
		blockMask = NULL; lift = NULL; push = NULL; stun = NULL; damage = NULL; totalStartup = NULL;
	}
	int active[hits];

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> startup;
		countFrames += startup;
		totalStartup[i] = countFrames;
		while(read.get() != ':'); read.ignore();
		read >> active[i];
		countFrames += active[i];
	}
 
	while(read.get() != ':'); read.ignore();
	read >> recovery;

	while(read.get() != ':'); read.ignore();
	read >> allowed.i;

	for(int i = 0; i < hits+1; i++){
		while(read.get() != ':'); read.ignore();
		read >>	state[i].i;
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> damage[i];
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stun[i];
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> push[i];
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> lift[i];
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> blockMask[i].i;
	}

	while(read.get() != ':'); read.ignore();
	read >> blockState.i;

	//Properties will be a bit more complicated, I'll add this later.
	collision = new SDL_Rect[frames];
	hitbox = new SDL_Rect*[frames];
	hitComplexity = new int[frames];
	hitreg = new SDL_Rect*[frames];
	regComplexity = new int[frames];
	delta = new SDL_Rect*[frames];
	deltaComplexity = new int[frames];

	currentHit = 0;

	for(int i = 0; i < frames; i++)
	{
		while(read.get() != '$'); read.ignore(2);
		read >> collision[i].x >> collision[i].y >> collision[i].w >> collision[i].h;
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		regComplexity[i] = 1;
		for(unsigned int j = 0; j < strlen(buffer); j++){
			if(buffer[j] == '\t') regComplexity[i]++;
		}
		hitreg[i] = new SDL_Rect[regComplexity[i]];
		char * bb[regComplexity[i]*4];
		bb[0] = strtok(buffer, ",\n\t ");
		for(int j = 1; j < regComplexity[i]*4; j++){
			bb[j] = strtok(NULL, ", \n\t"); j++;
			bb[j] = strtok(NULL, ", \n\t"); j++;
			bb[j] = strtok(NULL, ", \n\t"); j++;
			bb[j] = strtok(NULL, ", \n\t");
		}
		for(int j = 0; j < regComplexity[i]*4; j++){
			hitreg[i][j/4].x = atoi(bb[j]); j++;
			hitreg[i][j/4].y = atoi(bb[j]); j++;
			hitreg[i][j/4].w = atoi(bb[j]); j++;
			hitreg[i][j/4].h = atoi(bb[j]);
		}
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		deltaComplexity[i] = 1;
		for(unsigned int j = 0; j < strlen(buffer); j++){
			if(buffer[j] == '\t') regComplexity[i]++;
		}
		delta[i] = new SDL_Rect[deltaComplexity[i]];
		char * dd[deltaComplexity[i]*4];
		dd[0] = strtok(buffer, ",\n\t ");
		for(int j = 1; j < deltaComplexity[i]*4; j++){
			dd[j] = strtok(NULL, ", \n\t"); j++;
			dd[j] = strtok(NULL, ", \n\t"); j++;
			dd[j] = strtok(NULL, ", \n\t"); j++;
			dd[j] = strtok(NULL, ", \n\t");
		}
		for(int j = 0; j < deltaComplexity[i]*4; j++){
			delta[i][j/4].x = atoi(dd[j]); j++;
			delta[i][j/4].y = atoi(dd[j]); j++;
			delta[i][j/4].w = atoi(dd[j]); j++;
			delta[i][j/4].h = atoi(dd[j]);
		}
		if(hits > 0){
			if(i > totalStartup[currentHit] && i <= totalStartup[currentHit]+active[currentHit]){
				while(read.get() != '$'); read.ignore(2);
				read.get(buffer, 100, '\n');
				hitComplexity[i] = 1;
				for(unsigned int j = 0; j < strlen(buffer); j++){
					if(buffer[j] == '\t') hitComplexity[i]++;
				}
				hitbox[i] = new SDL_Rect[hitComplexity[i]];
				char* rr[hitComplexity[i]*4];
				rr[0] = strtok(buffer, ",\n\t ");
	
				for(int j = 1; j < hitComplexity[i]*4; j++){
					rr[j] = strtok(NULL, ", \n\t"); j++;
					rr[j] = strtok(NULL, ", \n\t"); j++;
					rr[j] = strtok(NULL, ", \n\t"); j++;
					rr[j] = strtok(NULL, ", \n\t");
				}
	
				for(int j = 0; j < hitComplexity[i]*4; j++){
					hitbox[i][j/4].x = atoi(rr[j]); j++;
					hitbox[i][j/4].y = atoi(rr[j]); j++;
					hitbox[i][j/4].w = atoi(rr[j]); j++;
					hitbox[i][j/4].h = atoi(rr[j]);
				}
				if(i == totalStartup[currentHit]+active[currentHit]) currentHit++;
			} else {
				hitComplexity[i] = 1;
				hitbox[i] = new SDL_Rect[hitComplexity[1]];
				hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
			}
		} else {
			hitComplexity[i] = 1;
			hitbox[i] = new SDL_Rect[hitComplexity[1]];
			hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
		}
	}

	launch = 0;
	for(int i = 0; i < 5; i++)
		button[i] = 0;
	int r = strlen(name);
	for(int i = 0; i < r; i++){
		switch(name[i]){
		case 'A':
			button[0] = 1;
			break;
		case 'B':
			button[1] = 1;
			break;
		case 'C':
			button[2] = 1;
			break;
		case 'D':
			button[3] = 1;
			break;
		case 'E':
			button[4] = 1;
			break;
		default:
			break;
		}

	}
	read.close();
	sprite = new SDL_Surface*[frames];
	fSprite = new SDL_Surface*[frames];
	for(int i = 0; i < frames; i++){
		sprintf(fname, "%s#%i.png", n, i);
		sprite[i] = aux::load_image(fname);
		sprintf(fname, "%s#%iF.png", n, i);
		fSprite[i] = aux::load_image(fname);
	}
}

bool move::check(bool pos[5], bool neg[5], int t, int f)
{
	//if(meter < cost) return 0;
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		}

	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return 1;
}

void move::pollRects(SDL_Rect &c, SDL_Rect* &r, int &rc, SDL_Rect* &b, int &hc)
{
	if(rc > 0) delete [] r;
	if(hc > 0) delete [] b;
	rc = regComplexity[currentFrame];
	hc = hitComplexity[currentFrame];
	r = new SDL_Rect[rc];
	b = new SDL_Rect[hc];

	c.x = collision[currentFrame].x; c.w = collision[currentFrame].w;
	c.y = collision[currentFrame].y; c.h = collision[currentFrame].h;

	SDL_Rect * tempreg = hitreg[currentFrame];
	for(int i = 0; i < rc; i++){
		r[i].x = tempreg[i].x; r[i].w = tempreg[i].w;
		r[i].y = tempreg[i].y; r[i].h = tempreg[i].h;
	}
	SDL_Rect * temphit = hitbox[currentFrame];
	for(int i = 0; i < hc; i++){
		if(cFlag > currentHit) {
			b[i].x = 0; b[i].w = 0;
			b[i].y = 0; b[i].h = 0;
		} else {
			b[i].x = temphit[i].x; b[i].w = temphit[i].w;
			b[i].y = temphit[i].y; b[i].h = temphit[i].h;
		}
	}
}


bool move::operator>(move * x)
{
	if(x->state[x->cFlag].i == 0) return 0;
	if(x == NULL) return 1;
	if(frames == 0 || x->frames == 0) return 0;
	else if(allowed.i & x->state[x->cFlag].i) return 1;
	return 0;
}

void move::step()
{
	currentFrame++;
	if(currentHit < hits-1 && currentFrame > totalStartup[currentHit+1]) currentHit++;
}

void move::init()
{
	cFlag = 0;
	currentFrame = 0;
	currentHit = 0;
}

void move::connect()
{
	cFlag++;
}

void move::blockSuccess(int st)
{
	return;
}

void move::execute(move * last)
{
	last->init();
}


#include <stdio.h>
#include <iostream>
#include <fstream>
#include "move.h"
#include <assert.h>
using namespace std;

move::move()
{
	name = NULL;
}

move::move(const char * n)
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
	if(totalStartup) delete [] totalStartup;
	if(name) delete [] name;
	if(stats) delete [] stats;
}

void move::build(const char * n)
{
	ifstream read;
	int startup, recovery, countFrames = -1;
	char fname[40];
	char buffer[100];
	buffer[0] = '\0';

	sprintf(fname, "%s.mv", n);
	read.open(fname);
	assert(!read.fail());
	
	while(read.get() != ':'); read.ignore();
	name = new char[strlen(n)+1];	
	sprintf(name, "%s", n);

	while(read.get() != ':'); read.ignore();
	read >> tolerance;
	while(read.get() != ':'); read.ignore();
	read >> activation;
	while(read.get() != ':'); read.ignore();
	read >> hits;

	state = new cancelField[hits+1];
	gain = new int[hits+1];

	while(read.get() != ':'); read.ignore();
	read >> frames;
	
	if(hits > 0) {
		totalStartup = new int[hits];
		stats = new hStat[hits];
	} else {
		totalStartup = NULL;
		stats = NULL;
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
		read >> stats[i].damage;
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].stun;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].untech;
	}
	
	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].push;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].lift;
	}

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].blowback;
	}
	
	for(int i = 0; i < hits+1; i++){
		while(read.get() != ':'); read.ignore();
		read >>	gain[i];
	}
	
	while(read.get() != ':'); read.ignore();
	read >> cost;

	for(int i = 0; i < hits; i++){
		while(read.get() != ':'); read.ignore();
		read >> stats[i].blockMask.i;
	}
	
	while(read.get() != ':'); read.ignore();
	read >> blockState.i;

	while(read.get() != ':'); read.ignore();
	read.getline(buffer, 100);
/*Debug*/
//	printf("%s properties: %s\n", name, buffer);
	stop = 0;
	crouch = 0;
	int ch = 0;
	for(unsigned int i = 0; i < strlen(buffer); i++){
		switch(buffer[i]){
		case '^': 
			stats[ch].launch = 1;
			break;
		case 's':
			stop = 1;
			break;
		case 'S': 
			stop = 2;
			break;
		case 'c':
			crouch = 1;
			break;
		case ':':
			ch++;
			break;
		default:
			break;
		}
	}
	
	//Properties will be a bit more complicated, I'll add this later.
	
	collision = new SDL_Rect[frames];
	hitbox = new SDL_Rect*[frames];
	hitComplexity = new int[frames];
	hitreg = new SDL_Rect*[frames];
	regComplexity = new int[frames];
	delta = new SDL_Rect*[frames];
	deltaComplexity = new int[frames];

	currentHit = 0;

	for(int i = 0; i < frames; i++){
		while(read.get() != '$'); read.ignore(2);
		read >> collision[i].x >> collision[i].y >> collision[i].w >> collision[i].h;
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		regComplexity[i] = aux::defineRectArray(buffer, hitreg[i]);
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		deltaComplexity[i] = aux::defineRectArray(buffer, delta[i]);
		if(hits > 0){
			if(i > totalStartup[currentHit] && i <= totalStartup[currentHit]+active[currentHit]){
				while(read.get() != '$'); read.ignore(2);
				read.get(buffer, 100, '\n');
				hitComplexity[i] = aux::defineRectArray(buffer, hitbox[i]);
				if(i == totalStartup[currentHit]+active[currentHit]) currentHit++;
			} else {
				hitComplexity[i] = 1;
				hitbox[i] = new SDL_Rect[1];
				hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
			}
		} else {
			hitComplexity[i] = 1;
			hitbox[i] = new SDL_Rect[1];
			hitbox[i][0].x = 0; hitbox[i][0].y = 0; hitbox[i][0].w = 0; hitbox[i][0].h = 0;
		}
	}
	next = NULL;
	read.close();
	
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
	sprite = new SDL_Surface*[frames];
	fSprite = new SDL_Surface*[frames];
	for(int i = 0; i < frames; i++){
		sprintf(fname, "%s#%i.png", n, i);
		sprite[i] = aux::load_image(fname);
		sprintf(fname, "%s#%iF.png", n, i);
		fSprite[i] = aux::load_image(fname);
	}
}

bool move::check(bool pos[5], bool neg[5], int t, int f, int resource[])
{
	for(int i = 0; i < 5; i++){
		if(button[i] == 1){
			if(!pos[i]) return 0;
		}

	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	if(cost > resource[0]) return 0;
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

void move::pollStats(hStat & s)
{
	s.damage = stats[currentHit].damage;
	s.stun = stats[currentHit].stun;
	s.push = stats[currentHit].push;
	s.lift = stats[currentHit].lift;
	s.untech = stats[currentHit].untech;
	s.launch = stats[currentHit].launch;
	s.blowback = stats[currentHit].blowback;
	s.blockMask.i = stats[currentHit].blockMask.i;	
}

bool move::operator>(move * x)
{	
	if(x == NULL) return 1;
	else{
		if(allowed.i & x->state[x->cFlag].i){
			if(x == this){
				if(x->cFlag == 0) return 0;
				else if(allowed.i & 4) return 1;
				else return 0;
			}
			else return 1;
		}
	}
	return 0;
}

void move::step(int *& resource)
{
	if(currentFrame == 0){
		if(resource[0] + gain[0] < 200) resource[0] += gain[0];
		else resource[0] = 200;
	}
	currentFrame++;
	if(currentHit < hits-1 && currentFrame > totalStartup[currentHit+1]) currentHit++;
}

void move::init()
{
	cFlag = 0;
	currentFrame = 0;
	currentHit = 0;
}

void move::connect(int *& resource)
{
	cFlag++;
	if(resource[0] + gain[cFlag] < 200) resource[0] += gain[cFlag];
	else resource[0] = 200; 
}

void move::blockSuccess(int st)
{
	return;
}

void move::execute(move * last, int *& resource)
{
	resource[0] -= cost;
	last->init();
}

void move::feed(move * c, int i)
{
	next = c;
}

bool move::takeHit(hStat & s)
{
	if(s.blockMask.i & blockState.i) return 0;
	else{
		init();
		return 1;
	}
}

bool move::CHState()
{
	if(hits < 1) return false;
	else if(currentFrame < totalStartup[hits-1]) return true;
	else return false;
}

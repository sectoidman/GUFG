#include <stdio.h>
#include <iostream>
#include <fstream>
#include "move.h"
#include <assert.h>
using namespace std;

move::move() : frames(0), hits(0), name(NULL)
{
	name = NULL;
}

move::move(const char * n) : frames(0), hits(0)
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
	char fname[40];
	char buffer[100];
	char savedBuffer[100];
	buffer[0] = '\0';

	sprintf(fname, "%s.mv", n);
	read.open(fname);
	assert(!read.fail());

	do {
		read.getline(buffer, 100);
		strcpy(savedBuffer, buffer);
	} while (setParameter(buffer));

	parseProperties(savedBuffer);

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
	int r = strlen(n);
	for(int i = 0; i < r; i++){
		switch(n[i]){
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

	SDL_Surface *temp;
	width = new int[frames];
	height = new int[frames];
	sprite = new GLuint[frames];
	for(int i = 0; i < frames; i++){
		sprintf(fname, "%s#%i.png", n, i);
		temp = aux::load_image(fname);
		if(!temp){
			width[i] = 0;
			height[i] = 0;
			sprite[i] = 0;
		} else {
			width[i] = temp->w;
			height[i] = temp->h;
			sprite[i] = aux::surface_to_texture(temp);
		}
	}
}

bool move::setParameter(char * buffer)
{
	char* token = strtok(buffer, "\t: \n");

	if(!strcmp("Name", token)){
		token = strtok(NULL, "\t:\n");
		name = new char[strlen(token)+1];
		sprintf(name, "%s", token);
//		printf(": %s\n", name);
		return 1;
	} else if (!strcmp("Buffer", token)) {
		token = strtok(NULL, "\t: \n");
		tolerance = atoi(token);
		token = strtok(NULL, "\t: \n");
		activation = atoi(token);
//		printf("Buffer: %i : %i\n", tolerance, activation);
		return 1;
	} else if (!strcmp("Hits", token)) {
		token = strtok(NULL, "\t: \n");
		hits = atoi(token);
		state = new cancelField[hits+1];
		gain = new int[hits+1];
//		printf("Hits: %i\n", hits);
		return 1;
	} else if (!strcmp("Blocks", token)) {
		token = strtok(NULL, "\t: \n");
		blockState.i = atoi(token);
//		printf("Blocks: %i\n", blockState.i);
		return 1;
	} else if (!strcmp("Check", token)) {
		token = strtok(NULL, "\t: \n");
		allowed.i = atoi(token);
//		printf("Check: %i\n", allowed.i);
		return 1;
	} else if (!strcmp("Cost", token)) {
		token = strtok(NULL, "\t: \n");
		cost = atoi(token);
//		printf("Cost: %i\n", cost);
		return 1;
	} else if (!strcmp("Frames", token)) {
		token = strtok(NULL, "\t: \n");
		frames = atoi(token);
//		printf("Frames: %i\n", frames);
		int startup, countFrames = -1;
		if(hits > 0) {
			totalStartup = new int[hits];
			stats = new hStat[hits];
			active = new int[hits];
		} else {
			totalStartup = NULL;
			stats = NULL;
			active = NULL;
		}

		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			startup = atoi(token);
			countFrames += startup;
			totalStartup[i] = countFrames;
			token = strtok(NULL, "\t: \n");
			active[i] = atoi(token);
			countFrames += active[i];
		}
		return 1;
	} else if (!strcmp("State", token)) {
//		printf("State");
		for(int i = 0; i < hits+1; i++){
			token = strtok(NULL, "\t: \n");
			state[i].i = atoi(token);
//			printf(": %i ", state[i].i);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Damage", token)) {
//		printf("Damage");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].damage = atoi(token);
//			printf(": %i ", stats[i].damage);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Push", token)) {
//		printf("Push");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].push = atoi(token);
//			printf(": %i ", stats[i].push);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Lift", token)) {
//		printf("Lift");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].lift = atoi(token);
//			printf(": %i ", stats[i].lift);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Blowback", token)) {
//		printf("Blowback");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].blowback = atoi(token);
//			printf(": %i ", stats[i].blowback);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Stun", token)) {
//		printf("Stun");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].stun = atoi(token);
//			printf(": %i ", stats[i].stun);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Untech", token)) {
//		printf("Untech");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].untech = atoi(token);
//			printf(": %i ", stats[i].untech);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Blockable", token)) {
//		printf("Blockable");
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].blockMask.i = atoi(token);
//			printf(": %i ", stats[i].blockMask.i);
		}
//		printf("\n");
		return 1;
	} else if (!strcmp("Gain", token)) {
//		printf("Gain");
		for(int i = 0; i < hits+1; i++){
			token = strtok(NULL, "\t: \n");
			gain[i] = atoi(token);
//			printf(": %i ", gain[i]);
		}
//		printf("\n");
		return 1;
	} else return 0;
}

void move::parseProperties(char * buffer)
{
	char * token = strtok(buffer, " \t\n:");
	token = strtok(NULL, "\n");
	/*Debug*/
//	printf("%s properties: %s\n", name, buffer);
	int ch = 0;
//	printf("%s: ", name);
	for(unsigned int i = 0; i < strlen(token); i++){
//		printf("%c ", token[i]);
		switch(token[i]){
		case '^': 
			stats[ch].launch = 1;
			break;
		case 'g':
			stats[ch].ghostHit = 1;
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
		case 't':
			throwinvuln = 1;
			break;
		case 'T':
			throwinvuln = 2;
			break;
		default:
			break;
		}
	}
//	printf("\n");
}

bool move::check(bool pos[5], bool neg[5], int t, int f, int resource[], SDL_Rect &p)
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
	s.ghostHit = stats[currentHit].ghostHit;
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
	cFlag = currentHit+1;
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



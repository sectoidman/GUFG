#include <stdio.h>
#include <iostream>
#include <fstream>
#include "action.h"
#include <assert.h>
using namespace std;

action::action() : frames(0), hits(0), name(NULL)
{
	name = NULL;
}

action::action(const char * n) : frames(0), hits(0)
{
	build(n);
}

action::~action()
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
	if(gain) delete [] gain;
	if(distortion) delete distortion;
	if(totalStartup) delete [] totalStartup;
	if(name) delete [] name;
	if(stats) delete [] stats;
	if(next) delete next;
	if(onConnect) delete [] onConnect;
}

void action::zero()
{
	attemptStart = 0;
	attemptEnd = 0;
	xRequisite = 0;
	yRequisite = 0;
	stop = 0;
	hits = 0;
	throwinvuln = 0;
	minHold = 1;
	maxHold = 1;
	crouch = 0;
	armorStart = 0; armorLength = 0;
	armorHits = 0;
	guardStart = 0; guardLength = 0;
	blockState.i = 0;
	isProjectile = 0;
	stats = NULL;
	cost = 0;
	dies = 0;
	fch = 0;
	armorCounter = 0;
	distortSpawn = -1;
	distortion = NULL;
	tempNext = NULL;
	tempAttempt = NULL;
	tempRiposte = NULL;
	displaceFrame = -1;
	displaceX = 0;
	displaceY = 0;
	soundClip = NULL;
	next = NULL;
	attempt = NULL;
	riposte = NULL;
}

void action::build(const char * n)
{
	zero();
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

	parseProperties(savedBuffer, 0);

	collision = new SDL_Rect[frames];
	hitbox = new SDL_Rect*[frames];
	hitComplexity = new int[frames];
	hitreg = new SDL_Rect*[frames];
	regComplexity = new int[frames];
	delta = new SDL_Rect*[frames];
	deltaComplexity = new int[frames];

	int currHit = 0;

	for(int i = 0; i < frames; i++){
		while(read.get() != '$'); read.ignore(2);
		read >> collision[i].x >> collision[i].y >> collision[i].w >> collision[i].h;
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		regComplexity[i] = aux::defineRectArray(buffer, hitreg[i]);
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		deltaComplexity[i] = aux::defineRectArray(buffer, delta[i]);
		if(hits > 0 && currHit < hits){
			if(i > totalStartup[currHit] && i <= totalStartup[currHit]+active[currHit]){
				while(read.get() != '$'); read.ignore(2);
				read.get(buffer, 100, '\n');
				hitComplexity[i] = aux::defineRectArray(buffer, hitbox[i]);
				if(i == totalStartup[currHit]+active[currHit]) currHit++;
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
	read.close();

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
	sprintf(fname, "%s.ogg", n);
	soundClip = Mix_LoadWAV(fname);
}

bool action::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);
	char* token = strtok(buffer, "\t:+ \n");

	if(!strcmp("Name", token)){
		token = strtok(NULL, "\t:\n");
		name = new char[strlen(token)+1];
		sprintf(name, "%s", token);
		return 1;
	} else if (!strcmp("Displace", token)) {
		token = strtok(NULL, "\t:\n");
		displaceFrame = atoi(token);
		token = strtok(NULL, "\t:\n");
		displaceX = atoi(token);
		token = strtok(NULL, "\t:\n");
		displaceY = atoi(token);
		return 1;
	} else if (!strcmp("Buffer", token)) {
		token = strtok(NULL, "\t: \n");
		tolerance = atoi(token);
		token = strtok(NULL, "\t: \n");
		activation = atoi(token);
		return 1;
	} else if(!strcmp("Proximity", token)){
		token = strtok(NULL, "\t: \n");
		xRequisite = atoi(token); 

		token = strtok(NULL, "\t: \n");
		yRequisite = atoi(token); 
		return 1;
	} else if (!strcmp("Hold", token)) {
		token = strtok(NULL, "\t: \n-");
		minHold = atoi(token);

		token = strtok(NULL, "\t: \n-");
		maxHold = atoi(token);
		return 1;
	} else if (!strcmp("Counterhit", token)) {
		parseProperties(savedBuffer, 1);
		return 1;
	} else if (!strcmp("Hits", token)) {
		token = strtok(NULL, "\t: \n");
		hits = atoi(token);
		if(hits > 0){
			stats = new hStat[hits];
			CHStats = new hStat[hits];
			onConnect = new action*[hits];
			tempOnConnect = new char*[hits];
			for (int i = 0; i < hits; i++){
				onConnect[i] = NULL;
				tempOnConnect[i] = NULL;
				stats[i].hitState.i = 0;
			}
		} else {
			stats = NULL;
			onConnect = NULL;
		}
		state = new cancelField[hits+1];
		gain = new int[hits+1];
		for(int i = 0; i < hits+1; i++)
			gain[i] = 0;
		return 1;
	} else if (!strcmp("Riposte", token)) {
		token = strtok(NULL, "\t: \n");
		tempRiposte = new char[strlen(token)+1];
		strcpy(tempRiposte, token);
		return 1;
	} else if (!strcmp("Distort", token)) {
		distortion = new attractor;
		token = strtok(NULL, "\t:- \n");
		distortSpawn = atoi(token);
		token = strtok(NULL, "\t:- \n");
		distortion->length = atoi(token);
		distortion->length -= distortSpawn;
		token = strtok(NULL, "\t: \n");
		distortion->x = atoi(token);
		token = strtok(NULL, "\t: \n");
		distortion->y = atoi(token);
		return 1;
	} else if (!strcmp("AttractorType", token)) {
		if(!distortion) return 1;
		token = strtok(NULL, "\t: \n");
		distortion->type = atoi(token);
		token = strtok(NULL, "\t: \n");
		distortion->radius = atoi(token);
		token = strtok(NULL, "\t: \n");
		distortion->ID = atoi(token);
		return 1;
	} else if (!strcmp("Next", token)) {
		token = strtok(NULL, "\t: \n");
		tempNext = new char[strlen(token)+1];
		strcpy(tempNext, token);
		return 1;
	} else if (!strcmp("Attempt", token)) {
		token = strtok(NULL, "\t: \n-");
		attemptStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		attemptEnd = atoi(token); 

		token = strtok(NULL, "\t: \n");
		tempAttempt = new char[strlen(token)+1];
		strcpy(tempAttempt, token);
		return 1;
	} else if (!strcmp("Connect", token)) {
		token = strtok(NULL, "\t: \n");
		int x = atoi(token);
		token = strtok(NULL, "\t: \n");
		tempOnConnect[x] = new char[strlen(token)+1];
		strcpy(tempOnConnect[x], token);
		return 1;
	} else if (!strcmp("Blocks", token)) {
		token = strtok(NULL, "\t: \n");
		blockState.i = atoi(token);
		return 1;
	} else if (!strcmp("Check", token)) {
		token = strtok(NULL, "\t: \n");
		allowed.i = atoi(token);
		return 1;
	} else if (!strcmp("Cost", token)) {
		token = strtok(NULL, "\t: \n");
		cost = atoi(token);
		return 1;
	} else if (!strcmp("Frames", token)) {
		token = strtok(NULL, "\t: \n");
		frames = atoi(token);
		int startup, countFrames = -1;
		if(hits > 0) {
			totalStartup = new int[hits];
			active = new int[hits];
		} else {
			totalStartup = NULL;
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
		for(int i = 0; i < hits+1; i++){
			token = strtok(NULL, "\t: \n");
			state[i].i = atoi(token);
		}
		return 1;
	} else if (!strcmp("HitAllows", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].hitState.i = atoi(token);
		}
		return 1;
	} else if (!strcmp("Damage", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].damage = atoi(token);
			else stats[i].damage = atoi(token);
		}
		return 1;
	} else if (!strcmp("Chip", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].chip = atoi(token);
		}
		return 1;
	} else if (!strcmp("Push", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+') 
				CHStats[i].push = atoi(token);
			else stats[i].push = atoi(token);
		}
		return 1;
	} else if (!strcmp("Lift", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].lift = atoi(token);
			else stats[i].lift = atoi(token);
		}
		return 1;
	} else if (!strcmp("Float", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].hover = atoi(token);
			else stats[i].hover = atoi(token);
		}
		return 1;
	} else if (!strcmp("Blowback", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].blowback = atoi(token);
			else stats[i].blowback = atoi(token);
		}
		return 1;
	} else if (!strcmp("Stun", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].stun = atoi(token);
			else {
				stats[i].stun = atoi(token);
				CHStats[i].stun = (stats[i].stun - 5) / 2;
			}
		}
		return 1;
	} else if (!strcmp("Untech", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].untech = atoi(token);
			else{
				stats[i].untech = atoi(token);
				CHStats[i].untech = 10;
			}
		}
		return 1;
	} else if (!strcmp("Blockable", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].blockMask.i = atoi(token);
		}
		return 1;
	} else if (!strcmp("Gain", token)) {
		for(int i = 0; i < hits+1; i++){
			token = strtok(NULL, "\t: \n");
			gain[i] = atoi(token);
		}
		return 1;
	} else if (!strcmp("Autoguard", token)) {
		token = strtok(NULL, "\t: \n-");
		guardStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		guardLength = atoi(token); 
		guardLength = guardLength - guardStart;
		return 1;
	} else if (!strcmp("Armor", token)) {
		token = strtok(NULL, "\t: \n-");
		armorStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		armorLength = atoi(token); 
		armorLength = armorLength - armorStart;
		return 1;
	} else if (!strcmp("MaxArmor", token)) {
		token = strtok(NULL, "\t: \n-");
		armorHits = atoi(token); 
		return 1;
	} else return 0;
}

void action::parseProperties(char * buffer, bool counter)
{
	char * token = strtok(buffer, " \t\n:");
	token = strtok(NULL, "\n");
	/*Debug*/
	int ch = 0;
	for(unsigned int i = 0; i < strlen(token); i++){
		switch(token[i]){
		case '^':
			if(counter) CHStats[ch].launch = 1;
			else stats[ch].launch = 1;
			break;
		case '>':
			if(counter) CHStats[ch].wallBounce = 1;
			else stats[ch].wallBounce = 1;
			break;
		case 'v':
			if(counter) CHStats[ch].floorBounce = 1;
			else stats[ch].floorBounce = 1;
			break;
		case '_':
			if(counter) CHStats[ch].slide = 1;
			else stats[ch].slide = 1;
			break;
		case '=':
			if(counter) CHStats[ch].stick = 1;
			else stats[ch].stick = 1;
			break;
		case 's':
			if(!counter) stop = 1;
			break;
		case 'S': 
			if(!counter) stop = 2;
			break;
		case '*':
			if(!counter) stop = 3;
			break;
		case 'c':
			if(!counter) crouch = 1;
			break;
		case ':':
			ch++;
			break;
		case 't':
			if(!counter) throwinvuln = 1;
			break;
		case 'T':
			if(!counter) throwinvuln = 2;
			break;
		case 'g':
			if(!counter) stats[ch].ghostHit = 1;
			break;
		case 'p':
			if(!counter) stats[ch].hitsProjectile = 1;
			break;
		case 'd':
			if(!counter) dies = 1;
			break;
		case 'C':
			if(!counter) fch = 1;
			break;
		case 'h':
			if(!counter) hidesMeter = 1;
			break;
		default:
			break;
		}
	}
}

bool action::window(int f)
{
	if(!attempt) return 0;
	if(f < attemptStart) return 0;
	if(f > attemptEnd) return 0;
	return 1;
}

bool action::activate(int pos[5], bool neg[5], int pattern, int t, int f, int meter[], SDL_Rect &p)
{
	for(int i = 0; i < 5; i++){
		if(pattern & (1 << i)){
			if(pos[i] < minHold) return 0;
			if(maxHold && pos[i] > maxHold) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

bool action::check(SDL_Rect &p, int meter[])
{
	if(cost > meter[1]) return 0;
	if(xRequisite > 0 && p.w > xRequisite) return 0;
	if(yRequisite > 0 && p.h > yRequisite) return 0;
	return 1;
}

void action::pollRects(SDL_Rect &c, SDL_Rect* &r, int &rc, SDL_Rect* &b, int &hc, int f, int cFlag)
{
	if(f >= frames) f = frames-1;
	if(rc > 0 && r) delete [] r;
	if(hc > 0 && b) delete [] b;
	rc = regComplexity[f];
	hc = hitComplexity[f];
	r = new SDL_Rect[rc];
	b = new SDL_Rect[hc];

	c.x = collision[f].x; c.w = collision[f].w;
	c.y = collision[f].y; c.h = collision[f].h;

	SDL_Rect * tempreg = hitreg[f];
	for(int i = 0; i < rc; i++){
		r[i].x = tempreg[i].x; r[i].w = tempreg[i].w;
		r[i].y = tempreg[i].y; r[i].h = tempreg[i].h;
	}
	SDL_Rect * temphit = hitbox[f];
	for(int i = 0; i < hc; i++){
		if(cFlag > calcCurrentHit(f)) {
			b[i].x = 0; b[i].w = 0;
			b[i].y = 0; b[i].h = 0;
		} else {
			b[i].x = temphit[i].x; b[i].w = temphit[i].w;
			b[i].y = temphit[i].y; b[i].h = temphit[i].h;
		}
	}
}

void action::pollDelta(SDL_Rect *& d, int & dc, int f)
{
	dc = deltaComplexity[f];
	d = new SDL_Rect[dc];
	SDL_Rect * tempdelta = delta[f];
	for(int i = 0; i < dc; i++){
		d[i].x = tempdelta[i].x; d[i].w = tempdelta[i].w;
		d[i].y = tempdelta[i].y; d[i].h = tempdelta[i].h;
	}
}

int action::displace(int x, int &y)
{
	y += displaceY;
	return displaceX;
}

void action::pollStats(hStat & s, int f, bool CH)
{
	int c = calcCurrentHit(f);
	s.damage = stats[c].damage + CHStats[c].damage * CH;
	s.chip = stats[c].chip;
	s.stun = stats[c].stun + CHStats[c].stun * CH;
	s.push = stats[c].push + CHStats[c].push * CH;
	s.lift = stats[c].lift + CHStats[c].lift * CH;
	s.untech = stats[c].untech + CHStats[c].untech * CH;
	s.blowback = stats[c].blowback + CHStats[c].blowback * CH;
	if(CH){
		s.launch = CHStats[c].launch || stats[c].launch;
		s.hover = CHStats[c].hover;
		s.wallBounce = CHStats[c].wallBounce;
		s.floorBounce = CHStats[c].floorBounce;
		s.slide = CHStats[c].slide;
		s.stick = CHStats[c].stick;
		s.ghostHit = CHStats[c].ghostHit;
	} else {
		s.launch = stats[c].launch;
		s.hover = stats[c].hover;
		s.wallBounce = stats[c].wallBounce;
		s.floorBounce = stats[c].floorBounce;
		s.slide = stats[c].slide;
		s.stick = stats[c].stick;
		s.ghostHit = stats[c].ghostHit;
	}
	s.blockMask.i = stats[c].blockMask.i;
}

bool action::cancel(action * x, int& c, int &h)
{
	cancelField r;
	r.i = x->state[c].i;
	if(h > 0 && h == c){ 
		r.i = r.i + x->stats[h - 1].hitState.i;
	}
	if(x == NULL) return 1;
	else{
		if(allowed.i & r.i){
			if(x == this){
				if(c == 0) return 0;
				else if(allowed.b.chain1) return 1;
				else return 0;
			}
			else return 1;
		}
	}
	return 0;
}

void action::step(int *& meter, int &f)
{
	if(f == 0){
		if(meter[1] + gain[0] < 300) meter[1] += gain[0];
		else meter[1] = 300;
	}
	f++;
}

int action::calcCurrentHit(int frame)
{
	int b = 0;
	for(int i = 0; i < hits; i++){
		if(frame > totalStartup[i]) b = i;
	} 
	return b;
}

action * action::connect(int *& meter, int &c, int f)
{
	c = calcCurrentHit(f)+1;
	if(meter[1] + gain[c] < 300) meter[1] += gain[c];
	else meter[1] = 300;
	if(onConnect[c-1] != NULL){
		return onConnect[c-1];
	}
	else return NULL;
}

action * action::blockSuccess()
{
	if(riposte) return riposte;
	else return this;
}

void action::playSound(int channel)
{
	Mix_PlayChannel(channel, soundClip, 0);
}

void action::execute(action * last, int *& meter, int &f, int &c, int &h)
{
	armorCounter = 0;
	meter[1] -= cost;
	f = 0;
	c = 0;
	h = 0;
}

void action::feed(action * c, int code, int i)
{
	switch(code){
	case 0:
		next = c;
		if(tempNext) delete [] tempNext;
		break;
	case 2:
		onConnect[i] = c;
		if(tempOnConnect[i]) delete [] tempOnConnect[i];
		break;
	case 3:
		attempt = c;
		if(tempAttempt) delete [] tempAttempt;
		break;
	case 5:
		riposte = c;
		if(tempRiposte) delete [] tempRiposte;
		break;
	}
}

char * action::request(int code, int i)
{
	switch(code){
	case 0: 
		return tempNext;
	case 2: 
		return tempOnConnect[i];
	case 3:
		return tempAttempt;
	case 5:
		return tempRiposte;
	default:
		return NULL;
	}
}

int action::takeHit(hStat & s, int b, int &f, int &c, int &h)
{
	if(s.blockMask.i & blockState.i && f > guardStart && f < guardStart + guardLength){
		if(riposte != NULL) return -2;
		else return 0;
	}
	else if (f > armorStart && f < armorStart + armorLength && (armorHits < 1 || armorHits < armorCounter)){
		s.stun = 0;
		armorCounter++;
		return 1;
	} else {
		if(s.stun != 0){
			f = 0;
			c = 0;
			h = 0;
		}
		return 1;
	}
}

bool action::CHState(int f)
{
	if(hits < 1) return false;
	else if(f < totalStartup[hits-1] + active[hits-1]) return true;
	else return fch;
}


#include <stdio.h>
#include <iostream>
#include <fstream>
#include "action.h"
#include "player.h"
#include <assert.h>
using namespace std;
action::action() : frames(0), hits(0)
{
}

action::action(const char * n) : frames(0), hits(0)
{
	build(n);
}

action::~action()
{
	if(!this) return;
	if(distortion) delete distortion;
	if(next) delete next;
}

void action::zero()
{
	offX = 0;
	offY = 0;
	linkable = 0;
	guardType = 0;
	attemptStart = 0;
	attemptEnd = 0;
	holdCheck = -1;
	holdFrame = -1;
	xRequisite = 0;
	yRequisite = 0;
	stunMin = 0;
	stunMax = 0;
	stop = 0;
	hits = 0;
	throwinvuln = 0;
	minHold = 1;
	maxHold = 1;
	crouch = 0;
	armorStart = -1; armorLength = 0;
	armorHits = 0;
	guardStart = -1; guardLength = 0;
	freezeFrame = -1; freezeLength = 0;
	blockState.i = 0;
	cost = 0;
	dies = 0;
	fch = 0;
	track = false;
	armorCounter = 0;
	distortSpawn = -1;
	distortion = NULL;
	tempNext = NULL;
	tempAttempt = NULL;
	tempRiposte = NULL;
	tempOnHold = NULL;
	tempPayload = NULL;
	displaceFrame = -1;
	displaceX = 0;
	displaceY = 0;
	hidesMeter = 0;
	soundClip = NULL;
	countersProjectile = true;
	next = NULL;
	attempt = NULL;
	riposte = NULL;
	basis = NULL;
	onHold = NULL;
	hittable = false;
	modifier = false;
	payload = NULL;
	spawnFrame = 0;
	spawnTrackY = 0;
	spawnTrackX = 0;
	spawnTrackFloor = 0;
	spawnPosY = 0;
	spawnPosX = 0;
	lifespan = -1;
	allegiance = 1;
	followStart = -1;
	followEnd = -1;
	followXRate = 0;
	followYRate = 0;
}

void action::generate(const char* directory, const char* name)
{
	payload = new projectile(directory, name);
	if(lifespan) payload->lifespan = lifespan;
	if(tempPayload) delete [] tempPayload;
	tempPayload = NULL;
}

instance * action::spawn()
{
	instance * ret = NULL;
	if(payload) ret = new instance(payload);
	return ret;
}

int action::arbitraryPoll(int q, int f)
{
	switch(q){
	case 2:
		if(f == freezeFrame) return freezeLength;
		else break;
	case 50:
		if(f == spawnFrame) return 1;
		else break;
	case 51:
		if(spawnTrackX) return 1;
		else break;
	case 52:
		if(spawnTrackY) return 1;
		else break;
	case 53:
		if(spawnTrackFloor) return 1;
		else break;
	case 54:
		return spawnPosX;
	case 55:
		return spawnPosY;
	case 56:
		return allegiance;
	}
	return 0;
}

void negNormal::zero()
{
	action::zero();
	minHold = 0;
	maxHold = 0;
}

void action::build(const char * n)
{
	zero();
	ifstream read;
	char fname[40];
	char buffer[100];
	char savedBuffer[100];
	buffer[0] = '\0';

	sprintf(fname, "content/characters/%s.mv", n);
	read.open(fname);
	assert(!read.fail());

	do {
		read.getline(buffer, 100);
		strcpy(savedBuffer, buffer);
	} while (setParameter(buffer));

	parseProperties(savedBuffer, 0);

	int currHit = 0;

	for(int i = 0; i < frames; i++){
		while(read.get() != '$'); read.ignore(2);
		SDL_Rect co;
		read >> co.x >> co.y >> co.w >> co.h;
		collision.push_back(co);
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		hitreg.push_back(aux::defineRectArray(buffer));
		while(read.get() != '$'); read.ignore(2);
		read.get(buffer, 100, '\n');
		delta.push_back(aux::defineRectArray(buffer));
		if(hits > 0 && currHit < hits){
			if(i > totalStartup[currHit] && i <= totalStartup[currHit]+active[currHit]){
				while(read.get() != '$'); read.ignore(2);
				read.get(buffer, 100, '\n');
				hitbox.push_back(aux::defineRectArray(buffer));
				if(i == totalStartup[currHit]+active[currHit]) currHit++;
			} else {
				std::vector<SDL_Rect> hi;
				hitbox.push_back(hi);
			}
		} else {
			std::vector<SDL_Rect> hi;
			hitbox.push_back(hi);
		}
	}
	read.close();
	unsigned int b = SDL_WasInit(SDL_INIT_VIDEO);
	if(b != 0)
		loadMisc(n); 
}

void action::loadMisc(const char *n)
{
	char fname[40];
	SDL_Surface *temp;
	for(int i = 0; i < frames; i++){
		sprintf(fname, "content/characters/%s#%i.png", n, i);
		temp = aux::load_image(fname);
		if(!temp){
			width.push_back(0);
			height.push_back(0);
			sprite.push_back(0);
		} else {
			width.push_back(temp->w);
			height.push_back(temp->h);
			sprite.push_back(aux::surface_to_texture(temp));
		}
		SDL_FreeSurface(temp);
	}
	sprintf(fname, "content/characters/%s.ogg", n);
	soundClip = Mix_LoadWAV(fname);
}

bool action::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);
	char* token = strtok(buffer, "\t:+ \n");

	if(!strcmp("Name", token)){
		token = strtok(NULL, "\t:\n");
		name += token;
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
	} else if (!strcmp("Offset", token)) {
		token = strtok(NULL, "\t: \n");
		offX = atoi(token);
		token = strtok(NULL, "\t: \n");
		offY = atoi(token);
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
			stats = std::vector<hStat> (hits);
			CHStats = std::vector<hStat> (hits);
			onConnect = std::vector <action*> (hits);
			tempOnConnect = std::vector <char*> (hits);
			for(action* i:onConnect) i = NULL;
			for(char* i:tempOnConnect) i = NULL;
			for(unsigned int i = 0; i < stats.size(); i++) stats[i].hitState.i = 0;
			for(unsigned int i = 0; i < CHStats.size(); i++) CHStats[i].hitState.i = 0;
		}
		state = std::vector<cancelField> (hits+1);
		gain = std::vector<int> (hits+1);
		for(unsigned int i = 0; i < state.size(); i++) state[i].i = 0;
		for(int i:gain) i = 0;
		return 1;
	} else if (!strcmp("Riposte", token)) {
		token = strtok(NULL, "\t: \n");
		tempRiposte = new char[strlen(token)+1];
		strcpy(tempRiposte, token);
		return 1;
	} else if (!strcmp("OnHold", token)) {
		token = strtok(NULL, "\t:- \n");
		holdFrame = atoi(token);
		token = strtok(NULL, "\t:- \n");
		holdCheck = 0;
		for(unsigned int i = 0; i < strlen(token); i++){
			switch(token[i]){
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
				holdCheck += 1 << (token[i] - 'A');
				break;
			}
		}
		token = strtok(NULL, "\t:- \n");
		tempOnHold = new char[strlen(token)+1];
		strcpy(tempOnHold, token);
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
		return 1;
	} else if (!strcmp("EventHorizon", token)) {
		token = strtok(NULL, "\t: \n");
		distortion->eventHorizon = atoi(token);
		return 1;
	} else if (!strcmp("Attracts", token)) {
		token = strtok(NULL, "\t: \n");
		distortion->ID = atoi(token);
		token = strtok(NULL, "\t: \n");
		distortion->effectCode = atoi(token);
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
			totalStartup = std::vector<int> (hits);
			active = std::vector<int> (hits);
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
	} else if (!strcmp("Connects", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].connect = atoi(token);
			else stats[i].connect = atoi(token);
		}
		return 1;
	} else if (!strcmp("Chip", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			stats[i].chip = atoi(token);
		}
		return 1;
	} else if (!strcmp("Prorate", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].prorate = atof(token);
			else stats[i].prorate = atof(token);
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
	} else if (!strcmp("Pause", token)) {
		for(int i = 0; i < hits; i++){
			token = strtok(NULL, "\t: \n");
			if(savedBuffer[0] == '+')
				CHStats[i].pause = atoi(token);
			else {
				stats[i].pause = atoi(token);
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
	} else if (!strcmp("BearStun", token)) {
		token = strtok(NULL, "\t: \n-");
		stunMin = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		stunMax = atoi(token); 
		return 1;
	} else if (!strcmp("GuardType", token)) {
		token = strtok(NULL, "\t: \n");
		guardType = atoi(token);
		return 1;
	} else if (!strcmp("Follow", token)) {
		token = strtok(NULL, "\t: \n-");
		followStart = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		followEnd = atoi(token); 

		token = strtok(NULL, "\t: \n");
		followXRate = atoi(token);

		token = strtok(NULL, "\t: \n");
		followYRate = atoi(token);
		return 1;
	} else if (!strcmp("SuperFreeze", token)){
		token = strtok(NULL, "\t: \n-");
		freezeFrame = atoi(token); 

		token = strtok(NULL, "\t: \n-");
		freezeLength = atoi(token); 
		freezeLength = freezeLength - freezeFrame;
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
	} else if(!strcmp("SpawnPosition", token)){
		token = strtok(NULL, "\t: \n");
		spawnPosX = atoi(token);

		token = strtok(NULL, "\t: \n");
		spawnPosY = atoi(token);
		return 1;
	} else if(!strcmp("Track", token)){
		token = strtok(NULL, "\t: \n");
		for(unsigned int i = 0; i < strlen(token) + 1; i++){
			switch(token[i]){
			case 'x': 
				spawnTrackX = true;
				break;
			case 'y':
				spawnTrackY = true;
				break;
			case 'f':
				spawnTrackFloor = true;
				break;
			}
		}
		return 1;
	} else if(!strcmp("SpawnsOn", token)){
		token = strtok(NULL, "\t: \n");
		spawnFrame = atoi(token);
		return 1;
	} else if(!strcmp("Lifespan", token)){
		token = strtok(NULL, "\t: \n");
		lifespan = atoi(token);
		return 1;
	} else if(!strcmp("Allegiance", token)){
		token = strtok(NULL, "\t: \n");
		allegiance = atoi(token);
		return 1;
	} else if(!strcmp("Payload", token)){
		token = strtok(NULL, "\t: \n");
		tempPayload = new char[strlen(token)+1];
		strcpy(tempPayload, token);
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
			if(!counter) stop += 1;
			break;
		case 'S': 
			if(!counter) stop += 2;
			break;
		case '*':
			if(!counter) stop += 4;
			break;
		case 'c':
			if(!counter) crouch = 1;
			break;
		case ':':
			ch++;
			break;
		case 'H':
			if(!counter) hittable = 1;
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
		case 'r':
			if(!counter) stats[ch].turnsProjectile = 1;
			break;
		case 'k':
			if(!counter) stats[ch].killsProjectile = 1;
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
		case 'm':
			if(!counter) modifier = 1;
			break;
		case 'l':
			if(!counter) linkable = 1;
			break;
		case 'f':
			if(!counter) track = 1;
			break;
		case 'i':
			if(!counter) countersProjectile = false;
			break;
		default:
			break;
		}
	}
}

bool action::window(int f)
{
	if(modifier && basis){
		if(basis->window(currentFrame)) return 1;
	}
	if(!attempt) return 0;
	if(f < attemptStart) return 0;
	if(f > attemptEnd) return 0;
	return 1;
}

bool action::activate(std::vector<int> inputs, int pattern, int t, int f, std::vector<int> meter, SDL_Rect &p)
{
	for(unsigned int i = 0; i < inputs.size(); i++){
		if(pattern & (1 << i)){
			if(inputs[i] < minHold) return 0;
			if(maxHold && inputs[i] > maxHold) return 0;
		}
	}
	if(t > tolerance) return 0;
	if(f > activation) return 0;
	return check(p, meter);
}

bool action::check(SDL_Rect &p, std::vector<int> meter)
{
	if(cost > meter[1]) return 0;
	if(xRequisite > 0 && p.w > xRequisite) return 0;
	if(yRequisite > 0 && p.h > yRequisite) return 0;
	return 1;
}

void action::pollRects(int f, int cFlag, SDL_Rect &c, std::vector<SDL_Rect> &r, std::vector<SDL_Rect> &b)
{
	if(modifier && basis) basis->pollRects(currentFrame, connectFlag, c, r, b);
	else {
		if(f >= frames) f = frames-1;

		c.x = collision[f].x; c.w = collision[f].w;
		c.y = collision[f].y; c.h = collision[f].h;

		r.clear();
		for(unsigned int i = 0; i < hitreg[f].size(); i++){
			SDL_Rect reg;
			reg.x = hitreg[f][i].x; reg.w = hitreg[f][i].w;
			reg.y = hitreg[f][i].y; reg.h = hitreg[f][i].h;
			r.push_back(reg);
		}
		b.clear();
		for(unsigned int i = 0; i < hitbox[f].size(); i++){
			if(cFlag > calcCurrentHit(f)) {
				i = hitbox[f].size();
			} else {
				SDL_Rect hit;
				hit.x = hitbox[f][i].x; hit.w = hitbox[f][i].w;
				hit.y = hitbox[f][i].y; hit.h = hitbox[f][i].h;
				b.push_back(hit);
			}
		}
	}
}

std::vector<SDL_Rect> action::pollDelta(int f)
{
	if(modifier && basis){ 
		std::vector<SDL_Rect> ret = basis->pollDelta(currentFrame);
		for(SDL_Rect i:delta[f]) ret.push_back(i);
		return ret;
	} else return delta[f];
}

int action::displace(int x, int &y, int f)
{
	int dx = 0;
	if(modifier && basis) dx += basis->displace(x, y, currentFrame);
	if(f == displaceFrame){
		y += displaceY;
		dx += displaceX;
	}
	return dx;
}

void action::pollStats(hStat & s, int f, bool CH)
{
	if(modifier && basis) basis->pollStats(s, currentFrame, CH);
	else{
		int c = calcCurrentHit(f);
		s.damage = stats[c].damage + CHStats[c].damage * CH;
		s.chip = stats[c].chip;
		s.stun = stats[c].stun + CHStats[c].stun * CH;
		s.push = stats[c].push + CHStats[c].push * CH;
		s.lift = stats[c].lift + CHStats[c].lift * CH;
		s.untech = stats[c].untech + CHStats[c].untech * CH;
		s.blowback = stats[c].blowback + CHStats[c].blowback * CH;
		s.pause = stats[c].pause + CHStats[c].pause * CH;
		s.connect = stats[c].connect + CHStats[c].connect * CH;
		if(CH){
			s.launch = CHStats[c].launch || stats[c].launch;
			s.hover = CHStats[c].hover;
			s.wallBounce = CHStats[c].wallBounce;
			s.floorBounce = CHStats[c].floorBounce;
			s.slide = CHStats[c].slide;
			s.stick = CHStats[c].stick;
			s.ghostHit = CHStats[c].ghostHit;
			s.prorate = CHStats[c].prorate;
		} else {
			s.launch = stats[c].launch;
			s.hover = stats[c].hover;
			s.wallBounce = stats[c].wallBounce;
			s.floorBounce = stats[c].floorBounce;
			s.slide = stats[c].slide;
			s.stick = stats[c].stick;
			s.ghostHit = stats[c].ghostHit;
			s.prorate = stats[c].prorate;
		}
		s.hitsProjectile = stats[c].hitsProjectile;
		s.turnsProjectile = stats[c].turnsProjectile;
		s.killsProjectile = stats[c].killsProjectile;
		s.blockMask.i = stats[c].blockMask.i;
	}
}

bool action::cancel(action *& x, int& c, int &h)
{
	cancelField r;
	r.i = 0;
	if(x == NULL) return 1;
	if(c > x->hits || h > x->hits) return 0;
	if(x->modifier && x->basis){
		if(x->basis == NULL){ 
			return 1;
		}
		r.i = x->basis->state[x->connectFlag].i;
		if(x->hitFlag > 0 && x->hitFlag == x->connectFlag){ 
			r.i = r.i + x->basis->stats[x->hitFlag - 1].hitState.i;
		}
		x = basis;
	} else {
		r.i = x->state[c].i;
		if(h > 0 && h == c){
			r.i = r.i + x->stats[h - 1].hitState.i;
		}
	}
	if(allowed.i & r.i){
		if(x == this){
			if(c == 0) return 0;
			else if(allowed.b.chain1) return 1;
			else return 0;
		} else {
			return 1;
		}
	}
	return 0;
}

void action::step(std::vector<int>& meter, status &current)
{
	if(!current.frame && !meter[4]){
		if(meter[1] + gain[0] < 300) meter[1] += gain[0];
		else meter[1] = 300;
	}
	current.frame++;
	if(modifier && basis){
		currentFrame++;
		if(basis && currentFrame >= basis->frames){
			if(basis->next) basis = basis->next;
			else basis = NULL;
			currentFrame = 0;
			connectFlag = 0;
			hitFlag = 0;
		}
	}
}

int action::calcCurrentHit(int frame)
{
	int b = 0;
	for(int i = 0; i < hits; i++){
		if(frame > totalStartup[i]) b = i;
	}
	return b;
}

action * action::connect(std::vector<int>& meter, int &c, int f)
{
	if(modifier && basis) return basis->connect(meter, connectFlag, currentFrame);
	else if (hits == 0) return NULL;
	else {
		c = calcCurrentHit(f)+1;
		if(!meter[4]){
			if(meter[1] + gain[c] < 300) meter[1] += gain[c];
			else meter[1] = 300;
		}
		if(onConnect[c-1] != NULL){
			return onConnect[c-1];
		} else return NULL;
	}
}

action * action::blockSuccess(int n, bool p)
{
	if(modifier && basis) return basis->blockSuccess(n, p);
	if(riposte){
		if(!p || countersProjectile) return riposte;
	}
	return this;
}

void action::playSound(int channel)
{
	Mix_PlayChannel(channel, soundClip, 0);
}

void action::execute(action * last, std::vector<int> & meter, int &f, int &c, int &h)
{
	armorCounter = 0;
	meter[1] -= cost;
	meter[4] += cost;
	if(modifier){
		if(last == NULL) basis = NULL;
		basis = last;
		currentFrame = f;
		connectFlag = c;
		hitFlag = h;
	}
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
	case 6:
		onHold = c;
		if(tempOnHold) delete [] tempOnHold;
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
	case 4:
		return tempPayload;
	case 5:
		return tempRiposte;
	case 6:
		return tempOnHold;
	default:
		return NULL;
	}
}

int action::takeHit(hStat & s, int b, status &current)
{
	if(modifier && basis) return basis->takeHit(s, b, current);
	else{
		if(!stunMin || s.stun >= stunMin){
			if(!stunMax || s.stun <= stunMax){
				if(s.blockMask.i & blockState.i && current.frame >= guardStart && current.frame <= guardStart + guardLength){
					if(riposte != NULL){ 
						if(!s.isProjectile || countersProjectile) return -5;
					}
					return guardType;
				} else if (current.frame >= armorStart && current.frame <= armorStart + armorLength && 
						   (armorHits < 1 || armorHits < armorCounter)){
					s.stun = 0;
					armorCounter++;
					return 1;
				}
			}
		}
		if(s.stun != 0){
			current.frame = 0;
			current.frame = 0;
			current.hit = 0;
		}
		return 1;
	}
}

bool action::CHState(int f)
{
	if(modifier && basis) return basis->CHState(currentFrame);
	if(hits < 1) return false;
	else if(f < totalStartup[hits-1] + active[hits-1]) return true;
	else return fch;
}


#include <stdio.h>
#include <iostream>
#include <fstream>
#include "action.h"
#include "player.h"
#include <assert.h>
#include "tokenizer.h"

using namespace std;
action::action() {}

action::action(std::string dir, std::string file)
{
	build(dir, file);
}

action::~action()
{
	if(!this) return;
	if(distortion) delete distortion;
	if(next) delete next;
}

void action::zero()
{
	typeKey = '0';
	frames = 0;
	hits = 0;
	collision.clear();
	hitbox.clear();
	hitreg.clear();
	delta.clear();
	tempNext.clear();
	tempOnConnect.clear();
	tempAttempt.clear();
	tempPayload.clear();
	tempRiposte.clear();
	tempOnHold.clear();
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
	distortion = nullptr;
	displaceFrame = -1;
	displaceX = 0;
	displaceY = 0;
	hidesMeter = 0;
	soundClip = nullptr;
	countersProjectile = true;
	next = nullptr;
	attempt = nullptr;
	riposte = nullptr;
	basis = nullptr;
	onHold = nullptr;
	hittable = false;
	modifier = false;
	payload = nullptr;
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
}

instance * action::spawn()
{
	instance * ret = nullptr;
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

void action::build(std::string dir, std::string n)
{
	zero();
	fileName = n;
	ifstream read;
	char fname[1024];
	char buffer[1024];
	char savedBuffer[1024];
	buffer[0] = '\0';

	sprintf(fname, "content/characters/%s/%s.mv", dir.c_str(), fileName.c_str());
	read.open(fname);
	if(read.fail()){
		//printf("Move %s/%s not found. Skipping\n", dir.c_str(), fileName.c_str());
		null = true;
		return;
	} else null = false;

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
}

void action::loadMisc(std::string dir)
{
	char fname[1024];
	SDL_Surface *temp;
	for(int i = 0; i < frames; i++){
		sprintf(fname, "content/characters/%s/%s#%i.png", dir.c_str(), fileName.c_str(), i);
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
	sprintf(fname, "content/characters/%s/%s.ogg", dir.c_str(), fileName.c_str());
	soundClip = Mix_LoadWAV(fname);
}

bool action::setParameter(string buffer)
{
	tokenizer t(buffer, "\t:+\n");
	if(t() == "Name"){;
		name += t();
		return true;
	} else if (t.current() == "Displace") {
		displaceFrame = stoi(t("\t:\n"));
		displaceX = stoi(t());
		displaceY = stoi(t());
		return true;
	} else if (t.current() == "Buffer") {
		tolerance = stoi(t("\t: \n"));
		activation = stoi(t());
		return true;
	} else if(t.current() == "Proximity"){
		xRequisite = stoi(t("\t: \n"));
		yRequisite = stoi(t());
		return true;
	} else if (t.current() == "Offset") {
		offX = stoi(t("\t: \n"));
		offY = stoi(t());
		return true;
	} else if (t.current() == "Hold") {
		minHold = stoi(t("\t: \n-"));
		maxHold = stoi(t());
		return true;
	} else if (t.current() == "Counterhit") {
		parseProperties(buffer, true);
		return true;
	} else if (t.current() == "Hits") {
		hits = stoi(t("\t: \n"));
		if(hits > 0){
			stats = std::vector<hStat> (hits);
			CHStats = std::vector<hStat> (hits);
			onConnect = std::vector <action*> (hits);
			tempOnConnect = std::vector <std::string> (hits);
			for(action* i:onConnect) i = nullptr;
			for(unsigned int i = 0; i < stats.size(); i++) stats[i].hitState.i = 0;
			for(unsigned int i = 0; i < CHStats.size(); i++) CHStats[i].hitState.i = 0;
		}
		state = std::vector<cancelField> (hits+1);
		gain = std::vector<int> (hits+1);
		for(unsigned int i = 0; i < state.size(); i++) state[i].i = 0;
		for(int i:gain) i = 0;
		return true;
	} else if (t.current() == "Riposte") {
		tempRiposte = t("\t: \n");
		return true;
	} else if (t.current() == "OnHold") {
		holdFrame = stoi(t("\t:- \n"));
		holdCheck = 0;
		for(char c : t()){
			switch(c){
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
				holdCheck += 1 << (c - 'A');
				break;
			}
		}
		tempOnHold = t();
		return true;
	} else if (t.current() == "Distort") {
		distortion = new attractor;
		distortSpawn = stoi(t("\t:- \n"));
		distortion->length = stoi(t());
		distortion->length -= distortSpawn;
		distortion->x = stoi(t("\t: \n"));
		distortion->y = stoi(t());
		return true;
	} else if (t.current() == "AttractorType") {
		if(!distortion) return true;
		distortion->type = stoi(t("\t: \n"));
		distortion->radius = stoi(t());
		return true;
	} else if (t.current() == "EventHorizon") {
		distortion->eventHorizon = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Attracts") {
		distortion->ID = stoi(t("\t: \n"));
		distortion->effectCode = stoi(t());
		return true;
	} else if (t.current() == "Next") {
		tempNext = t("\t: \n");
		return true;
	} else if (t.current() == "Attempt") {
		attemptStart = stoi(t("\t: \n-")); 
		attemptEnd = stoi(t()); 
		tempAttempt = t();
		return true;
	} else if (t.current() == "Connect") {
		int x = stoi(t("\t: \n"));
		tempOnConnect[x] = t();
		return true;
	} else if (t.current() == "Blocks") {
		blockState.i = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Check") {
		allowed.i = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Cost") {
		cost = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Frames") {
		frames = stoi(t("\t: \n"));
		int startup, countFrames = -1;
		if(hits > 0) {
			totalStartup = std::vector<int> (hits);
			active = std::vector<int> (hits);
		}
		for(int i = 0; i < hits; i++){
			startup = stoi(t());
			countFrames += startup;
			totalStartup[i] = countFrames;
			active[i] = stoi(t());
			countFrames += active[i];
		}
		return true;
	} else if (t.current() == "State") {
		for(int i = 0; i < hits+1; i++){
			state[i].i = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "HitAllows") {
		for(int i = 0; i < hits; i++){
			stats[i].hitState.i = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Damage") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].damage = stoi(t("\t: \n"));
			else stats[i].damage = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Connects") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].connect = stoi(t("\t: \n"));
			else stats[i].connect = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Chip") {
		for(int i = 0; i < hits; i++){
			stats[i].chip = stoi(t( "\t: \n"));
		}
		return true;
	} else if (t.current() == "Prorate") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].prorate = stof(t("\t: \n"));
			else stats[i].prorate = stof(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Push") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+') 
				CHStats[i].push = stoi(t("\t: \n"));
			else stats[i].push = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Lift") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].lift = stoi(t("\t: \n"));
			else stats[i].lift = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Float") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].hover = stoi(t("\t: \n"));
			else stats[i].hover = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Blowback") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].blowback = stoi(t("\t: \n"));
			else stats[i].blowback = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Stun") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].stun = stoi(t("\t: \n"));
			else {
				stats[i].stun = stoi(t("\t: \n"));
				CHStats[i].stun = (stats[i].stun - 5) / 2;
			}
		}
		return true;
	} else if (t.current() == "Pause") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].pause = stoi(t("\t: \n"));
			else {
				stats[i].pause = stoi(t("\t: \n"));
			}
		}
		return true;
	} else if (t.current() == "Untech") {
		for(int i = 0; i < hits; i++){
			if(buffer[0] == '+')
				CHStats[i].untech = stoi(t("\t: \n"));
			else{
				stats[i].untech = stoi(t("\t: \n"));
				CHStats[i].untech = 10;
			}
		}
		return true;
	} else if (t.current() == "Blockable") {
		for(int i = 0; i < hits; i++){
			stats[i].blockMask.i = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Gain") {
		for(int i = 0; i < hits+1; i++){
			gain[i] = stoi(t("\t: \n"));
		}
		return true;
	} else if (t.current() == "Autoguard") {
		guardStart = stoi(t("\t: \n-"));
		guardLength = stoi(t());
		guardLength = guardLength - guardStart;
		return true;
	} else if (t.current() == "BearStun") { //RARRRRRR
		stunMin = stoi(t("\t: \n-"));
		stunMax = stoi(t());
		return true;
	} else if (t.current() == "GuardType") {
		guardType = stoi(t("\t: \n"));
		return true;
	} else if (t.current() == "Follow") {
		followStart = stoi(t("\t: \n-"));
		followEnd = stoi(t()); 
		followXRate = stoi(t());
		followYRate = stoi(t());
		return true;
	} else if (t.current() == "SuperFreeze"){
		freezeFrame = stoi(t("\t: \n-")); 
		freezeLength = stoi(t()); 
		freezeLength = freezeLength - freezeFrame;
		return true;
	} else if (t.current() == "Armor") {
		armorStart = stoi(t("\t: \n-")); 
		armorLength = stoi(t()); 
		armorLength = armorLength - armorStart;
		return true;
	} else if (t.current() == "MaxArmor") {
		armorHits = stoi(t("\t: \n-")); 
		return true;
	} else if(t.current() == "SpawnPosition"){
		spawnPosX = stoi(t("\t: \n"));
		spawnPosY = stoi(t());
		return true;
	} else if(t.current() == "Track"){
		for(char c : t("\t: \n")){
			switch(c){
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
		return true;
	} else if(t.current() == "SpawnsOn"){
		spawnFrame = stoi(t("\t: \n"));
		return true;
	} else if(t.current() == "Lifespan"){
		lifespan = stoi(t("\t: \n"));
		return true;
	} else if(t.current() == "Allegiance"){
		allegiance = stoi(t("\t: \n"));
		return true;
	} else if(t.current() == "Payload"){
		tempPayload = t("\t: \n");
		return true;
	} else return 0;
}

void action::parseProperties(string buffer, bool counter)
{
	int ch = 0;
	unsigned int i = 0;
	while(buffer[i++] != ':'); i++;
	for(; i < buffer.size(); i++){
		switch(buffer[i]){
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
	if(cost && cost > meter[1]){ 
		return 0;
	}
	if(xRequisite > 0 && p.w > xRequisite) return 0;
	if(yRequisite > 0 && p.h > yRequisite) return 0;
	return 1;
}

bool special::check(SDL_Rect &p, std::vector<int> meter)
{
	if(cost > meter[1]) return 0;
	else return action::check(p, meter);
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
	if(x == nullptr) return 1;
	if(c > x->hits || h > x->hits) return 0;
	if(x->modifier && x->basis){
		if(x->basis == nullptr){ 
			return 1;
		}
		r.i = x->basis->state[x->connectFlag].i;
		if(x->hitFlag > 0 && x->hitFlag == x->connectFlag){ 
			r.i = r.i + x->basis->stats[x->hitFlag - 1].hitState.i;
		}
		x = basis;
	} else {
		if(c <= hits){ 
			r.i = x->state[c].i;
			if(h > 0 && h == c){
				r.i = r.i + x->stats[h - 1].hitState.i;
			}
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
			else basis = nullptr;
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
	else if (hits == 0) return nullptr;
	else {
		c = calcCurrentHit(f)+1;
		if(!meter[4]){
			if(meter[1] + gain[c] < 300) meter[1] += gain[c];
			else meter[1] = 300;
		}
		if(onConnect[c-1] != nullptr){
			return onConnect[c-1];
		} else return nullptr;
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
		if(last == nullptr) basis = nullptr;
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
		break;
	case 2:
		onConnect[i] = c;
		break;
	case 3:
		attempt = c;
		break;
	case 5:
		riposte = c;
		break;
	case 6:
		onHold = c;
		break;
	}
}

std::string action::request(int code, int i)
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
		return "";
	}
}

int action::takeHit(hStat & s, int b, status &current)
{
	if(modifier && basis) return basis->takeHit(s, b, current);
	else{
		if(!stunMin || s.stun >= stunMin){
			if(!stunMax || s.stun <= stunMax){
				if(s.blockMask.i & blockState.i && current.frame >= guardStart && current.frame <= guardStart + guardLength){
					if(riposte != nullptr){ 
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


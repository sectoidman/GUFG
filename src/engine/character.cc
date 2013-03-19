#include <stdio.h>
#include <cmath>
#include <fstream>
#include <assert.h>
#include <cstdlib>
#include "character.h"
#include <iostream>
#include <cstring>

character::character()
//Character constructor. This loads the whole character into memory so that that we don't have disk reads during gameplay
{
	//build("genericCharacter", "genericCharacter");
}

character::character(const char* name)
{
	head = new actionTrie;
	airHead = new actionTrie;
	build(name, name);
}

character::~character()
	//Character destructor. Might not need this if we aren't working with any dynamic memory, but it might be worthwhile to have.
{
	if(!dFlag){
		if(head) delete head;
		if(airHead) delete airHead;
		delete neutral;
		delete crouch;
		delete reel;
		delete untech;
		delete crouchReel;
		delete crouchBlock;
		delete standBlock;
		delete airBlock;
		delete down;
		if(name) delete [] name;
	}
}

void avatar::prepHooks(status &current, action *& cMove, int inputBuffer[30], std::vector<int> buttons, SDL_Rect &p, bool dryrun, std::vector<int> & meter)
{
	action * t = NULL;
	if(current.move && current.reversal){
		if(current.move->state[current.hit].i & current.reversal->allowed.i){
			if(current.reversal->check(p, meter)){
				if(!dryrun) current.reversal->execute(neutral, meter, current.frame, current.connect, current.hit);
				cMove = current.reversal;
				current.reversalFlag = true;
				if(!dryrun) current.reversal = NULL;
			}
		}
	}
	else if(!current.move) neutralize(current, cMove, meter);
	t = hook(inputBuffer, 0, -1, meter, buttons, cMove, p, current.connect, current.hit, current.aerial);
	if(t == NULL){
		if(cMove->window(current.frame)){
			if(cMove->attempt->check(p, meter)){
				t = cMove->attempt;
			}
		}
		else if(cMove->holdFrame == current.frame){
			if(cMove->onHold->activate(buttons, cMove->holdCheck, 0, 0, meter, p)){
				t = cMove->onHold;
			}
		}
		if (current.bufferedMove != NULL && current.freeze <= 0) {
			if(!dryrun){ 
				current.bufferedMove->execute(cMove, meter, current.frame, current.connect, current.hit);
			}
			cMove = current.bufferedMove;
			if(!dryrun) current.bufferedMove = NULL;
		} else {
			action * r;
			bool l = current.reversalFlag;
			neutralize(current, r, meter);
			current.reversalFlag = l;
			if (!current.reversal){
				if((current.frame + 10 > cMove->frames && current.frame > 5 && cMove != r) || (current.counter + current.freeze < -10 && current.counter < 0) || cMove->linkable) {
					int l = 0, m = 0;
					current.reversal = hook(inputBuffer, 0, -1, meter, buttons, r, p, l, m, current.aerial);
					if(current.reversal){
						if(current.reversal->state[0].b.neutral) 
							current.reversal = NULL;
					}
				}
			}
		}
	}
	if(t != NULL) {
		current.reversalFlag = false;
		if(current.freeze > 0){
			if(current.bufferedMove == NULL){ 
				if(!dryrun) current.bufferedMove = t;
			}
		} else {
			if(!dryrun) t->execute(cMove, meter, current.frame, current.connect, current.hit);
			cMove = t;
		}
	}
}

action * avatar::hook(int inputBuffer[30], int i, int f, std::vector<int> meter, std::vector<int> buttons, action * c, SDL_Rect &p, int &cFlag, int &hFlag, bool aerial)
{
	return head->actionHook(inputBuffer, 0, -1, meter, buttons, c, p, cFlag, hFlag);
}

action * character::hook(int inputBuffer[30], int i, int f, std::vector<int> meter, std::vector<int> buttons, action * c, SDL_Rect &p, int &cFlag, int &hFlag, bool aerial)
{
	if(aerial) return airHead->actionHook(inputBuffer, 0, -1, meter, buttons, c, p, cFlag, hFlag);
	else return avatar::hook(inputBuffer, 0, -1, meter, buttons, c, p, cFlag, hFlag, aerial);
}

action * avatar::moveSignal(int)
{
	return NULL;
}

void avatar::neutralize(status &current, action *& cMove, std::vector<int> & meter)
{
	cMove = neutral;
	current.reversalFlag = false;
}

void character::neutralize(status &current, action *& cMove, std::vector<int> & meter)
{
	if(current.aerial) cMove = airNeutral;
	else cMove = neutral;
	current.reversalFlag = false;
}

void avatar::getName(const char* directory, const char* file)
{
	char buffer[101];
	std::ifstream read;
	sprintf(buffer, "content/characters/%s/%s.ch", directory, file);

	read.open(buffer);
	assert(!read.fail());

	read.get(buffer, 50); read.ignore(100, '\n');
	name = new char[strlen(buffer)+1];
	strcpy(name, buffer);
}

int character::comboState(action * c)
{
	if(c == reel) return 1;
	if(c == crouchReel) return 1;
	if(c == untech) return 1;
	if(c == standBlock) return -1;
	if(c == crouchBlock) return -1;
	if(c == airBlock) return -1;
	if(c == fall) return -2;
	return 0;
}

void avatar::build(const char* directory, const char* file)
{
	char buffer[101];
	char buffer2[101];
	action * m = NULL;
	bool commentFlag;
	std::ifstream read;
	sprintf(buffer, "content/characters/%s/%s.ch", directory, file);

	read.open(buffer);
	assert(!read.fail());

	read.get(buffer, 50); read.ignore(100, '\n');

	while(!read.eof()){
		commentFlag = 0;
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		
		if(buffer[0] == '#' || buffer[0] == '\0')
			commentFlag = 1;
		if(!commentFlag){
			strcpy(buffer2, buffer);

			m = createMove(buffer);
			processMove(m);
			sortMove(m, buffer2);
		}
	}
	read.close();
	movesByName.clear();
}

void avatar::sortMove(action * m, char* buffer)
{
	char * token;
	token = strtok(buffer, " \t=~>-&?@%$_!\n");
	while (token){
		token = NULL;
		token = strtok(NULL, " \t=~>-&?@%$_!\n");
		if(token) {
			switch (token[0]){
			case 'h':
				head->insert(m, token);
				break;
			default:
				break;
			}
		}
	}
}

void character::sortMove(action * m, char* buffer)
{
	char * token;
	token = strtok(buffer, " \t=>-&?@%$_!\n");
	while (token){
		token = NULL;
		token = strtok(NULL, " \t=>-&?@%$_!\n");
		if(token) {
			switch (token[0]){
			case 'h':
				head->insert(m, token);
				break;
			case 'a':
				airHead->insert(m, token);
				break;
			default:
				break;
			}
		}
	}
}

void character::build(const char *directory, const char *file)
{
	getName(directory, file);

	char buffer[101];

	sprintf(buffer, "%s/NS", name);
	neutral = new looping(buffer);
	processMove(neutral);
	dFlag = 0;

	avatar::build(directory, file);

	sprintf(buffer, "%s/die", name);
	die = new untechState(buffer);

	sprintf(buffer, "%s/dead", name);
	dead = new looping(buffer);
	die->feed(dead, 0, 0);
	die->feed(dead, 1, 0);

	sprintf(buffer, "%s/NL", name);
	crouch = new looping(buffer);
	processMove(crouch);

	sprintf(buffer, "%s/NJ", name);
	airNeutral = new airLooping(buffer);
	airNeutral->feed(neutral, 1, 0);
	processMove(airNeutral);

	sprintf(buffer, "%s/HS", name);
	reel = new hitstun(buffer);
	processMove(reel);

	sprintf(buffer, "%s/Fall", name);
	fall = new airLooping(buffer);
	processMove(fall);

	sprintf(buffer, "%s/UT", name);
	untech = new untechState(buffer);
	untech->feed(fall, 0, 0);
	processMove(untech);

	sprintf(buffer, "%s/down", name);
	down = new utility(buffer);
	untech->feed(down, 1, 0);
	fall->feed(down, 1, 0);
	processMove(fall);

	sprintf(buffer, "%s/HL", name);
	crouchReel = new hitstun(buffer);
	processMove(crouchReel);

	sprintf(buffer, "%s/BH", name);
	standBlock = new hitstun(buffer);

	sprintf(buffer, "%s/BL", name);
	crouchBlock = new hitstun(buffer);

	sprintf(buffer, "%s/BA", name);
	airBlock = new hitstun(buffer);	

	sprintf(buffer, "%s/break", name);
	throwBreak = new utility(buffer);
	processMove(throwBreak);

	head->insert(5, neutral);

	head->insert(2, crouch);
	head->insert(3, crouch);
	head->insert(1, crouch);

	airHead->insert(5, airNeutral);
}

void avatar::processMove(action * m)
{
	char* temp = NULL;
	action* t = NULL;
	for(int i = 0; i < 7; i++){
		if(i == 2){
			for(int j = 0; j < m->hits; j++){
				temp = m->request(i, j);
				if(temp != NULL){ 
					t = createMove(temp);
					m->feed(t, i, j);
					processMove(t);
				}
			}
		} else if (i == 4) {
			temp = m->request(i, 0);
			if(temp != NULL){
				m->generate(name, temp);
			}
		} else {
			temp = m->request(i, 0);
			if(temp != NULL){
				t = createMove(temp);
				m->feed(t, i, 0);
				processMove(t);
			}
		}
	}
}

action * avatar::createMove(char * fullName)
{
	char * token;
	char type[2] = {fullName[0], fullName[1]};
	char actionName[151];

	token = strtok(fullName, " \t=~>-&?@%$_!^\n");
	sprintf(actionName, "%s/%s", name, token);

	action * m;
	switch(type[0]){
	case '%':
		if(type[1] == 'j') m = new airSpecial(actionName);
		else m = new special(actionName);
		break;
	case '~':
		if(type[1] == 'j') m = new airNegNormal(actionName);
		else m = new negNormal(actionName);
		break;
	case '-':
		if(type[1] == 'j') m = new airUtility(actionName);
		else m = new utility(actionName);
		break;
	case '@':
		if(type[1] == 'j') m = new airLooping(actionName);
		else m = new looping(actionName);
		break;
	case '^':
		m = new releaseCheck(actionName);
		break;
	case '?':
		m = new mash(actionName);
		break;
	case '_':
		if(type[1] == 'j') m = new luftigeWerf(actionName);
		else m = new werf(actionName);
		break;
	case 'j':
		m = new airMove(actionName);
		break;
	default:
		m = new action(actionName);
		break;
	}
	return m;
}

instance * avatar::spawn(action * source)
{
	return source->spawn();
}

void avatar::connect(status &current, std::vector<int>& meter)
{
	action * t = current.move->connect(meter, current.connect, current.frame);
	if(t != NULL){
		current.bufferedMove = t;
	}
}

int character::checkBlocking(status &current, int input[])
{
	int st;
	bool success = false;
	int ret = -1;
	st = -(current.counter);
	if(st <= 0) st = 1;
	switch(input[0]){
	case 3:
	case 6:
	case 9:
		for(int i = 1; i < 5; i++){
			if(input[i] % 3 == 2){
				ret = 2;
				block(current, st, (input[0] > 3));
				break;
			}
		}
		break;
	case 7:
	case 4:
	case 1:
		block(current, st, (input[0] > 3));
		success = true;
		break;
	}
	if(success){
		ret = 0;
		for(int i = 1; i < 7; i++){
			if(input[i] % 3 != 1)
				ret = 1;
		}
	}
	return ret;
}

void character::block(status &current, int st, bool high)
{
	current.frame = 0;
	current.hit = 0;
	current.connect = 0;
	current.counter = -st;
	if(current.aerial){
		if(airBlock->cancel(current.move, current.connect, current.hit)) {
			current.move = airBlock;
		}
	} else {
		if(high){
			if(standBlock->cancel(current.move, current.connect, current.hit)) {
				current.move = standBlock;
			}
		} else {
			if(crouchBlock->cancel(current.move, current.connect, current.hit)) {
				current.move = crouchBlock;
			}
		}
	}
}

void avatar::pollStats(hStat & s, status &current)
{
	s.isProjectile = false;
	current.move->pollStats(s, current.frame, current.counter);
}

int character::takeHit(status &current, hStat & s, int blockType, int &hitType, std::vector<int>& meter)
{
	bool dead = false;
	int freeze = 0;
	if(s.pause < 0){
		freeze = s.stun/4 + 10;
		if(s.ghostHit) freeze = 0;
	} else freeze = s.pause;
	hitType = current.move->takeHit(s, blockType, current);
	if(hitType == 1) meter[0] -= s.damage;
	else if(hitType > -2) {
		meter[0] -= s.chip;
		if(hitType == -1 && meter[0] <= 0){ 
			meter[0] = 1;
		}
	}
	if(meter[0] <= 0){
		meter[0] = 0;
		dead = true;
	}
	if(dead == true){
		current.counter = -(s.stun+s.untech);
		die->execute(current.move, meter, current.frame, current.connect, current.hit);
		current.move = die;
		current.aerial = true;
	} else if (hitType == 1){
		if(s.launch) current.aerial = true;
		if(s.stun != 0){
			current.frame = 0;
			if(current.aerial){
				current.counter = -(s.stun+s.untech);
				current.move = untech;
				resetAirOptions(meter);
			} else if(current.move->crouch){
				current.counter = -(s.stun + s.stun/5);
				current.move = crouchReel;
			} else {
				current.counter = -(s.stun);
				current.move = reel;
			}
		}
	} else if (hitType == -1) {
		if(!meter[4]){
			if(meter[1] + 6 < 300) meter[1] += 12;
			else meter[1] = 300;
		}
	}

	if (hitType == 1){ 
		if(!meter[4]){
			meter[1] += 2;
		}
	} else if (hitType > -2) {
		if(!meter[4]){
			if(meter[1] + 1 < 300) meter[1] += 3;
			else meter[1] = 300;
		}
	}
	return freeze;
}

std::vector<int> avatar::generateMeter()
{
	std::vector<int> meter(5);
	return meter;
}

void character::init(std::vector<int>& meter)
{
	meter[0] = 600;
	meter[1] = 0;
	resetAirOptions(meter);
	meter[4] = 0;
}

void character::resetAirOptions(std::vector<int>& meter)
{
	meter[2] = 1;
	meter[3] = 1;
}

int avatar::acceptTarget(action * c, int f)
{
	return 1;
}

void character::land(action *& cMove, int &frame, int &connectFlag, int &hitFlag, std::vector<int> & meter)
{
	if(cMove->allowed.b.block){
		cMove = standBlock;
	} else { 
		cMove = cMove->land(frame, connectFlag, hitFlag);
		if(!cMove) cMove = neutral;
	}
	resetAirOptions(meter);
}

void avatar::step(status &current, std::vector<int> & meter)
{
	int a = 0;
	if(current.move->hits > 0){
		a += current.move->stats[current.connect-1].connect;
		if(current.counter > 0)
			a += current.move->CHStats[current.connect-1].connect;
	}
	if(current.freeze <= 0){
		current.move->step(meter, current);
		tick(meter);
		if(a < 0) current.connect += a;
		if(current.counter > 0) current.counter = 0;
	} else {
		current.freeze--;
		if(!current.freeze && a > 0){
			current.connect += a;
			current.hit += a;
		}
	}
	if(meter[4] > 0) meter[4]--;
}

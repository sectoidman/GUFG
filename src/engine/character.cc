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

void avatar::prepHooks(int freeze, action *& cMove, action *& bMove, action *& sMove, int inputBuffer[30], std::vector<int> down, std::vector<bool> up, SDL_Rect &p, int &f, int &cFlag, int &hFlag, bool dryrun, bool aerial, int *& meter)
{
	action * t = NULL;
	if (cMove == NULL){
		if(sMove){
			if(sMove->check(p, meter)){
				if(!dryrun) sMove->execute(neutral, meter, f, cFlag, hFlag);
				cMove = sMove;
				if(!dryrun) sMove = NULL;
			}
		}
		else neutralize(cMove, aerial, meter);
	}
	t = hook(inputBuffer, 0, -1, meter, down, up, cMove, p, cFlag, hFlag, aerial);
	if(t == NULL){
		if(cMove->window(f)){
			if(cMove->attempt->check(p, meter)){
				t = cMove->attempt;
			}
		}
		else if(cMove->holdFrame == f){
			if(cMove->onHold->activate(down, up, cMove->holdCheck, 0, 0, meter, p)){
				t = cMove->onHold;
			}
		}
		if (bMove != NULL && freeze <= 0) {
			if(!dryrun){ 
				bMove->execute(cMove, meter, f, cFlag, hFlag);
			}
			cMove = bMove;
			if(!dryrun) bMove = NULL;
		} else {
			action * r;
			neutralize(r, aerial, meter);
			if (!sMove && f + 4 > cMove->frames && cMove->frames > 10 && cMove != r) {
				int l = 0, m = 0;
				sMove = hook(inputBuffer, 0, -1, meter, down, up, r, p, l, m, aerial);
				if(r == sMove || cMove == sMove) sMove = NULL;
			}
		}
	}
	if(t != NULL) {
		if(freeze > 0){
			if(bMove == NULL){ 
				if(!dryrun) bMove = t;
			}
		} else {
			if(!dryrun) t->execute(cMove, meter, f, cFlag, hFlag);
			cMove = t;
		}
	}
}

action * avatar::hook(int inputBuffer[30], int i, int f, int * meter, std::vector<int> down, std::vector<bool> up, action * c, SDL_Rect &p, int &cFlag, int &hFlag, bool aerial)
{
	return head->actionHook(inputBuffer, 0, -1, meter, down, up, c, p, cFlag, hFlag);
}

action * avatar::moveSignal(int)
{
	return NULL;
}

action * character::hook(int inputBuffer[30], int i, int f, int * meter, std::vector<int> down, std::vector<bool> up, action * c, SDL_Rect &p, int &cFlag, int &hFlag, bool aerial)
{
	if(aerial) return airHead->actionHook(inputBuffer, 0, -1, meter, down, up, c, p, cFlag, hFlag);
	else return avatar::hook(inputBuffer, 0, -1, meter, down, up, c, p, cFlag, hFlag, aerial);
}

void avatar::neutralize(action *& cMove, bool aerial, int *& meter)
{
	cMove = neutral;
}

void character::neutralize(action *& cMove, bool aerial, int *& meter)
{
	if(aerial) cMove = airNeutral;
	else cMove = neutral;
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

	sprintf(buffer, "%s/NJ", name);
	airNeutral = new airLooping(buffer);
	airNeutral->feed(neutral, 1, 0);

	sprintf(buffer, "%s/HS", name);
	reel = new hitstun(buffer);

	sprintf(buffer, "%s/Fall", name);
	fall = new airLooping(buffer);

	sprintf(buffer, "%s/UT", name);
	untech = new untechState(buffer);
	untech->feed(fall, 0, 0);

	sprintf(buffer, "%s/down", name);
	down = new utility(buffer);
	untech->feed(down, 1, 0);
	fall->feed(down, 1, 0);

	sprintf(buffer, "%s/HL", name);
	crouchReel = new hitstun(buffer);

	sprintf(buffer, "%s/BH", name);
	standBlock = new hitstun(buffer);

	sprintf(buffer, "%s/BL", name);
	crouchBlock = new hitstun(buffer);

	sprintf(buffer, "%s/BA", name);
	airBlock = new hitstun(buffer);	

	sprintf(buffer, "%s/break", name);
	throwBreak = new utility(buffer);

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

void avatar::connect(action *& cMove, action *& bMove, action *& sMove, hStat & s, int & connectFlag, int frame, int *& meter)
{
	action * t = cMove->connect(meter, connectFlag, frame);
	if(t != NULL){
		bMove = t;
	}
}

int character::checkBlocking(action *& cMove, int input[], int &connectFlag, int &hitFlag, bool aerial)
{
	int st;
	bool success = false;
	int ret = -1;
	st = cMove->arbitraryPoll(1, 0);
	if(st == 0) st = 1;
	switch(input[0]){
	case 3:
	case 6:
	case 9:
		for(int i = 1; i < 7; i++){
			if(input[i] % 3 == 1){
				for(int j = i+1; j < 8; j++){
					if(input[j] % 3 == 2){
						if(aerial){
							if(airBlock->cancel(cMove, connectFlag, hitFlag)) {
								airBlock->init(st);
								cMove = airBlock;
							}
						} else {
							if(input[0] > 3){ 
								if(standBlock->cancel(cMove, connectFlag, hitFlag)) {
									standBlock->init(st);
									cMove = standBlock;
								}
							} else {
								if(crouchBlock->cancel(cMove, connectFlag, hitFlag)) {
									crouchBlock->init(st);
									cMove = crouchBlock;
								}
							}
							ret = 2;
							j = 10;
						}
					}
				}
				i = 9;
			}
		}
		break;
	case 7:
	case 4:
	case 1:
		if(aerial){
			if(airBlock->cancel(cMove, connectFlag, hitFlag)) {
				airBlock->init(st);
				cMove = airBlock;
			}
		} else { 
			if(input[0] > 3){ 
				if(standBlock->cancel(cMove, connectFlag, hitFlag)) {
					standBlock->init(st);
					cMove = standBlock;
				}
			} else {
				if(crouchBlock->cancel(cMove, connectFlag, hitFlag)) {
					crouchBlock->init(st);
					cMove = crouchBlock;
				}
			}
		}
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

int character::takeHit(action *& cMove, hStat & s, int blockType, int &frame, int &connectFlag, int &hitFlag, int &hitType, bool &aerial, int *& meter)
{
	bool dead = false;
	int freeze = 0;
	if(s.pause < 0){ 
		freeze = s.stun/4 + 10;
		if(s.ghostHit) freeze = 0;
	} else freeze = s.pause;
	hitType = cMove->takeHit(s, blockType, frame, connectFlag, hitFlag);
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
		die->init(s.stun+s.untech);
		cMove = die;
		aerial = true;
	} else if (hitType == 1){
		if(s.launch) aerial = true;
		if(s.stun != 0){
			frame = 0;
			if(aerial){
				untech->init(s.stun+s.untech);
				cMove = untech;
				resetAirOptions(meter);
			} else if(cMove->crouch){
				crouchReel->init(s.stun + s.stun/5);
				cMove = crouchReel;
			} else {
				reel->init(s.stun);
				cMove = reel;
			}
		}
	} else if (hitType == -1) {
		if(meter[1] + 6 < 300) meter[1] += 6;
		else meter[1] = 300;
	} else if (hitType > -2) {
		if(meter[1] + 1 < 300) meter[1] += 1;
		else meter[1] = 300;
	}
	return freeze;
}

int * avatar::generateMeter()
{
	int * meter;
	meter = new int[4];
	return meter;
}

void character::init(int *& meter)
{
	meter[0] = 600;
	meter[1] = 0;
	resetAirOptions(meter);
}

void character::resetAirOptions(int *& meter)
{
	meter[2] = 1;
	meter[3] = 1;
}

int avatar::acceptTarget(action * c, int f)
{
	return 1;
}

void character::land(action *& cMove, int &frame, int &connectFlag, int &hitFlag, int *& meter)
{
	if(cMove->allowed.b.block){
		standBlock->init(airBlock->counter);
		cMove = standBlock;
	} else { 
		cMove = cMove->land(frame, connectFlag, hitFlag);
		if(!cMove) cMove = neutral;
	}
	resetAirOptions(meter);
}

void avatar::step(action *& cMove, int &currentFrame, int &freeze, int &connectFlag, int &hitFlag, int *& meter)
{
	if(freeze <= 0) {
		cMove->step(meter, currentFrame, connectFlag, hitFlag);
		if(cMove->hits > 0 && cMove->stats[connectFlag-1].noConnect) connectFlag--;
		tick(meter);
	} else {
		freeze--;
	}
}

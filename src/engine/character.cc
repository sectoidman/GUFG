#include <stdio.h>
#include <cmath>
#include <fstream>
#include <assert.h>
#include <cstdlib>
#include "character.h"
#include <iostream>
#include <cstring>

character::character()
{
	meter = new int[4];
}

character::character(const char*)
//Character constructor. This loads the whole character into memory so that that we don't have disk reads during gameplay
{
/*	action * temp;

	name = NULL;

	head = new actionTrie(new action("White/A"));

	temp = new action("White/D");
	head->insert(temp, 0);
	airHead = new actionTrie(temp, 0);
	
	head->insert(new action("White/B"), 0);
	neutral = new looping("White/NS");
	crouch = new looping("White/NL");
	head->insert(5, neutral);
	head->insert(2, crouch);
	head->insert(1, crouch);
	head->insert(3, crouch);

	airNeutral = new airLooping("White/NS");
	airNeutral->feed(neutral, 1, 0);

	head->insert(4, new looping("White/WQ"));
	head->insert(6, new looping("White/W"));

	reel = new hitstun("White/HS");
	untech = new untechState("White/UT");
	crouchReel = new hitstun("White/HL");

	airBlock = new hitstun("White/BA");
	standBlock = new hitstun("White/BH");
	crouchBlock = new hitstun("White/BL");

	head->insert(8, new utility("White/JN"));
	head->insert(7, new utility("White/JQ"));
	head->insert(9, new utility("White/JF"));
	
	throwBreak = new utility("White/break");

	meter = new int[3];
*/
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
		delete [] meter;
		if(name) delete [] name;
	}
}

void avatar::prepHooks(int freeze, action *& cMove, action *& bMove, action *& sMove, int inputBuffer[30], int down[5], bool up[5], SDL_Rect &p, int &f, int &cFlag, int &hFlag, bool dryrun)
{
	action * t = NULL;
	if (cMove == NULL) neutralize(cMove);
	t = hook(inputBuffer, 0, -1, meter, down, up, cMove, p, cFlag, hFlag);

	if(t == NULL && cMove->window(f)){
		if(cMove->attempt->check(p, meter)) t = cMove->attempt;
	}

	if(t != NULL){
		if(freeze > 0){
			if(bMove == NULL){ 
				if(!dryrun) bMove = t;
/*			} else if(sMove == NULL){
				if(!dryrun) sMove = t;
*/			}
		}
		else {
			if(!dryrun){ 
				t->execute(cMove, meter, f, cFlag, hFlag);
			}
			cMove = t;
		}
	} else if (bMove != NULL && freeze <= 0) {
		if(!dryrun){ 
			bMove->execute(cMove, meter, f, cFlag, hFlag);
		}
		cMove = bMove;
		if(!dryrun) bMove = NULL;
	} else if (sMove != NULL && freeze <= 0) {
		if(sMove->check(p, meter) && sMove->cancel(cMove, cFlag, hFlag)){
			cMove = sMove;
			if(!dryrun){
				sMove->execute(cMove, meter, f, cFlag, hFlag);
				sMove = NULL;
			}
		}
	}
}

action * avatar::hook(int inputBuffer[30], int i, int f, int * r, int down[5], bool up[5], action * c, SDL_Rect &p, int &cFlag, int &hFlag)
{
	return head->actionHook(inputBuffer, 0, -1, meter, down, up, c, p, cFlag, hFlag);
}

action * character::hook(int inputBuffer[30], int i, int f, int * r, int down[5], bool up[5], action * c, SDL_Rect &p, int &cFlag, int &hFlag)
{
	if(aerial) return airHead->actionHook(inputBuffer, 0, -1, meter, down, up, c, p, cFlag, hFlag);
	else return avatar::hook(inputBuffer, 0, -1, meter, down, up, c, p, cFlag, hFlag);
}

void avatar::neutralize(action *& cMove){
	cMove = neutral;
}

void character::neutralize(action *& cMove){
	if(aerial) cMove = airNeutral;
	else cMove = neutral;
}

void avatar::getName(const char* directory, const char* file)
{
	char buffer[101];
	std::ifstream read;
	sprintf(buffer, "resources/characters/%s/%s.ch", directory, file);

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
	sprintf(buffer, "resources/characters/%s/%s.ch", directory, file);

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
	char component[2];
	char * token;
	int pattern;
	int q;
	actionTrie * t = NULL;
	token = strtok(buffer, " \t=>-&?@%$_!\n");
	while (token){
		token = NULL;
		token = strtok(NULL, " \t=>-&?@%$_!\n");
		if(token) {
			switch (token[0]){
			case 'h':
				t = head;
				break;
			default:
				break;
			}
			pattern = 0;
			for(int i = strlen(token)-1; i > 0; i--){
				switch(token[i]){
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
					pattern += 1 << (token[i] - 'A');
					break;
				default:
					sprintf(component, "%c\0", token[i]);
					q = atoi(component);
					if(q > 10) q = q % 10;
					t = t->insert(q);
					break;
				}
			}
			t->insert(m, pattern);
		}
	}
}

void character::sortMove(action * m, char* buffer)
{
	char component[2];
	char * token;
	int q;
	int pattern;
	actionTrie * t = NULL;
	token = strtok(buffer, " \t=>-&?@%$_!\n");
	while (token){
		token = NULL;
		token = strtok(NULL, " \t=>-&?@%$_!\n");
		if(token) {
			switch (token[0]){
			case 'h':
				t = head;
				break;
			case 'a':
				t = airHead;
				break;
			default:
				break;
			}
			pattern = 0;
			for(int i = strlen(token)-1; i > 0; i--){
				switch(token[i]){
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
					pattern += 1 << (token[i] - 'A');
					break;
				default:
					sprintf(component, "%c\0", token[i]);
					q = atoi(component);
					if(q > 10) q = q % 10;
					t = t->insert(q);
					break;
				}
			}
			t->insert(m, pattern);
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
	die = new airLooping(buffer);

	sprintf(buffer, "%s/dead", name);
	dead = new looping(buffer);
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

void character::init(action *& cMove){
	meter[0] = 600;
	meter[1] = 0;
	resetAirOptions();
	aerial = 0;
	neutralize(cMove);
}

void avatar::processMove(action * m)
{
	char* temp = NULL;
	action* t = NULL;
	for(int i = 0; i < 6; i++){
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

	token = strtok(fullName, " \t=>-&?@%$_!\n");
	sprintf(actionName, "%s/%s", name, token);

	action * m;
	switch(type[0]){
	case '%':
		if(type[1] == 'j') m = new airSpecial(actionName);
		else m = new special(actionName);
		break;
	case '-':
		if(type[1] == 'j') m = new airUtility(actionName);
		else m = new utility(actionName);
		break;
	case '@':
		if(type[1] == 'j') m = new airLooping(actionName);
		else m = new looping(actionName);
		break;
	case '!':
//		if(type[1] == 'j') m = new airSuper(actionName);
//		else 
		if(type[1] == 'j') m = new airSuper(actionName);
		else m = new super(actionName);
		break;
	case '?':
		m = new mash(actionName);
		break;
	case '_':
		if(type[1] == 'j') m = new luftigeWerf(actionName);
		else m = new werf(actionName);
		break;
	case '>':
		if(type[1] == 'j') m = new airSummon(actionName);
		else m = new summon(actionName);
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

void avatar::connect(action *& cMove, action *& bMove, action *& sMove, hStat & s, int & c, int f)
{
	action * t = cMove->connect(meter, c, f);
	if(t != NULL){
		if(bMove != NULL) sMove = bMove;
		bMove = t;
	}
}

int character::checkBlocking(action *& cMove, int input[], int &connectFlag, int &hitFlag)
{
	int st;
	bool success = false;
	int ret = -1;
	st = cMove->arbitraryPoll(1, 0);
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

int character::takeHit(action *& cMove, hStat & s, int b, int &f, int &c, int &h, int &p)
{
	bool dead = false;
	int freeze = 0;
	if(s.pause < 0){ 
		freeze = s.stun/4 + 10;
		if(s.ghostHit) freeze = 0;
	} else freeze = s.pause;
	p = cMove->takeHit(s, b, f, c, h);
	if(p == 1) meter[0] -= s.damage;
	else if(p > -2) { 
		meter[0] -= s.chip;
		if(p == -1 && meter[0] <= 0){ 
			meter[0] = 1;
		}
	}
	if(meter[0] <= 0){ 
		meter[0] = 0;
		dead = true;
	}
	if(dead == true){
		cMove = die;
		aerial = true;
	} else if (p == 1){
		if(s.launch) aerial = 1;
		if(s.stun != 0){
			f = 0;
			if(aerial){
				untech->init(s.stun+s.untech);
				cMove = untech;
				resetAirOptions();
			} else if(cMove->crouch){
				crouchReel->init(s.stun + s.stun/5);
				cMove = crouchReel;
			} else {
				reel->init(s.stun);
				cMove = reel;
			}
		} 
	} else if (p == -1) {
		if(meter[1] + 6 < 300) meter[1] += 6;
		else meter[1] = 300;
	}
	if(p > -2){
		if(meter[1] + 1 < 300) meter[1] += 1;
		else meter[1] = 300;
	}
	return freeze;
}

void character::resetAirOptions()
{
	meter[2] = 1;
	meter[3] = 1;
}

bool avatar::acceptTarget(action * c, int f)
{
	return 1;
}

void character::land(action *& cMove, int &f, int &c, int &h)
{
	if(cMove->allowed.b.block){
		standBlock->init(airBlock->counter);
		cMove = standBlock;
	} else { 
		cMove = cMove->land(f, c, h);
		if(!cMove) cMove = neutral;
	}
	aerial = 0; 
	resetAirOptions();
}

void avatar::step(action *& cMove, int &currentFrame, int &freeze)
{
	if(freeze <= 0) {
		cMove->step(meter, currentFrame);
		tick();
	} else {
		freeze--;
	}
}

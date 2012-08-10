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
	bMove = NULL;
	cMove = NULL;
	meter = new int[3];
}

character::character(const char*)
//Character constructor. This loads the whole character into memory so that that we don't have disk reads during gameplay
{
	action * temp;

	name = NULL;

	/*Currently I'm using this as a test case for my action hooks*/

	head = new actionTrie(new action("White/A"));

	temp = new action("White/D");
	head->insert(temp);
	airHead = new actionTrie(temp);
	
	head->insert(new action("White/B"));
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
	
	cMove = neutral;
	bMove = NULL;

	meter = new int[3];
}

character::~character()
	//Character destructor. Might not need this if we aren't working with any dynamic memory, but it might be worthwhile to have.
{
	delete head;
	delete airHead;
	delete neutral;
	delete crouch;
	delete reel;
	delete untech;
	delete crouchReel;
	delete crouchBlock;
	delete standBlock;
	delete airBlock;
	delete [] meter;
	if(name) delete [] name;
}

/*Here begin action functions. Actually contemplating making this a class instead, but this might be simpler for now*/

void character::prepHooks(int inputBuffer[30], bool down[5], bool up[5], SDL_Rect &p, bool dryrun)
{
	action * t = NULL;
	if (cMove == NULL) {
		if(aerial) cMove = airNeutral;
		else cMove = neutral;
	}
	
	if(aerial) t = airHead->actionHook(inputBuffer, 0, -1, meter, down, up, cMove, p);
	else t = head->actionHook(inputBuffer, 0, -1, meter, down, up, cMove, p);
	if(t == NULL && cMove->window()){
		if(cMove->attempt->check(p)) t = cMove->attempt;
	}

	if(t != NULL){
		if(freeze > 0){
			if(bMove == NULL) 
				if(!dryrun) bMove = t;
		}
		else {
			if(!dryrun) t->execute(cMove, meter);
			cMove = t;
		}
	} else if (bMove != NULL && freeze <= 0) {
		if(!dryrun) bMove->execute(cMove, meter);
		cMove = bMove;
		if(!dryrun) bMove = NULL;
	} 
}

void avatar::build(const char* n)
{
	char buffer[101];
	char buffer2[101];
//	char actionName[151];
//	char type[2];
	actionTrie * t = NULL;
	action * m = NULL;
	bool commentFlag;
	char component[2];
	char * token;
	int q;
	std::ifstream read;
	sprintf(buffer, "%s/%s.ch", n, n);

	read.open(buffer);
	assert(!read.fail());

	read.get(buffer, 50); read.ignore(100, '\n');
	name = new char[strlen(buffer)+1];
	strcpy(name, buffer);

	while(!read.eof()){
		commentFlag = 0;
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		
		if(buffer[0] == '#' || buffer[0] == '\0')
			commentFlag = 1;
		if(!commentFlag){
			strcpy(buffer2, buffer);

			m = createMove(buffer);
			processMove(m);
			token = strtok(buffer2, " \t-?%@$&_!\n");
			while (token){
				token = NULL;
				token = strtok(NULL, " \t=-&?@%$_!\n");
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
					for(int i = strlen(token)-1; i > 0; i--){
						sprintf(component, "%c\0", token[i]);
						q = atoi(component);// % 10;
						if(q > 10) q = q % 10;
						t = t->insert(q);
					}
					t->insert(m);
				}
			}
		}
	}
	read.close();	

}

void character::build(const char *n)
{
	char buffer[101];
	sprintf(buffer, "%s/NS", n);
	neutral = new looping(buffer);

	sprintf(buffer, "%s/NL", n);
	crouch = new looping(buffer);

	sprintf(buffer, "%s/NJ", n);
	airNeutral = new airLooping(buffer);
	airNeutral->feed(neutral, 1, 0);

	sprintf(buffer, "%s/HS", n);
	reel = new hitstun(buffer);

	sprintf(buffer, "%s/Fall", n);
	fall = new airLooping(buffer);
	
	sprintf(buffer, "%s/UT", n);
	untech = new untechState(buffer);
	untech->feed(fall, 0, 0);

	sprintf(buffer, "%s/down", n);
	down = new utility(buffer);
	untech->feed(down, 1, 0);
	fall->feed(down, 1, 0);

	sprintf(buffer, "%s/BA", n);
	airBlock = new hitstun(buffer);

	sprintf(buffer, "%s/HL", n);
	crouchReel = new hitstun(buffer);

	sprintf(buffer, "%s/BH", n);
	standBlock = new hitstun(buffer);
	
	sprintf(buffer, "%s/BL", n);
	crouchBlock = new hitstun(buffer);
	
	sprintf(buffer, "%s/BA", n);
	airBlock = new hitstun(buffer);	

	sprintf(buffer, "%s/break", n);
	throwBreak = new utility(buffer);

	avatar::build(n);

	head->insert(5, neutral);
	
	head->insert(2, crouch);
	head->insert(3, crouch);
	head->insert(1, crouch);

	airHead->insert(5, airNeutral);
}

void character::init(){
	if(cMove){
		if(cMove != neutral) cMove->init();
	}
	neutral->init();
	cMove = neutral;
	health = 300;
	meter[0] = 0;
	resetAirOptions();
	aerial = 0;
	freeze = 0;
}

void avatar::processMove(action * m)
{
	char* temp = NULL;
	action* t = NULL;
	for(int i = 0; i < 4; i++){
		if(i == 2){
			for(int j = 0; j < m->hits; j++){
				temp = m->request(i, j);
				if(temp != NULL){ 
					t = createMove(temp);
					m->feed(t, i, j);
					processMove(t);
				}
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

	token = strtok(fullName, " \t-@?%_$!\n");
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
	case 'j':
		m = new airMove(actionName);
		break;
	default:
		m = new action(actionName);
		break;	
	}
	return m;
}

void avatar::connect(hStat & s)
{
	cMove->connect(meter, bMove);
	if(bMove == cMove){ 
		bMove = NULL;
	}
	if(!s.ghostHit) freeze = s.stun/4+10;
}

int character::takeHit(hStat & s, int b)
{
	if(s.ghostHit) freeze = 0;
	else freeze = s.stun/4+10;
	int x = cMove->takeHit(s, b);
	if (x == 1){
		if(s.launch) aerial = 1;
		health -= s.damage; 
		if(health < 0) health = 0;
		if(s.stun > 0){
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
	} else if (x == -1) {
		if(meter[0] + 4 < 200) meter[0] += 4;
		else meter[0] = 200;
	}
	if(meter[0] + 1 < 200) meter[0] += 1;
	else meter[0] = 200;
	return x;
}

void character::resetAirOptions()
{
	meter[1] = 1;
	meter[2] = 1;
}

void character::land()
{
	if(cMove == airBlock){
		standBlock->init(airBlock->counter);
		cMove = standBlock;
	} else { 
		cMove = cMove->land();
		if(!cMove) cMove = neutral;
	}
	aerial = 0; 
	resetAirOptions();
}

void character::step()
{
	if(freeze <= 0) {
		cMove->step(meter);
		tick();
	}
	if(freeze > 0) freeze--;
	if(cMove && cMove->currentFrame == cMove->frames){
		cMove->init();
		cMove = cMove->next;
		if(cMove) cMove->init();
	}
}

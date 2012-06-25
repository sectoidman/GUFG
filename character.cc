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

	meter = new int[3];
}

character::character(const char*)
//Character constructor. This loads the whole character into memory so that that we don't have disk reads during gameplay
{
	move * temp;

	name = NULL;

	/*Currently I'm using this as a test case for my move hooks*/

	head = new moveTrie(new move("White/A"));

	temp = new move("White/D");
	head->insert(temp);
	airHead = new moveTrie(temp);
	
	head->insert(new move("White/B"));
	neutral = new looping("White/NS");
	crouch = new looping("White/NL");
	head->insert(5, neutral);
	head->insert(2, crouch);
	head->insert(1, crouch);
	head->insert(3, crouch);

	airNeutral = new airLooping("White/NS");
	airNeutral->feed(neutral, 1);

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
	
	cMove = neutral;
	bMove = NULL;

	meter = new int[3];
}

character::~character()
	//Character destructor. Might not need this if we aren't working with any dynamic memory, but it might be worthwhile to have.
{
	delete head;
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

/*Here begin move functions. Actually contemplating making this a class instead, but this might be simpler for now*/

void character::prepHooks(int inputBuffer[30], bool down[5], bool up[5], SDL_Rect &p)
{
	move * t = NULL;
	if (cMove == NULL) {
		if(aerial) cMove = airNeutral;
		else cMove = neutral;
	}
	
	if(aerial) t = airHead->moveHook(inputBuffer, 0, -1, meter, down, up, cMove, p);
	else t = head->moveHook(inputBuffer, 0, -1, meter, down, up, cMove, p);

	if(t != NULL){
		if(freeze > 0){
			if(bMove == NULL || (*t) > bMove) bMove = t;
		} 
		else {
			t->execute(cMove, meter);
			cMove = t;
		}
	} else if (bMove != NULL && freeze <= 0) {
		bMove->execute(cMove, meter);
		cMove = bMove;
		bMove = NULL;
	}
}

void character::build(const char* n)
{
	char buffer[101];
	char buffer2[101];
//	char moveName[151];
//	char type[2];
	moveTrie * t = NULL;
	move * m = NULL;
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

	sprintf(buffer, "%s/NS", name);
	neutral = new looping(buffer);

	sprintf(buffer, "%s/NL", name);
	crouch = new looping(buffer);

	sprintf(buffer, "%s/NJ", name);
	airNeutral = new airLooping(buffer);
	airNeutral->feed(neutral, 1);

	sprintf(buffer, "%s/HS", name);
	reel = new hitstun(buffer);

	sprintf(buffer, "%s/Fall", name);
	fall = new airLooping(buffer);
	
	sprintf(buffer, "%s/UT", name);
	untech = new untechState(buffer);
	untech->feed(fall, 0);

	sprintf(buffer, "%s/down", name);
	down = new utility(buffer);
	untech->feed(down, 1);
	fall->feed(down, 1);

	sprintf(buffer, "%s/BA", name);
	airBlock = new hitstun(buffer);

	sprintf(buffer, "%s/HL", name);
	crouchReel = new hitstun(buffer);

	sprintf(buffer, "%s/BH", name);
	standBlock = new hitstun(buffer);
	
	sprintf(buffer, "%s/BL", name);
	crouchBlock = new hitstun(buffer);
	
	sprintf(buffer, "%s/BA", name);
	airBlock = new hitstun(buffer);	

	while(!read.eof()){
		commentFlag = 0;
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		
		if(buffer[0] == '#' || buffer[0] == '\0')
			commentFlag = 1;
		if(!commentFlag){
			strcpy(buffer2, buffer);

			m = createMove(buffer);
			token = strtok(buffer2, " \t-?%@$!\n");
			while (token){
				token = NULL;
				token = strtok(NULL, " \t=-?@%$!\n");
				if(token) {
					switch (token[0]){
					case 'h':
						t = head;
						break;
					case 'a':
						t = airHead;
						break;
					default:
						printf("??\n");
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

	head->insert(5, neutral);
	
	head->insert(2, crouch);
	head->insert(3, crouch);
	head->insert(1, crouch);

	airHead->insert(5, airNeutral);
}

void character::init(){
	health = 300;
	meter[0] = 0;
	meter[1] = 1;
	meter[2] = 1;
	rounds = 0;
	aerial = 0;
	freeze = 0;
	cMove = neutral;
}

move * character::createMove(char * fullName)
{
	char * token;
	char type[2] = {fullName[0], fullName[1]};
	char moveName[151];

	token = strtok(fullName, " \t-@?%$!\n");
	sprintf(moveName, "%s/%s", name, token);

	move * m;
	switch(type[0]){
	case '%':
		if(type[1] == 'j') m = new airSpecial(moveName);
		else m = new special(moveName);
		break;
	case '-':
		if(type[1] == 'j') m = new airUtility(moveName);
		else m = new utility(moveName);
		break;
	case '@':
		if(type[1] == 'j') m = new airLooping(moveName);
		else m = new looping(moveName);
		break;
	case '!':
//		if(type[1] == 'j') m = new airSuper(moveName);
//		else 
		m = new super(moveName);
		break;
	case '?':
		m = new mash(moveName);
		break;
	case 'j':
		m = new airMove(moveName);
		m->feed(neutral, 1);
		break;	
	default:
		m = new move(moveName);
		break;	
	}
	return m;
}

void character::connect(hStat & s)
{
	cMove->connect(meter);
	freeze = s.stun/4+10;
}

int character::takeHit(hStat & s)
{
	freeze = s.stun/4+10;

	if(cMove->takeHit(s))
	{
		if(s.launch) aerial = 1;
		health -= s.damage; 
		if(health < 0) health = 0;
		if(aerial){
			untech->init(s.stun+s.untech);
			cMove = untech;
			resetAirOptions();
		} else if(cMove->crouch){
			crouchReel->init(s.stun+2);
			cMove = crouchReel;
		} else {
			reel->init(s.stun);
			cMove = reel;
		}
		return 1;
	} else return 0;
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
	if(freeze <= 0) cMove->step(meter);
	if(freeze > 0) freeze--;
	if(cMove->currentFrame == cMove->frames){
		cMove->init();
		cMove = cMove->next;
	}
}

#include <stdio.h>
#include <cmath>
#include <fstream>
#include <assert.h>
#include <cstdlib>
#include "character.h"
#include <iostream>
#include <cstring>
using namespace std;

character::character()
//Character constructor. This loads the whole character into memory so that that we don't have disk reads during gameplay
{
	name = "White";
	move * temp;


	/*Currently I'm using this as a test case for my move hooks*/

	head = new moveTrie(new move("White/A"));

	temp = new move("White/D");
	head->insert(temp);
	airHead = new moveTrie(temp);
	
	head->insert(new move("White/B"));
	neutral = new utility("White/NS");
	crouch = new looping("White/NL");
	head->insert(neutral);

	head->insert(4, new utility("White/WQ"));
	head->insert(6, new utility("White/W"));

	reel = new hitstun("White/HS");
	fall = new hitstun("White/UT");
	crouchReel = new hitstun("White/HL");

	airBlock = new hitstun("White/BA");
	standBlock = new hitstun("White/BH");
	crouchBlock = new hitstun("White/BL");
	
	head->insert(8, new utility("White/JN"));
	head->insert(7, new utility("White/JQ"));
	head->insert(9, new utility("White/JF"));
	
	cMove = neutral;
	bMove = NULL;

	meter = new int[1];
}

character::~character()
	//Character destructor. Might not need this if we aren't working with any dynamic memory, but it might be worthwhile to have.
{
	delete head;
	delete neutral;
	delete crouch;
	delete reel;
	delete fall;
	delete crouchReel;
	delete crouchBlock;
	delete standBlock;
	delete airBlock;
	delete [] meter;
}

/*Here begin move functions. Actually contemplating making this a class instead, but this might be simpler for now*/

int character::takeHit(character * attacker, SDL_Rect &pushVector, int combo)
{
	move *attack = attacker->cMove;
	/*All the important logic like blocking and stuff will go here later.*/
	cMove->init();
	int ct = 0;
	int dmg;
	/*Damage scaling logic will factor into this later*/
	if(attack->cFlag <= attack->currentHit){
		if(cMove->blockState.i & attack->blockMask[attack->currentHit].i){
		/*Do blocking stuff. Specifically, we need to put the player in
		block stun, a state in which they're frozen in the last block animation until blockstun ends.
		During blockstun, generally the option available to everyone is to switch blocks, so as not
		to allow mixup to be guaranteed. Some games have options like Alpha Counters (Any attack out of blockstun) 
		or Rolls (Invulnerable movement option) that cost meter but can break out of blockstun state. These can
		be universal or character-specific. Notably, characters are throw-invulnerable during blockstun,
		for what should be obvious reasons. In MOST games, characters remain throw-invuln for a few frames after
		coming out of blockstun.
		*/
			cMove->blockSuccess(attack->stun[attack->currentHit]);
			printf("Block!\n");
		}
		else{
		/*Do hitstun stuff. Specifically, the player needs to be put in a "hitstun" state for a number
		of frames defined by the stun of the attacking move. Blockstun may be separate, or a function of the same
		number. The hitstun state while standing generally has a "reeling" animation that can affect what hits them,
		but unless we have a "burst-like" option, no actions can be taken by a player during hitstun. There will probably
		also be a "launch" property, which knocks a grounded player into the air if hit by certain moves.
		In the air, hitstun is slightly different. There is a "falling" animation the character is in, and they are launched
		a little bit by any further move that hits them. Generally, there's some option to get out of aerial hitstun, most
		easily referred to as a "Tech." Therefore, while falling state persists until they hit the ground, there's an amount of
		"untechable" time associated with any move that hits them. This is not functionally different from hitstun in any other way.
		*/
			if(cMove == reel || cMove == fall || cMove == crouchReel) ct++;
			if(!aerial && attack->launch) aerial = 1;
			if(aerial){
				pushVector.y = -(attack->lift[attack->currentHit]);
				fall->init(attack->stun[attack->currentHit] - combo);
				cMove = fall;
			} else {
				if(cMove == crouch || cMove == crouchReel){
					crouchReel->init(attack->stun[attack->currentHit]+1);
					cMove = crouchReel;
				} else {
					reel->init(attack->stun[attack->currentHit]);
					cMove = reel;
				}
			}

			//This will probably change, but it's the simplest possible damage scaling algorithm
			dmg = (attack->damage[attack->currentHit] - combo);

			//This won't change. Minimum damage per hit is 1.
			if(dmg <= 0) dmg = 1;
			health -= dmg;
			if(health < 0){
				health = 0; //Healthbar can't go below 0;
				//Reckon other KO stuff;
			}
		}
		if(aerial) pushVector.x = -(attack->push[attack->currentHit]/5);
		else if (cMove == crouchReel) pushVector.x = -(attack->push[attack->currentHit]);
		
		else pushVector.x = -(attack->push[attack->currentHit]);
		
		attack->connect(); //Tell the attack it's connected.
	}
	freeze = 10 + attack->stun[attack->currentHit] / 10; //For now this is the simple formula for freeze. Eventually it might be changed, or made a separate parameter
 	return ct;
	/*Eventually the plan is to have this return a combo count. This not only allows us to display a counter and do whatever scaling/combo 
	mitigation we want to, but also allows us to do things like pushback ramping during blockstrings*/
}

void character::prepHooks(int inputBuffer[30], bool down[5], bool up[5])
{
	move * t = NULL;
	if (cMove == NULL) {
		if(aerial) cMove = /*air*/neutral;
		else cMove = neutral;
	}
	
	if(aerial) t = airHead->moveHook(inputBuffer, 0, -1, meter, down, up, cMove);
	else t = head->moveHook(inputBuffer, 0, -1, meter, down, up, cMove);

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
	char moveName[151];
	char name[51];
	char type[2];
	moveTrie * t = NULL;
	move * m = NULL;
	char component;
	bool commentFlag;
	char * token;
	int q;
	ifstream read; 
	sprintf(buffer, "%s/%s.ch", n, n);
	
	read.open(buffer);
	assert(!read.fail());
	read.get(name, 50); read.ignore(100, '\n');
	while(!read.eof()){
		commentFlag = 0;
		read.get(buffer, 100, '\n'); read.ignore(100, '\n');
		type[0] = buffer[0];
		type[1] = buffer[1];
		
		if(type[0] == '#' || type[0] == '\0')
			commentFlag = 1;
		if(!commentFlag){
			strcpy(buffer2, buffer);
			token = strtok(buffer, " \t-@%$\n");
			sprintf(moveName, "%s/%s", name, token);
			m = createMove(type, moveName);
			token = strtok(buffer2, " \t-%@$\n");
			while (token){
				token = NULL;
				token = strtok(NULL, " \t=-@%$\n");
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
						component = token[i];
						q = atoi(&component);// % 10;
						if(q > 10) { 
//							printf("%s : %s\n", token, &component);
							q = q % 10;
						}
						t = t->insert(q);
					}
					t->insert(m);
				}
			}
		}
	}
	read.close();	

	sprintf(buffer, "%s/NS", name);
	neutral = new looping(buffer);
	head->insert(5, neutral);
	
	sprintf(buffer, "%s/NL", name);
	crouch = new looping(buffer);
	head->insert(2, crouch);
	head->insert(3, crouch);
	head->insert(1, crouch);

	sprintf(buffer, "%s/NJ", name);
	airNeutral = new airLooping(buffer);
	airHead->insert(airNeutral);

	sprintf(buffer, "%s/HS", name);
	reel = new hitstun(buffer);
	
	sprintf(buffer, "%s/UT", name);
	fall = new hitstun(buffer);

	sprintf(buffer, "%s/HL", name);
	crouchReel = new hitstun(buffer);

	sprintf(buffer, "%s/BH", name);
	standBlock = new hitstun(buffer);
	
	sprintf(buffer, "%s/BL", name);
	crouchBlock = new hitstun(buffer);
	
	sprintf(buffer, "%s/BA", name);
	airBlock = new hitstun(buffer);	
}

void character::init(){
	health = 300;
	meter[0] = 0;
	rounds = 0;
	aerial = 0;
	freeze = 0;
}

move * character::createMove(char * type, char * moveName)
{
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
	case 'j':
		m = new airMove(moveName);
		break;	
	default:
		m = new move(moveName);
		break;	
	}
	return m;
}

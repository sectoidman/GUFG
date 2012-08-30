#include <stdio.h>
#include <iostream>
#include <SDL/SDL.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include "player.h"
player::player()
{
	init();
}

player::player(int id)
{
	ID = id;
	init();
}

instance::instance()
{
	init();
}

instance::instance(avatar * f)
{
	v = f;
	init();
}

void instance::init()
{
	momentumComplexity = 0;
	momentum = NULL;
	deltaX = 0;
	deltaY = 0;
	regComplexity = 0;
	hitComplexity = 0;
	currentFrame = 0;
	connectFlag = 0;
	hitFlag = 0;
	cMove = NULL;
	bMove = NULL;
	sMove = NULL;
	freeze = 0;
	for(int i = 0; i < 30; i++) inputBuffer[i] = 5;
}

bool instance::acceptTarget(instance * m)
{
	if(m->ID == ID) return 0;
	else return m->pick()->acceptTarget(cMove, currentFrame);
}

void player::init()
{
	/*Initialize input containers*/
	for(int i = 0; i < 30; i++)
		inputBuffer[i] = 5;

	/*Yeah yeah, I know, char* to literal conversion. I'm lazy right now. Will fix later. Maybe with cstring*/
	inputName[0] = "Up";
	inputName[1] = "Down";
	inputName[2] = "Left";
	inputName[3] = "Right";
	inputName[4] = "A";
	inputName[5] = "B";
	inputName[6] = "C";
	inputName[7] = "D";
	inputName[8] = "E";
	inputName[9] = "Start";

	if(!readConfig()) writeConfig();

	/*Set up the sprite*/

	v = NULL;
	rounds = 0;
	instance::init();
}

void player::roundInit()
{
	instance::init();
	cMove = pick()->neutral;
	if(v) pick()->init(cMove);
	updateRects();
	lCorner = 0;
	rCorner = 0;
	elasticX = 0;
	elasticY = 0;
	blockType = 0;
	slide = 0;
	stick = 0;
	hover = 0;
	throwInvuln = 0;
	particleLife = 0;
	particleType = 0;
	if(ID == 1){ 
		facing = 1;
		posX = 1400;
	} else {
		facing = -1;
		posX = 1800;
	}
	updateRects();
}

bool player::readConfig()
{
	char fname[30];
	sprintf(fname, "Misc/.p%i.conf", ID);
	std::ifstream read;
	read.open(fname);
	if(read.fail()) {
		read.close();
		return 0;
	}
	else{
		char * token;
		char buffer[30];
		for(int i = 0; i < 10; i++){
			read.get(buffer, 30, ':'); read.ignore(); 
			input[i].type = atoi(buffer);
			read.ignore();
			read.get(buffer, 30, '\n'); read.ignore();
			switch (input[i].type){
			case SDL_JOYAXISMOTION:
				token = strtok(buffer, " \n");
				input[i].jaxis.which = atoi(token);
				token = strtok(NULL, " \n");
				input[i].jaxis.axis = atoi(token);
				token = strtok(NULL, " \n");
				input[i].jaxis.value = atoi(token);
				break;
			case SDL_JOYBUTTONDOWN:
				token = strtok(buffer, " \n");
				input[i].jbutton.which = atoi(token);
				token = strtok(NULL, " \n");
				input[i].jbutton.button = atoi(token);
				break;
			case SDL_KEYDOWN:
				token = strtok(buffer, " \n");	
				input[i].key.keysym.sym = (SDLKey)atoi(token);
				break;
			default:
				break;
			}
//			printf("Type?: %i\n", input[i].type);
		}
		read.close();
		return 1;
	}
}

bool player::aerial()
{
	return pick()->aerial;
}

void player::writeConfig()
{
	char fname[30];
	printf("Player %i:\n", ID);
	sprintf(fname, "Misc/.p%i.conf", ID);
	std::ofstream write;
	SDL_Event temp; 
	write.open(fname);
	/*Set up ALL the inputs*/
	for(int i = 0; i < 10; i++){
		printf("Please enter a command for %s\n", inputName[i]);
		/*Set up dummy event*/

		/*Flag for breaking the loop*/
		bool configFlag = 0;

		while(SDL_PollEvent(&temp));
		while (configFlag == 0){
			if (SDL_PollEvent(&temp)) {
				switch (temp.type) {
				case SDL_JOYAXISMOTION:
					if(temp.jaxis.value != 0){
						input[i] = temp;
						write << (int)temp.type << " : " << (int)temp.jaxis.which << " " << (int)temp.jaxis.axis << " " << (int)temp.jaxis.value << "\n";
						printf("Set to Joystick %i axis %i value %i\n", input[i].jaxis.which, input[i].jaxis.axis, input[i].jaxis.value);
						configFlag = 1;
					}
					break;
				case SDL_JOYBUTTONDOWN:
					input[i] = temp;
					write << (int)temp.type << " : " << (int)temp.jbutton.which << " " << (int)temp.jbutton.button << "\n";
					printf("Set to Joystick %i button %i\n", input[i].jbutton.which, input[i].jbutton.button);
					configFlag = 1;
					break;
				case SDL_KEYDOWN:
					input[i] = temp;
					write << (int)temp.type << " : " << (int)temp.key.keysym.sym << "\n";
					printf("Set to keyboard %s\n", SDL_GetKeyName(input[i].key.keysym.sym));
					configFlag = 1;
					break;
				default: 
					break;
				}
			}
		}
	}
	write.close();
}

void player::characterSelect(int i)
{
	v = NULL;
	switch(i){
	case 1:
		v = new red;
		break;
	case 2:
		v = new yellow;
		break;
	default:
		v = new character("White");
		break;
	}
}

void instance::updateRects()
{
	if(cMove != NULL) {
		cMove->pollRects(collision, hitreg, regComplexity, hitbox, hitComplexity, currentFrame, connectFlag);
		for(int i = 0; i < hitComplexity; i++){
			if(facing == -1) hitbox[i].x = posX - hitbox[i].x - hitbox[i].w;
			else hitbox[i].x += posX;
			hitbox[i].y += posY;
		}
		for(int i = 0; i < regComplexity; i++){
			if(facing == -1) hitreg[i].x = posX - hitreg[i].x - hitreg[i].w;
			else hitreg[i].x += posX;
			hitreg[i].y += posY;
		}
		if(facing == -1) collision.x = posX - collision.x - collision.w;
		else collision.x += posX;
		collision.y += posY;
	}
}

void instance::combineDelta()
{
	for(int i = 0; i < momentumComplexity; i++){
		deltaX += momentum[i].x;
		deltaY += momentum[i].y;

		if(momentum[i].w <= 0) {
			removeVector(i);
			i--;
		}
		else momentum[i].w--;
	}
	posX += deltaX;
	posY += deltaY;
	updateRects();
}

void player::enforceGravity(int grav, int floor)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(collision.y + collision.h < floor && pick()->aerial == 0){
		pick()->aerial = 1;
	}

	else if(pick()->aerial && !freeze){ 
		if(hover > 0 && deltaY + 3 > 0) g.y = -deltaY;
		addVector(g);
	}
}

void player::checkBlocking()
{
	int st;
	bool block = false;
	if(cMove == pick()->airBlock || cMove == pick()->standBlock || cMove == pick()->crouchBlock)
		st = cMove->arbitraryPoll(1, currentFrame);
	else st = 0;
	

	switch(inputBuffer[0]){
	case 7:
	case 4:
		if(pick()->aerial && pick()->airBlock->cancel(cMove, connectFlag, hitFlag)) {
			pick()->airBlock->init(st);
			cMove = pick()->airBlock;
		}
		else if(pick()->standBlock->cancel(cMove, connectFlag, hitFlag)) {
			pick()->standBlock->init(st);
			cMove = pick()->standBlock;
		}
		updateRects();
		block = true;
		break;
	case 1:
		if(pick()->aerial && pick()->airBlock->cancel(cMove, connectFlag, hitFlag)) {
			pick()->airBlock->init(st);
			cMove = pick()->airBlock;
		}
		else if(pick()->crouchBlock->cancel(cMove, connectFlag, hitFlag)) {
			pick()->crouchBlock->init(st);
			cMove = pick()->crouchBlock;
		}
		updateRects();
		block = true;
		break;
	default:
		break;
	}

	blockType = 0;
	if(block){
		for(int i = 1; i < 7; i++){
			if(inputBuffer[i] % 3 != 1)
			blockType = 1;
		}
	}
}

void player::checkCorners(int floor, int left, int right)
{
	/*Offset variables. I could do these calculations on the fly, but it's easier this way.
	Essentially, this represents the offset between the sprite and the collision box, since
	even though we're *checking* collision, we're still *moving* spr*/
	int lOffset = posX - collision.x;
	int rOffset = posX - (collision.x + collision.w);
	int hOffset = posY - (collision.y);

	/*Floor, or "Bottom corner"*/

	if (collision.y + collision.h > floor){
		if(elasticY){
			deltaY = -deltaY;
			elasticY = false;
		} else if (slide) {
			deltaY = 0;
			if(cMove == pick()->untech){ 
				if(deltaX < 0) deltaX++;
				else if(deltaX > 0) deltaX--;
				pick()->aerial = 1;
			} else {
				deltaX = 0;
				slide = 0;
			}
		} else {
			if(pick()->aerial == 1){
				land();
//				printf("P%i landed!\n", ID);
				updateRects();
				hOffset = posY - (collision.y);
				deltaX = 0;
			}
			deltaY = 0;
		}
		posY = floor - collision.h + hOffset;
	}
	updateRects();

	/*Walls, or "Left and Right" corners

	This not only keeps the characters within the stage boundaries, but flags them as "in the corner"
	so we can specialcase collision checks for when one player is in the corner.*/

	if(collision.x <= left){
		if(elasticX){
			if(deltaX < 0) deltaX = -deltaX;
			elasticX = false;
		} 
		if(collision.x <= 50){ 
			if(facing == 1) lCorner = 1;
			if (stick) {
				if(cMove == pick()->untech){
					deltaX = 0;
					deltaY = 0;
					momentumComplexity = 0;
				} else stick = 0;
			}
		}
		if(collision.x < left) 
			posX = left + lOffset;
	} else lCorner = 0;
	if(collision.x + collision.w >= right){
		if(elasticX){
			if(deltaX > 0) deltaX = -deltaX; 
			elasticX = false;
		} 
		if(collision.x + collision.w >= 3150){ 
			if(facing == -1) rCorner = 1;
			if (stick) {
				if(cMove == pick()->untech){
					deltaX = 0;
					deltaY = 0;
					momentumComplexity = 0;
				} else stick = 0;
			}
		}
		if(collision.x + collision.w > right){
			posX = right + rOffset;
		}
	} else rCorner = 0;
	updateRects(); //Update rectangles or the next collision check will be wrong.
}

void player::land()
{
	for(int i = 0; i < momentumComplexity; i++){
		if(momentum[i].y < 0) removeVector(i);
	}
	pick()->land(cMove, currentFrame, connectFlag, hitFlag);
}

void instance::step()
{
	if(pick()->death(cMove, currentFrame)) dead = true;
	if(posX > 3300 || posX < -100) dead = true;
	pick()->step(cMove, currentFrame, freeze);
	if(cMove && currentFrame >= cMove->frames){
		cMove = cMove->next;
		currentFrame = 0;
		connectFlag = 0;
		hitFlag = 0;
	}
}

void player::checkFacing(player * other){
	int comparison, midpoint;
	midpoint = collision.x + collision.w/2;
	comparison = other->collision.x + other->collision.w/2;

	if(other->posX < posX) comparison += collision.w % 2; 
	else midpoint += collision.w % 2;

	if (lCorner) facing = 1;
	else if (rCorner) facing = -1;
	else if (midpoint < comparison){
		if(facing == -1) posX += collision.x - (posX + (posX - collision.x - collision.w));
		facing = 1;
	}
	else if (midpoint > comparison){
		if(facing == 1) posX += (collision.w + collision.x) - posX*2 + collision.x;
		facing = -1;
	}

	updateRects();
}

int player::dragBG(int left, int right)
{
	if(collision.x < left) return collision.x - left;
	else if (collision.x + collision.w > right) return (collision.x + collision.w) - right;
	else return 0;
}

void instance::pushInput(bool axis[4])
{
	int temp = 5 + axis[0]*3 - axis[1]*3 - axis[2]*facing + axis[3]*facing;
	inputBuffer[0] = temp;

	for(int i = 29; i > 0; i--){
		inputBuffer[i] = inputBuffer[i-1];
	}
}

void instance::getMove(bool down[5], bool up[5], SDL_Rect &p, bool dryrun)
{
	action * dummyMove, *save;
	dummyMove = cMove;
	save = cMove;
	pick()->prepHooks(freeze, dummyMove, bMove, sMove, inputBuffer, down, up, p, currentFrame, connectFlag, hitFlag, dryrun);
	if(dummyMove){
		if(dummyMove->throwinvuln == 1 && throwInvuln <= 0) throwInvuln = 1;
		if(dummyMove->throwinvuln == 2) throwInvuln = 6;
	}
	if(!dryrun) cMove = dummyMove;
	else cMove = save;
}

void instance::pullVolition()
{
	int top = 0;
	for(int i = 0; i < momentumComplexity; i++)
		if(momentum[i].h > 0 && momentum[i].h > top){ 
			top = (short)momentum[i].h;
		}
	if(cMove->stop){
		if(currentFrame == 0){ 
			deltaX = 0; deltaY = 0;
			if(cMove->stop == 2)
				momentumComplexity = 0;
		}
	}
	if(freeze < 1){
		if(currentFrame < cMove->frames){
			SDL_Rect * temp = cMove->delta[currentFrame];
			for(int i = 0; i < cMove->deltaComplexity[currentFrame]; i++){
				if(temp[i].x || temp[i].y || temp[i].h){
					if(abs((short)temp[i].h) >= top || top == 0){
						addVector(temp[i]);
					}
				}
			}
		}
	}
}

void instance::addVector(SDL_Rect &v)
{
	int i;
	SDL_Rect * temp;
	temp = new SDL_Rect[momentumComplexity+1];
	for(i = 0; i < momentumComplexity; i++){
		temp[i].x = momentum[i].x;
		temp[i].y = momentum[i].y;
		temp[i].w = momentum[i].w;
		temp[i].h = momentum[i].h;
	}
	temp[i].x = v.x*facing;
	temp[i].y = v.y;
	temp[i].w = v.w;
	temp[i].h = v.h;
	if(momentumComplexity > 0) delete [] momentum;
	momentum = temp;
	momentumComplexity++;
}

void instance::removeVector(int n)
{
	if(momentumComplexity < 0 || !momentum) return;
	for(int i = n; i < momentumComplexity-1; i++){
		momentum[i].x = momentum[i+1].x;
		momentum[i].y = momentum[i+1].y;
		momentum[i].w = momentum[i+1].w;
		momentum[i].h = momentum[i+1].h;
	}
	momentumComplexity--;
}

void player::readEvent(SDL_Event & event, bool *& sAxis, bool *& posEdge, bool *& negEdge)
{
//	printf("Player %i read event of type %i:\n", ID, event.type);
	switch(event.type){
	case SDL_JOYAXISMOTION:
		for(int i = 0; i < 4; i++){
			if(input[i].type == SDL_JOYAXISMOTION){
				if(event.jaxis.which == input[i].jaxis.which && event.jaxis.axis == input[i].jaxis.axis && event.jaxis.value == input[i].jaxis.value)
					sAxis[i] = 1;
				if(event.jaxis.which == input[i].jaxis.which && event.jaxis.axis == input[i].jaxis.axis && event.jaxis.value == 0)
					sAxis[i] = 0;
			}
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(int i = 4; i < 9; i++){
			if(event.jbutton.which == input[i].jbutton.which && event.jbutton.button == input[i].jbutton.button && input[i].type == SDL_JOYBUTTONDOWN)
				posEdge[i-4] = 1;
		}
		break;
	case SDL_JOYBUTTONUP:
		for(int i = 4; i < 9; i++){
			if(event.jbutton.which == input[i].jbutton.which && event.jbutton.button == input[i].jbutton.button && input[i].type == SDL_JOYBUTTONDOWN)
				negEdge[i-4] = 1;
		}
		break;
	case SDL_KEYDOWN:
		for(int i = 0; i < 4; i++) {
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN) 
				sAxis[i] = 1;
		}
		for(int i = 4; i < 9; i++) {
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN)
				posEdge[i-4] = 1;
		}
		break;
	case SDL_KEYUP:
		for(int i = 0; i < 4; i++){
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN)
				sAxis[i] = 0;
		}
		for(int i = 4; i < 9; i++){
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN)
				negEdge[i-4] = 1;
		}
		break;
	}
}

void instance::connect(int combo, hStat & s)
{
//	printf("Hit with %s!\n", cMove->name);
	if(!s.ghostHit) freeze = s.stun/4+10;
	pick()->connect(cMove, bMove, sMove, s, connectFlag, currentFrame);
	if(bMove == cMove) bMove = NULL;
}

void player::connect(int combo, hStat & s)
{
//	printf("Hit with %s!\n", cMove->name);
	SDL_Rect v = {0, 0, 1, 0};
	if(combo < 2) v.x = 0;
	else if (!pick()->aerial) v.x = -combo;
	addVector(v);
	instance::connect(combo, s);
}

int player::takeHit(int combo, hStat & s)
{
	SDL_Rect v = {0, 0, 1, 0};
	if(s.damage > 0){
		if(combo >= s.damage) s.damage = 1;
		else s.damage -= combo; 
	}
	s.untech -= combo;
	if(slide) s.lift += 7 - s.lift/5;
	if(s.ghostHit) freeze = 0;
	else freeze = s.stun/4+10;
	particleType = pick()->takeHit(cMove, s, blockType, currentFrame, connectFlag, hitFlag);
	particleLife = 8;
	deltaX = 0; deltaY = 0; momentumComplexity = 0;
	if(pick()->aerial) v.y = -s.lift;
	else v.y = 0;
	if(pick()->aerial) v.x = -(s.push/5 + s.blowback);
	else v.x = -s.push;
	addVector(v);
	if(pick()->aerial && s.hover) hover = s.hover;
	else hover = 0;
	if(pick()->aerial && s.wallBounce) elasticX = true;
	else elasticX = false;
	if(pick()->aerial && s.floorBounce) elasticY = true;
	else elasticY = false;
	if(pick()->aerial && s.slide) slide = true;
	else slide = false;
	if(pick()->aerial && s.stick) stick = true;
	else stick = false;
	updateRects();
	if(s.ghostHit && combo < 1) return 0;
	else if(particleType == 1) return particleType;
	else return -1;
}

void instance::invertVectors(int operation)
{
	switch (operation){
	case 1:
		for(int i = 0; i < momentumComplexity; i++)
			momentum[i].x = -momentum[i].x;
		break;
	case 2:
		for(int i = 0; i < momentumComplexity; i++)
			momentum[i].y = -momentum[i].y;
		break;
	case 3:
		for(int i = 0; i < momentumComplexity; i++){
			momentum[i].x = -momentum[i].x;
			momentum[i].y = -momentum[i].y;
		}
		break;
	default:
		return;
		break;
	}
}

bool player::CHState()
{
	if(hitbox[0].w > 0) return true;
	else return cMove->CHState(currentFrame);
}

void instance::setPosition(int x, int y)
{
	posX = x;
	posY = y;
	updateRects();
}

void player::getThrown(action *toss, int x, int y)
{
	int xSign = x / abs(x);
	setPosition(toss->arbitraryPoll(27, currentFrame)*xSign + abs(x), toss->arbitraryPoll(26, currentFrame) + y);
	pick()->reel->init(1);
	cMove = pick()->reel;
	updateRects();
}

instance::~instance(){}
player::~player(){}

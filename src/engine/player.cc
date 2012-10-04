#include <stdio.h>
#include <iostream>
#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>
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
	counter = 0;
	cMove = NULL;
	bMove = NULL;
	sMove = NULL;
	freeze = 0;
	dead = false;
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

	/*Set up the sprite*/

	v = NULL;
	rounds = 0;
	instance::init();
}

void player::roundInit()
{
	instance::init();
	pick()->neutralize(cMove);
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
		}
		read.close();
		return 1;
	}
}

bool player::aerial()
{
	return pick()->aerial;
}

SDL_Event player::writeConfig(int i)
{
	SDL_Event temp; 
	/*Set up ALL the inputs*/

	/*Set up dummy event*/

	/*Flag for breaking the loop*/
	bool configFlag = 0;

	while(SDL_PollEvent(&temp));
	while (configFlag == 0){
		if (SDL_PollEvent(&temp)) {
			switch (temp.type) {
			case SDL_JOYAXISMOTION:
				if(temp.jaxis.axis < 6){
					for(int j = 0; j < 4; j++){
						if(temp.jaxis.value == 0 || abs(temp.jaxis.value) < abs(input[j].jaxis.value - 100))
							break;
						else {
							input[i] = temp;
							configFlag = 1;
						}
					}
				}
				break;
			case SDL_JOYBUTTONDOWN:
				input[i] = temp;
				configFlag = 1;
				break;
			case SDL_KEYDOWN:
				input[i] = temp;
				configFlag = 1;
				break;
			default: 
				break;
			}
		}
	}
	return temp;
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

void instance::enforceAttractor(attractor* p)
{
	SDL_Rect resultant;
	int midpoint;
	if(facing == 1) midpoint = posX + facing*cMove->collision[currentFrame].x + facing*collision.w/2;
	else midpoint = posX + facing*cMove->collision[currentFrame].x + facing*collision.w/2 + collision.w%2;
	resultant.x = p->x; resultant.y = p->y; resultant.w = 0; resultant.h = 0;
	if(!pick()->aerial) resultant.y = 0;
	int directionX = 0, directionY = 0;
	if(midpoint > p->posX) directionX = 1;
	else if(midpoint < p->posX) directionX = -1;
	if(collision.y + collision.h/2 > p->posY) directionY = 1;
	else if(collision.y + collision.h/2 < p->posY) directionY = -1;
	float totalDist = sqrt(pow(midpoint - p->posX, 2) + pow(collision.y + collision.h/2 - p->posY, 2));
	switch(p->type){
	case 0:
		break;
	case 1:
		resultant.x -= totalDist/p->radius;
		resultant.y -= totalDist/p->radius;
		resultant.x *= directionX;
		resultant.y *= directionY;
		break;
	case 2:
		for(int i = 1; i < totalDist/p->radius; i++){
			resultant.x /= 2;
			resultant.y /= 2;
		}
		resultant.x *= directionX;
		resultant.y *= directionY;
		break;
	case 3:
		if(totalDist > p->radius){
			resultant.x = 0;
			resultant.y = 0;
		}
		resultant.x *= directionX;
		resultant.y *= directionY;
		break;
	default:
		return;
	}
	addVector(resultant);
}

void instance::enforceGravity(int grav, int floor)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(collision.y > floor && pick()->aerial == 0){
		pick()->aerial = 1;
	}
	else if(pick()->aerial && !freeze){ 
		addVector(g);
	}
}

void player::enforceGravity(int grav, int floor)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(collision.y > floor && pick()->aerial == 0){
		pick()->aerial = 1;
	}
	else if(pick()->aerial && !freeze){ 
		if(hover > 0 && deltaY - 6 < 0) g.y = -deltaY;
		addVector(g);
	}
}

void player::checkBlocking()
{
	blockType = -pick()->checkBlocking(cMove, inputBuffer, connectFlag, hitFlag);
	updateRects();
}

void player::checkCorners(int floor, int left, int right)
{
	/*Offset variables. I could do these calculations on the fly, but it's easier this way.
	Essentially, this represents the offset between the sprite and the collision box, since
	even though we're *checking* collision, we're still *moving* spr*/
	int lOffset = posX - collision.x;
	int rOffset = posX - (collision.x + collision.w);

	/*Floor, or "Bottom corner"*/

	if (collision.y < floor){
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
				updateRects();
				deltaX = 0;
			}
			deltaY = 0;
		}
		posY = floor - cMove->collision[currentFrame].y;
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
		if(momentum[i].y > 0) removeVector(i);
	}
	pick()->land(cMove, currentFrame, connectFlag, hitFlag);
}

void instance::step()
{
	action * m = cMove;
	if(pick()->death(cMove, currentFrame, counter)) dead = true;
	if(m != cMove){
		currentFrame = 0;
		connectFlag = 0;
		hitFlag = 0;
	}
	if(posX > 3300 || posX < -100) dead = true;
	if(!freeze) counter++;
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

void instance::getMove(int down[5], bool up[5], SDL_Rect &p, bool dryrun)
{
	action * dummyMove, *save;
	dummyMove = cMove;
	save = cMove;
	int n = currentFrame;
	pick()->prepHooks(freeze, dummyMove, bMove, sMove, inputBuffer, down, up, p, currentFrame, connectFlag, hitFlag, dryrun);
	if(dummyMove){
		if(dummyMove->throwinvuln == 1 && throwInvuln <= 0) throwInvuln = 1;
		if(dummyMove->throwinvuln == 2) throwInvuln = 6;
	}
	if(!dryrun){ 
		cMove = dummyMove;
		if(currentFrame != n || cMove != save) cMove->playSound(ID);
	}
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
	if(cMove->displaceFrame == currentFrame) setPosition(posX + facing*cMove->displaceX, posY + cMove->displaceY);
	if(freeze < 1){
		if(currentFrame < cMove->frames){
			int complexity;
			SDL_Rect * temp; 
			cMove->pollDelta(temp, complexity, currentFrame);
			if(cMove->displaceFrame == currentFrame){ 
				setPosition(posX + facing*cMove->displace(posX, posY), posY);
			}
			for(int i = 0; i < complexity; i++){
				temp[i].x *= facing;
				if(temp[i].x || temp[i].y || temp[i].h){
					if(abs((short)temp[i].h) >= top || top == 0){
						addVector(temp[i]);
					}
				}
			}
			delete [] temp;
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
	temp[i].x = v.x;
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

void player::readEvent(SDL_Event & event, bool *& sAxis, int *& posEdge, bool *& negEdge)
{
//	printf("Player %i read event of type %i:\n", ID, event.type);
	switch(event.type){
	case SDL_JOYAXISMOTION:
		for(int i = 0; i < 4; i++){
			if(input[i].type == SDL_JOYAXISMOTION){
				if(event.jaxis.which == input[i].jaxis.which && event.jaxis.axis == input[i].jaxis.axis && event.jaxis.value == input[i].jaxis.value)
					sAxis[i] = 1;
				if(event.jaxis.which == input[i].jaxis.which && event.jaxis.axis == input[i].jaxis.axis && abs(event.jaxis.value) < abs(input[i].jaxis.value - 100))
					sAxis[i] = 0;
			}
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(int i = 0; i < 4; i++) {
			if(event.jbutton.which == input[i].jbutton.which && event.jbutton.button == input[i].jbutton.button && input[i].type == SDL_JOYBUTTONDOWN)
				sAxis[i] = 1;
		}
		for(int i = 4; i < 10; i++){
			if(event.jbutton.which == input[i].jbutton.which && event.jbutton.button == input[i].jbutton.button && input[i].type == SDL_JOYBUTTONDOWN)
				posEdge[i-4] = 1;
		}
		break;
	case SDL_JOYBUTTONUP:
		for(int i = 0; i < 4; i++) {
			if(event.jbutton.which == input[i].jbutton.which && event.jbutton.button == input[i].jbutton.button && input[i].type == SDL_JOYBUTTONDOWN)
				sAxis[i] = 0;
		}
		for(int i = 4; i < 10; i++){
			if(event.jbutton.which == input[i].jbutton.which && event.jbutton.button == input[i].jbutton.button && input[i].type == SDL_JOYBUTTONDOWN){
				negEdge[i-4] = 1;
				posEdge[i-4] = 0;
			}
		}
		break;
	case SDL_KEYDOWN:
		for(int i = 0; i < 4; i++) {
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN) 
				sAxis[i] = 1;
		}
		for(int i = 4; i < 10; i++) {
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN)
				posEdge[i-4] = 1;
		}
		break;
	case SDL_KEYUP:
		for(int i = 0; i < 4; i++){
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN)
				sAxis[i] = 0;
		}
		for(int i = 4; i < 10; i++){
			if(event.key.keysym.sym == input[i].key.keysym.sym && input[i].type == SDL_KEYDOWN){
				negEdge[i-4] = 1;
				posEdge[i-4] = 0;
			}
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

int instance::takeHit(int combo, hStat & s)
{
	return pick()->takeHit(cMove, s, blockType, currentFrame, connectFlag, hitFlag, particleType);
}

int player::takeHit(int combo, hStat & s)
{
	SDL_Rect v = {0, 0, 1, 0};
	action * temp = NULL;
	if(s.damage > 0){
		if(combo >= s.damage) s.damage = 1;
		else s.damage -= combo; 
	}
	s.untech -= combo;
	int f;
	if(slide) s.lift += 7 - s.lift/5;
	f = instance::takeHit(combo, s);
	if(s.ghostHit) freeze = 0;
	else freeze = f;
	if(particleType != 1){ 
		temp = cMove->blockSuccess();
	}
	if(temp && temp != cMove){
		combo = 0;
		bMove = temp;
		freeze = 0;
	} else {
		particleLife = 8;
		deltaX = 0; deltaY = 0; momentumComplexity = 0;
		if(pick()->aerial) v.y = s.lift;
		else v.y = 0;
		if(pick()->aerial) v.x = -(s.push/5 + s.blowback);
		else v.x = -s.push;
		v.x *= facing;
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
	}
	if(cMove == pick()->die){ 
		bMove = NULL;
		currentFrame = 0;
		connectFlag = 0;
		hitFlag = 0;
	}
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
	hStat dummy;
	dummy.stun = 1;
	dummy.ghostHit = 1;
	setPosition(toss->arbitraryPoll(27, currentFrame)*xSign + abs(x), toss->arbitraryPoll(26, currentFrame) + y);
	pick()->neutralize(cMove);
	pick()->takeHit(cMove, dummy, 0, currentFrame, connectFlag, hitFlag, particleType);
	updateRects();
}

instance::~instance(){}
player::~player(){}

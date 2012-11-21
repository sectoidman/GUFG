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
	meter = NULL;
	init();
}

player::player(int id)
{
	ID = id;
	meter = NULL;
	init();
	wins = 0;
}

instance::instance()
{
	meter = NULL;
	init();
}

instance::instance(avatar * f)
{
	v = f;
	init();
	meter = pick()->generateMeter();
	pick()->init(meter);
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
	aerial = false;
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

	v = NULL;
	rounds = 0;
	inputComplexity = 0;
	input = NULL;
	instance::init();
}

void player::roundInit()
{
	instance::init();
	pick()->neutralize(cMove, aerial, meter);
	if(v) pick()->init(meter);
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
	sprintf(fname, ".config/p%i.conf", ID);
	std::ifstream read;
	int i = 0;
	read.open(fname);
	if(read.fail()) {
		read.close();
		return 0;
	} else {
		char * token;
		char buffer[30];
		do{
			addInput();
			read.get(buffer, 100, ':'); read.ignore(); 
			input[i]->trigger.type = atoi(buffer);
			read.ignore();
			read.get(buffer, 100, '\n'); read.ignore();
			switch (input[i]->trigger.type){
			case SDL_JOYAXISMOTION:
				token = strtok(buffer, " \n");
				input[i]->trigger.jaxis.which = atoi(token);
				token = strtok(NULL, " \n");
				input[i]->trigger.jaxis.axis = atoi(token);
				token = strtok(NULL, " \n");
				input[i]->trigger.jaxis.value = atoi(token);
				break;
			case SDL_JOYBUTTONDOWN:
				token = strtok(buffer, " \n");
				input[i]->trigger.jbutton.which = atoi(token);
				token = strtok(NULL, " \n");
				input[i]->trigger.jbutton.button = atoi(token);
				break;
			case SDL_KEYDOWN:
				token = strtok(buffer, " \n");	
				input[i]->trigger.key.keysym.sym = (SDLKey)atoi(token);
				break;
			default:
				break;
			}
			token = strtok(NULL, " \n");
			input[i]->effect.i = atoi(token);
			i++;
			read.peek();
		} while(!read.eof());
		read.close();
		return 1;
	}
}

keySetting::keySetting()
{
	trigger.type = 0;
	effect.i = 0;
}

void player::addInput()
{
	keySetting ** temp;
	int i;
	temp = new keySetting*[inputComplexity + 1];
	for(i = 0; i < inputComplexity; i++){
		temp[i] = input[i];
	}
	temp[i] = new keySetting;
	input = temp;
	inputComplexity++;
}

void player::cullInput(int q)
{
	if(input[q] != NULL) delete input[q];
	for(int i = q; i < inputComplexity - 1; i++){
		input[i] = input[i + 1];
	}
	inputComplexity--;
}

/*This function wraps around the normal setKey function, handling input by itself*/
void player::setKey(int effect) 
{
	SDL_Event temp;
	bool configFlag = 0;

	while(SDL_PollEvent(&temp));
	while (configFlag == 0){
		if (SDL_PollEvent(&temp)) {
			configFlag = setKey(effect, temp);
		}
	}
}

/*This function will only set buttons to the existing stick*/
bool player::setKey(SDL_Event temp, int effect)
{
	int type = -10, controller = -10;
	for(int i = 0; i < inputComplexity; i++){
		if(input[i]->effect.i & 1){ //Compares to the "up" direction
			type = input[i]->trigger.type;
			switch(input[i]->trigger.type){
			case SDL_JOYAXISMOTION:
				controller = input[i]->trigger.jaxis.which;
				break;
			case SDL_JOYBUTTONDOWN:
				controller = input[i]->trigger.jbutton.which;
				break;
			}
			i = inputComplexity;
		}
	}
	if(temp.type != type) return 0;
	switch(temp.type){
		case SDL_JOYAXISMOTION:
			if(temp.jaxis.which != controller) return 0;
		case SDL_JOYBUTTONDOWN:
			if(temp.jbutton.which != controller) return 0;
	}
	return setKey(effect, temp);
}

/*This function takes an event and a desired effect and maps them in the keysetting array*/
bool player::setKey(int effect, SDL_Event temp)
{
	int workingIndex = -1;
	switch (temp.type){
	case SDL_JOYAXISMOTION:
		if(temp.jaxis.axis < 6 && temp.jaxis.value != 0){
			for(int i = 0; i < inputComplexity; i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jaxis.which == temp.jaxis.which &&
				   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
				   input[i]->trigger.jaxis.value == temp.jaxis.value){
					workingIndex = i; 
					i = inputComplexity;
				}
			}
			if(workingIndex < 0){
				addInput();
				workingIndex = inputComplexity - 1;
				input[workingIndex]->trigger.type = temp.type;
				input[workingIndex]->trigger.jaxis.which = temp.jaxis.which;
				input[workingIndex]->trigger.jaxis.axis = temp.jaxis.axis;
				input[workingIndex]->trigger.jaxis.value = temp.jaxis.value;
			}
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(int i = 0; i < inputComplexity; i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.jbutton.which == temp.jbutton.which &&
			   input[i]->trigger.jbutton.button == temp.jbutton.button){
				workingIndex = i;
				i = inputComplexity;
			}
		}
		if(workingIndex < 0){
			addInput();
			workingIndex = inputComplexity - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jbutton.which = temp.jbutton.which;
			input[workingIndex]->trigger.jbutton.button = temp.jbutton.button;
		}
		break;
	case SDL_KEYDOWN:
		for(int i = 0; i < inputComplexity; i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				workingIndex = i;
				i = inputComplexity;
			}
		}
		if(workingIndex < 0){
			addInput();
			workingIndex = inputComplexity - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.key.keysym.sym = temp.key.keysym.sym;
		}
		break;
	default:
		break;
	}
	if(workingIndex > -1){ 
		input[workingIndex]->effect.i += effect;
		return 1;
	} else return 0;
}

void player::writeConfig()
{
	char fname[30];
	sprintf(fname, ".config/p%i.conf", ID);
	std::ofstream write;
	write.open(fname);
	for(int i = 0; i < inputComplexity; i++){
		switch(input[i]->trigger.type){
		case SDL_JOYAXISMOTION:
			if(input[i]->trigger.jaxis.value != 0 && input[i]->trigger.jaxis.axis < 6){
				write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.jaxis.which << " " << (int)input[i]->trigger.jaxis.axis << " " << (int)input[i]->trigger.jaxis.value;
			}
			break;
		case SDL_JOYBUTTONDOWN:
			write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.jbutton.which << " " << (int)input[i]->trigger.jbutton.button;
			break;
		case SDL_KEYDOWN:
			write << (int)input[i]->trigger.type << " : " << (int)input[i]->trigger.key.keysym.sym;
			break;
		}
		write << " " << input[i]->effect.i << "\n";
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
	meter = pick()->generateMeter();
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
	if(!aerial) resultant.y = 0;
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

	if(collision.y > floor && aerial == 0){
		aerial = 1;
		sMove = NULL;
	}
	else if(aerial && !freeze){ 
		addVector(g);
	}
}

void player::enforceGravity(int grav, int floor)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(collision.y > floor && aerial == 0){
		aerial = 1;
		sMove = NULL;
	}
	else if(aerial && !freeze){ 
		if(hover > 0 && deltaY - 6 < 0) g.y = -deltaY;
		addVector(g);
	}
}

void player::checkBlocking()
{
	blockType = -pick()->checkBlocking(cMove, inputBuffer, connectFlag, hitFlag, aerial);
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
				aerial = 1;
			} else {
				deltaX = 0;
				slide = 0;
			}
		} else {
			if(aerial == 1){
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
	pick()->land(cMove, currentFrame, connectFlag, hitFlag, meter);
	sMove = NULL;
	aerial = false;
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
	pick()->step(cMove, currentFrame, freeze, meter);
	if(cMove && currentFrame >= cMove->frames){
		if(cMove->modifier && cMove->basis){ 
			currentFrame = cMove->currentFrame;
			connectFlag = cMove->connectFlag;
			hitFlag = cMove->hitFlag;
			cMove = cMove->basis;
		} else {
			cMove = cMove->next;
			currentFrame = 0;
			connectFlag = 0;
			hitFlag = 0;
		}
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

int instance::passSignal(int sig)
{
	switch (sig){
	case 1:
		action * a; 
		a = pick()->moveSignal(counter);
		if(a != NULL){
			cMove = a;
			currentFrame = 0;
			connectFlag = 0;
			hitFlag = 0;
			return 1;
		} else return 0;
		break;
	default:
		return pick()->passSignal(sig);
		break;
	}
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
	pick()->prepHooks(freeze, dummyMove, bMove, sMove, inputBuffer, down, up, p, currentFrame, connectFlag, hitFlag, dryrun, aerial, meter);
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
	int dx = cMove->displace(posX, posY, currentFrame);
	setPosition(posX + facing*dx, posY);
	if(freeze < 1){
		if(currentFrame < cMove->frames){
			int complexity;
			SDL_Rect * temp; 
			cMove->pollDelta(temp, complexity, currentFrame);
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
	int value = -1;
	bool pos;
	for(int i = 0; i < inputComplexity; i++){
		switch(event.type){
		case SDL_JOYAXISMOTION:
			if(input[i]->trigger.type == SDL_JOYAXISMOTION){
				if(event.jaxis.which == input[i]->trigger.jaxis.which && event.jaxis.axis == input[i]->trigger.jaxis.axis){
					if(event.jaxis.value == input[i]->trigger.jaxis.value){
						value = i;
						pos = 1;
						i = inputComplexity;
					} else if(event.jaxis.value == 0) {
						value = i;
						pos = 0;
						i = inputComplexity;
					}
				}
			}
			break;
		case SDL_JOYBUTTONDOWN:
			if(input[i]->trigger.type == SDL_JOYBUTTONDOWN){
				if(event.jbutton.which == input[i]->trigger.jbutton.which && event.jbutton.button == input[i]->trigger.jbutton.button){
					value = i;
					pos = 1;
					i = inputComplexity;
				}
			}
			break;
		case SDL_JOYBUTTONUP:
			if(input[i]->trigger.type == SDL_JOYBUTTONDOWN){
				if(event.jbutton.which == input[i]->trigger.jbutton.which && event.jbutton.button == input[i]->trigger.jbutton.button){
					value = i;
					pos = 0;
					i = inputComplexity;
				}
			}
			break;
		case SDL_KEYDOWN:
			if(input[i]->trigger.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == input[i]->trigger.key.keysym.sym){
					value = i;
					pos = 1;
					i = inputComplexity;
				}
			}
			break;
		case SDL_KEYUP:
			if(input[i]->trigger.type == SDL_KEYDOWN){
				if(event.key.keysym.sym == input[i]->trigger.key.keysym.sym){
					value = i;
					pos = 0;
					i = inputComplexity;
				}
			}
		break;
		}
	}
	if(value > -1){
		for(int i = 0; i < 6; i++){
			if(i < 4){
				if(input[value]->effect.i & (1 << i)){ 
					sAxis[i] = pos;
					if(!pos){
						switch(i){
						case 0:
							sAxis[1] = 0;
						case 1:
							sAxis[0] = 0;
							break;
						case 2:
							sAxis[3] = 0;
						case 3:
							sAxis[2] = 0;
							break;
						}
					}
				}
			}
			if(input[value]->effect.i & (1 << (i + 4))){
				posEdge[i] = pos;
				negEdge[i] = !pos;
			}
		}
	}
}

void instance::connect(int combo, hStat & s)
{
	if(s.pause < 0){
		if(!s.ghostHit) freeze = s.stun/4+10;
	} else freeze = s.pause;
	pick()->connect(cMove, bMove, sMove, s, connectFlag, currentFrame, meter);
	if(bMove == cMove) bMove = NULL;
}

int instance::takeHit(int combo, hStat & s, SDL_Rect &p)
{
	return pick()->takeHit(cMove, s, blockType, currentFrame, connectFlag, hitFlag, particleType, aerial, meter);
}

int player::takeHit(int combo, hStat & s, SDL_Rect &p)
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
	f = instance::takeHit(combo, s, p);
	freeze = f;
	if(particleType != 1){
		temp = cMove->blockSuccess();
	}
	if(temp && temp != cMove && temp->check(p, meter)){
		combo = 0;
		bMove = temp;
		freeze = 0;
	} else {
		particleLife = 8;
		deltaX = 0; deltaY = 0; momentumComplexity = 0;
		if(aerial) v.y = s.lift;
		else v.y = 0;
		if(aerial) v.x = -(s.push/5 + s.blowback);
		else v.x = -s.push;
		v.x *= facing;
		addVector(v);
		if(aerial && s.hover) hover = s.hover;
		else hover = 0;
		if(aerial && s.wallBounce) elasticX = true;
		else elasticX = false;
		if(aerial && s.floorBounce) elasticY = true;
		else elasticY = false;
		if(aerial && s.slide) slide = true;
		else slide = false;
		if(aerial && s.stick) stick = true;
		else stick = false;
	}
	if(cMove == pick()->die){ 
		bMove = NULL;
		currentFrame = 0;
		connectFlag = 0;
		hitFlag = 0;
	}
	updateRects();
	if(s.ghostHit) return 0;
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
	pick()->neutralize(cMove, aerial, meter);
	pick()->takeHit(cMove, dummy, 0, currentFrame, connectFlag, hitFlag, particleType, aerial, meter);
	updateRects();
}

instance::~instance()
{
	if(meter) delete [] meter;
}

player::~player(){}

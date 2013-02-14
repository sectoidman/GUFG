#include <stdio.h>
#include <iostream>
#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include "player.h"
#include "analytics.h"
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
	meter = pick()->generateMeter();
	pick()->init(meter);
	init();
}

void instance::init()
{
	current.deltaX = 0;
	current.deltaY = 0;
	current.lCorner = 0;
	current.rCorner = 0;
	current.frame = 0;
	current.connect = 0;
	current.hit = 0;
	counter = 0;
	current.move = NULL;
	current.bufferedMove = NULL;
	current.reversal = NULL;
	current.freeze = 0;
	current.aerial = false;
	current.dead = false;
	for(int i = 0; i < 30; i++) inputBuffer[i] = 5;
}

bool instance::acceptTarget(instance * m)
{
	if(m->ID == ID) return 0;
	else if(m->current.move->hittable) return 1;
	else return m->pick()->acceptTarget(current.move, current.frame);
}

void player::init()
{
	/*Initialize input containers*/
	for(int i = 0; i < 30; i++)
		inputBuffer[i] = 5;

	/*Yeah yeah, I know, char* to literal conversion. I'm lazy right now. Will fix later. Maybe with cstring*/
	inputName.push_back("Up");
	inputName.push_back("Down");
	inputName.push_back("Left");
	inputName.push_back("Right");
	inputName.push_back("A");
	inputName.push_back("B");
	inputName.push_back("C");
	inputName.push_back("D");
	inputName.push_back("E");
	inputName.push_back("Start");

	currentMacro = NULL;
	record = NULL;
	v = NULL;
	rounds = 0;
	instance::init();
}

void player::roundInit()
{
	char buffer[200];
	instance::init();
	pick()->neutralize(current.move, current.aerial, meter);
	if(v) pick()->init(meter);
	if(record){
		sprintf(buffer, "%i-%s.sh", ID, pick()->name);
		record->write(buffer);
		delete record;
		record = NULL;
	}
	elasticX = 0;
	elasticY = 0;
	blockType = 0;
	iterator = 0;
	slide = 0;
	stick = 0;
	hover = 0;
	current.throwInvuln = 0;
	particleLife = 0;
	particleType = 0;
	search = 0;
	if(ID == 1){ 
		current.facing = 1;
		current.posX = 1400;
	} else {
		current.facing = -1;
		current.posX = 1800;
	}
	updateRects();
}

bool controller::readConfig(int ID)
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
			input.push_back(new keySetting);
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
			input[i]->effect = atoi(token);
			i++;
			read.peek();
		} while(!read.eof());
		read.close();
		return 1;
	}
}

/*This function wraps around the normal setKey function, handling input by itself*/
void player::setKey(int effect) 
{
	SDL_Event temp;
	bool configFlag = 0;

	while(SDL_PollEvent(&temp));
	while (!configFlag){
		if (SDL_PollEvent(&temp)) {
			configFlag = setKey(effect, temp);
		}
	}
}

/*This function takes an event and a desired effect and maps them in the keySetting array*/
bool player::setKey(int effect, SDL_Event temp)
{
	int workingIndex = -1;
	switch (temp.type){
	case SDL_JOYAXISMOTION:
		if(temp.jaxis.axis < 6 && temp.jaxis.value != 0){
			for(unsigned int i = 0; i < input.size(); i++){
				if(input[i]->trigger.type == temp.type &&
				   input[i]->trigger.jaxis.which == temp.jaxis.which &&
				   input[i]->trigger.jaxis.axis == temp.jaxis.axis &&
				   input[i]->trigger.jaxis.value == temp.jaxis.value){
					workingIndex = i; 
					i = input.size();
				}
			}
			if(workingIndex < 0){
				input.push_back(new keySetting);
				workingIndex = input.size() - 1;
				input[workingIndex]->trigger.type = temp.type;
				input[workingIndex]->trigger.jaxis.which = temp.jaxis.which;
				input[workingIndex]->trigger.jaxis.axis = temp.jaxis.axis;
				input[workingIndex]->trigger.jaxis.value = temp.jaxis.value;
			}
		}
		break;
	case SDL_JOYBUTTONDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.jbutton.which == temp.jbutton.which &&
			   input[i]->trigger.jbutton.button == temp.jbutton.button){
				workingIndex = i;
				i = input.size();
			}
		}
		if(workingIndex < 0){
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.jbutton.which = temp.jbutton.which;
			input[workingIndex]->trigger.jbutton.button = temp.jbutton.button;
		}
		break;
	case SDL_KEYDOWN:
		for(unsigned int i = 0; i < input.size(); i++){
			if(input[i]->trigger.type == temp.type &&
			   input[i]->trigger.key.keysym.sym == temp.key.keysym.sym){
				workingIndex = i;
				i = input.size();
			}
		}
		if(workingIndex < 0){
			input.push_back(new keySetting);
			workingIndex = input.size() - 1;
			input[workingIndex]->trigger.type = temp.type;
			input[workingIndex]->trigger.key.keysym.sym = temp.key.keysym.sym;
		}
		break;
	default:
		break;
	}
	if(workingIndex > -1){
		if(input[workingIndex]->effect & effect);
		else input[workingIndex]->effect += effect;
		return 1;
	} else return 0;
}

void controller::writeConfig(int ID)
{
	char fname[30];
	sprintf(fname, ".config/p%i.conf", ID);
	std::ofstream write;
	write.open(fname);
	for(unsigned int i = 0; i < input.size(); i++){
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
		write << " " << input[i]->effect << "\n";
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
	iterator = 0;
	meter = pick()->generateMeter();
}

void player::readScripts()
{
	char buffer[200];
	char *token;
	int tempIterator;
	sprintf(buffer, ".config/%src", v->name);
	macro.clear();
	std::ifstream read;
	read.open(buffer);
	if(read.fail()) return;
	while(!read.eof()){
		tempIterator = 0;
		read.get(buffer, 200, '\n');
		token = strtok(buffer, " \n");
		if(!token) return;
		macro.push_back(new script(token));
		if(token = strtok(NULL, " \n")){
			for(unsigned int i = 0; i < strlen(token); i++){
				if(token[i] >= 'A' && token[i] <= 'E'){
					tempIterator += 16 << (token[i] - 'A');
				}
			}
		}
		if(!macro[macro.size()-1]->test()) macro.pop_back();
		else pattern.push_back(tempIterator);
	}
}

void instance::updateRects()
{
	if(current.move != NULL) {
		current.move->pollRects(current.frame, current.connect, collision, hitreg, hitbox);
		for(unsigned int i = 0; i < hitbox.size(); i++){
			if(current.facing == -1) hitbox[i].x = current.posX - hitbox[i].x - hitbox[i].w;
			else hitbox[i].x += current.posX;
			hitbox[i].y += current.posY;
		}
		for(unsigned int i = 0; i < hitreg.size(); i++){
			if(current.facing == -1) hitreg[i].x = current.posX - hitreg[i].x - hitreg[i].w;
			else hitreg[i].x += current.posX;
			hitreg[i].y += current.posY;
		}
		if(current.facing == -1) collision.x = current.posX - collision.x - collision.w;
		else collision.x += current.posX;
		collision.y += current.posY;
	}
}

void instance::combineDelta()
{
	for(unsigned int i = 0; i < momentum.size(); i++){
		current.deltaX += momentum[i].x;
		current.deltaY += momentum[i].y;

		if(momentum[i].w <= 0) {
			momentum.erase(momentum.begin()+i);
			i--;
		}
		else momentum[i].w--;
	}
	current.posX += current.deltaX;
	current.posY += current.deltaY;

	updateRects();
}

void instance::enforceAttractor(attractor* p)
{
	SDL_Rect resultant;
	int midpoint, xDist, yDist;
	if(current.facing == 1) midpoint = current.posX + current.facing*current.move->collision[current.frame].x + current.facing*collision.w/2;
	else midpoint = current.posX + current.facing*current.move->collision[current.frame].x + current.facing*collision.w/2 + collision.w%2;
	resultant.x = p->x; resultant.y = p->y; resultant.w = 0; resultant.h = 0;
	if(!current.aerial) resultant.y = 0;
	int directionX = 0, directionY = 0;
	if(midpoint > p->posX) directionX = 1;
	else if(midpoint < p->posX) directionX = -1;
	if(collision.y + collision.h/2 > p->posY) directionY = 1;
	else if(collision.y + collision.h/2 < p->posY) directionY = -1;
	xDist = abs(midpoint - p->posX);
	yDist = abs(collision.y + collision.h/2 - p->posY);
	float totalDist = sqrt(pow(xDist, 2) + pow(yDist, 2));
	if(totalDist < p->eventHorizon && p->eventHorizon > 0){
		resultant.x = 0;
		resultant.y = 0;
		current.deltaX = 0; current.deltaY = 0;
		momentum.clear();
	} else {
		switch(p->type){
		case 0:
			break;
		case 1:
			resultant.x -= yDist/p->radius;
			resultant.y -= yDist/p->radius;
			resultant.x *= directionX;
			resultant.y *= directionY;
			break;
		case 2:
			for(int i = 1; i < xDist/p->radius; i++)
				resultant.x /= 2;
			for(int i = 1; i < yDist/p->radius; i++)
				resultant.y /= 2;
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
	}
	momentum.push_back(resultant);
}

void instance::enforceGravity(int grav, int floor)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(collision.y > floor && current.aerial == 0){
		current.aerial = 1;
		current.reversal = NULL;
	}
	else if(current.aerial && !current.freeze){ 
		momentum.push_back(g);
	}
}

void player::enforceGravity(int grav, int floor)
{
	SDL_Rect g; g.x = 0; g.y = grav; g.w = 0; g.h = 0;

	if(collision.y > floor && current.aerial == 0){
		current.aerial = 1;
		current.reversal = NULL;
	}
	else if(current.aerial && !current.freeze){ 
		if(hover > 0 && current.deltaY - 6 < 0) g.y = -current.deltaY;
		momentum.push_back(g);
	}
}

void player::checkBlocking()
{
	blockType = -pick()->checkBlocking(current.move, inputBuffer, current.connect, current.hit, current.aerial);
	updateRects();
}

void player::enforceFloor(int floor)
{
	/*Floor, or "Bottom corner"*/

	if (collision.y < floor){
		if(elasticY){
			current.deltaY = -current.deltaY;
			elasticY = false;
		} else if (slide) {
			current.deltaY = 0;
			if(current.move == pick()->untech || current.move == pick()->die){ 
				if(current.deltaX < 0) current.deltaX++;
				else if(current.deltaX > 0) current.deltaX--;
				current.aerial = 1;
			} else {
				current.deltaX = 0;
				slide = 0;
			}
		} else {
			if(current.aerial == 1){
				land();
				updateRects();
				current.deltaX = 0;
			}
			current.deltaY = 0;
		}
		current.posY = floor - current.move->collision[current.frame].y;
	}
	updateRects();
}

void player::checkCorners(int left, int right)
{
	/*Walls, or "Left and Right" corners
	This not only keeps the characters within the stage boundaries, but flags them as "in the corner"
	so we can specialcase collision checks for when one player is in the corner.*/

	/*Offset variables. I could do these calculations on the fly, but it's easier this way.
	Essentially, this represents the offset between the sprite and the collision box, since
	even though we're *checking* collision, we're still *moving* spr*/
	int lOffset = current.posX - collision.x;
	int rOffset = current.posX - (collision.x + collision.w);
	if(collision.x <= left){
		if(elasticX){
			current.lCorner = 0;
			if(current.deltaX < 0) current.deltaX = -current.deltaX;
			elasticX = false;
		}
		if(collision.x <= 50){
			if(current.facing == 1) current.lCorner = 1;
			if (stick) {
				if(current.move == pick()->untech || current.move == pick()->die){
					current.deltaX = 0;
					current.deltaY = 0;
					momentum.clear();
				} else stick = 0;
			}
		}
		if(collision.x < left)
			current.posX = left + lOffset;
	} else current.lCorner = 0;
	if(collision.x + collision.w >= right){
		if(elasticX){
			current.rCorner = 0;
			if(current.deltaX > 0) current.deltaX = -current.deltaX; 
			elasticX = false;
		}
		if(collision.x + collision.w >= 3150){ 
			if(current.facing == -1) current.rCorner = 1;
			if (stick) {
				if(current.move == pick()->untech || current.move == pick()->die){
					current.deltaX = 0;
					current.deltaY = 0;
					momentum.clear();
				} else stick = 0;
			}
		}
		if(collision.x + collision.w > right){
			current.posX = right + rOffset;
		}
	} else current.rCorner = 0;
	updateRects(); //Update rectangles or the next collision check will be wrong.
}

void player::land()
{
	for(unsigned int i = 0; i < momentum.size(); i++){
		if(momentum[i].y > 0) momentum.erase(momentum.begin()+i);
	}
	pick()->land(current.move, current.frame, current.connect, current.hit, meter);
	current.reversal = NULL;
	current.aerial = false;
}

void instance::follow(instance *other){
	if(current.frame >= current.move->followStart && current.frame <= current.move->followEnd){
		if(abs(other->current.posX - current.posX) > current.move->followXRate){ 
			if(other->current.posX > current.posX) current.posX += current.move->followXRate;
			else if(other->current.posX < current.posX) current.posX -= current.move->followXRate;
		} else current.posX += other->current.posX - current.posX;
		if(abs(other->current.posY - current.posY) > current.move->followYRate){ 
			if(other->current.posY > current.posY) current.posY += current.move->followYRate;
			else if(other->current.posY < current.posY) current.posY -= current.move->followYRate;
		} else current.posY += other->current.posY - current.posY;
	}
}

void instance::step()
{
	action * m = current.move;
	if(pick()->death(current.move, current.frame, counter)) current.dead = true;
	if(m != current.move){
		current.frame = 0;
		current.connect = 0;
		current.hit = 0;
	}
	if(current.posX > 3300 || current.posX < -100) current.dead = true;
	if(!current.freeze) counter++;
	pick()->step(current, meter);
	if(current.move && current.frame >= current.move->frames){
		if(current.move->modifier && current.move->basis){ 
			current.frame = current.move->currentFrame;
			current.connect = current.move->connectFlag;
			current.hit = current.move->hitFlag;
			current.move = current.move->basis;
		} else {
			if(current.move->next) current.move = current.move->next;
			else pick()->neutralize(current.move, current.aerial, meter);
			current.frame = 0;
			current.connect = 0;
			current.hit = 0;
		}
	}
}

void instance::neutralize()
{
	pick()->neutralize(current.move, current.aerial, meter);
}

void instance::flip()
{
		if(current.facing == -1){
			current.posX += collision.x - (current.posX + (current.posX - collision.x - collision.w));
			current.facing = 1;
		} else { 
			current.posX += (collision.w + collision.x) - current.posX*2 + collision.x;
			current.facing = -1;
		}
}

void instance::checkFacing(instance * other)
{
	int comparison, midpoint;
	midpoint = collision.x + collision.w/2;
	comparison = other->collision.x + other->collision.w/2;

	if(other->current.posX < current.posX) comparison += collision.w % 2; 
	else midpoint += collision.w % 2;

	if (current.lCorner) current.facing = 1;
	else if (current.rCorner) current.facing = -1;
	else if (midpoint < comparison){
		if(current.facing == -1) flip();
	} else if (midpoint > comparison){
		if(current.facing == 1) flip();
	}
	updateRects();
}

int instance::dragBG(int left, int right)
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
			current.move = a;
			current.frame = 0;
			current.connect = 0;
			current.hit = 0;
			return 1;
		} else return 0;
		break;
	default:
		return pick()->passSignal(sig);
		break;
	}
}

void instance::pushInput(std::vector<bool> axis)
{
	int temp = 5 + axis[0]*3 - axis[1]*3 - axis[2]*current.facing + axis[3]*current.facing;
	inputBuffer[0] = temp;

	for(int i = 29; i > 0; i--){
		inputBuffer[i] = inputBuffer[i-1];
	}
}

void instance::getMove(std::vector<int> down, std::vector<bool> up, SDL_Rect &p, bool dryrun)
{
	action * dummyMove, *save;
	dummyMove = current.move;
	save = current.move;
	int n = current.frame;
	pick()->prepHooks(current, dummyMove, inputBuffer, down, up, p, dryrun, meter);
	if(dummyMove){
		if(dummyMove->throwinvuln == 1 && current.throwInvuln <= 0) current.throwInvuln = 1;
		if(dummyMove->throwinvuln == 2) current.throwInvuln = 6;
	}
	if(!dryrun){ 
		current.move = dummyMove;
		if(current.frame != n || current.move != save) current.move->playSound(ID);
	}
	else current.move = save;
}

void instance::pullVolition()
{
	int top = 0;
	for(unsigned int i = 0; i < momentum.size(); i++)
		if(momentum[i].h > 0 && momentum[i].h > top){ 
			top = (short)momentum[i].h;
		}
	if(current.move->stop){
		if(current.frame == 0){
			if(current.move->stop & 1){ 
				current.deltaX = 0; current.deltaY = 0;
			}
			if(current.move->stop & 2)
				momentum.clear();
		}
	}
	int dx = current.move->displace(current.posX, current.posY, current.frame);
	setPosition(current.posX + current.facing*dx, current.posY);
	if(current.freeze < 1){
		if(current.frame < current.move->frames){
			std::vector<SDL_Rect> temp = current.move->pollDelta(current.frame);
			for(unsigned int i = 0; i < temp.size(); i++){
				temp[i].x *= current.facing;
				if(temp[i].x || temp[i].y || temp[i].h){
					if(abs((short)temp[i].h) >= top || top == 0){
						momentum.push_back(temp[i]);
					}
				}
			}
		}
	}
}

int instance::middle()
{
	if(current.facing == 1) return collision.x + collision.w / 2;
	else return collision.x + collision.w / 2 + collision.w % 2;
}

void player::macroCheck(SDL_Event &event)
{
	char buffer[200];
	int effect = tap(event);
	if(effect > 0){
		currentMacro = NULL;
		if(effect & 512) search = true;
	} else if (effect < 0) {
		if(abs(effect) & 512){
			search = false;
			if(!currentMacro){
				if(!record){
					record = new script();
					record->init(1);
				} else {
					sprintf(buffer, "%i-%s.sh", ID, v->name);
					record->write(buffer);
					delete record;
					record = NULL;
				}
			}
		}
	}
}

void controller::readEvent(SDL_Event & event, frame &t)
{
	int effect = tap(event);
	if(effect != 0){
		for(int i = 0; i < 4; i++){
			if(abs(effect) & (1 << i)){ 
				t.axis[i] = (effect > 0);
			}
		}
		for(unsigned int i = 0; i < t.pos.size(); i++){
			if(abs(effect) & (1 << (i + 4))){
				t.pos[i] = (effect > 0);
				t.neg[i] = (effect < 0);
			}
		}
	}
}

void instance::connect(int combo, hStat & s)
{
	if(s.pause < 0){
		if(!s.ghostHit) current.freeze = s.stun/4+10;
	} else current.freeze = s.pause;
	pick()->connect(current, meter);
	if(current.bufferedMove == current.move) current.bufferedMove = NULL;
}

int instance::takeHit(int combo, hStat & s, SDL_Rect &p)
{
	if(s.turnsProjectile){
		if(pick()->turn(ID)){ 
			flip();
			invertVectors(1);
			current.deltaX = -current.deltaX;
			current.freeze = 0;
		}
	}
	return pick()->takeHit(current, s, blockType, particleType, meter);
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
	current.freeze = f;
	if(particleType != 1){
		temp = current.move->blockSuccess();
	}
	if(temp && temp != current.move && temp->check(p, meter)){
		combo = 0;
		current.bufferedMove = temp;
		current.freeze = 0;
	} else {
		particleLife = 8;
		current.deltaX /= 6;
		if(current.deltaY < 0) current.deltaY /= 60; 
		else current.deltaY /= 6;
		momentum.clear();
		if(current.aerial) v.y = s.lift;
		else v.y = 0;
		if(current.aerial) v.x = -(s.push/5 + s.blowback);
		else v.x = -s.push;
		v.x *= current.facing;
		momentum.push_back(v);
		if(particleType == -1){ 
			v.x /= 5;
			v.y /= 5;
		}
		if(particleType == -2){
			v.x = 0;
			v.y = 0;
		}
		if(current.aerial && s.hover) hover = s.hover;
		else hover = 0;
		if(current.aerial && s.wallBounce) elasticX = true;
		else elasticX = false;
		if(current.aerial && s.floorBounce) elasticY = true;
		else elasticY = false;
		if(current.aerial && s.slide) slide = true;
		else slide = false;
		if(current.aerial && s.stick) stick = true;
		else stick = false;
	}
	if(current.move == pick()->die){ 
		current.bufferedMove = NULL;
		current.frame = 0;
		current.connect = 0;
		current.hit = 0;
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
		for(unsigned int i = 0; i < momentum.size(); i++)
			momentum[i].x = -momentum[i].x;
		break;
	case 2:
		for(unsigned int i = 0; i < momentum.size(); i++)
			momentum[i].y = -momentum[i].y;
		break;
	case 3:
		for(unsigned int i = 0; i < momentum.size(); i++){
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
	if(!hitbox.empty()) return true;
	else return current.move->CHState(current.frame);
}

void instance::setPosition(int x, int y)
{
	current.posX = x;
	current.posY = y;
	updateRects();
}

void player::getThrown(action *toss, int x, int y)
{
	int xSign = x / abs(x);
	hStat dummy;
	dummy.stun = 1;
	dummy.ghostHit = 1;
	setPosition(toss->arbitraryPoll(27, current.frame)*xSign + abs(x), toss->arbitraryPoll(26, current.frame) + y);
	pick()->neutralize(current.move, current.aerial, meter);
	pick()->takeHit(current, dummy, 0, particleType, meter);
	updateRects();
}

instance::~instance()
{
	if(meter) delete [] meter;
}

player::~player(){}


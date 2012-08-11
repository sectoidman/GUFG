#include "interface.h"
projectile::projectile(const char* directory, const char* file)
{
	head = new actionTrie;
	airHead = new actionTrie;
	build(directory, file);
	meter = new int[3];
	init();
}

void projectile::build(const char* directory, const char* file)
{
	avatar::build(directory, file);
	char buffer[101];
	sprintf(buffer, "%s/NS", name);
	neutral = new looping(buffer);
	sprintf(buffer, "%s/die", name);
	die = new action(buffer);
	head->insert(die);
	airHead->insert(die);
}

void projectile::init()
{
	if(cMove){
		if(cMove != neutral) cMove->init();
	}
	neutral->init();
	cMove = neutral;
	meter[0] = 60 * 30;
	freeze = 0;
	dead = false;
	freeze = 0;
	aerial = 0;
}

summon::summon(const char * n)
{
	build(n);
	init();
}

airSummon::airSummon(const char * n)
{
	build(n);
	init();
}

avatar * summon::spawn()
{
	payload->init();
	return payload;
}

void summon::zero()
{
	spawnFrame = 0;
	spawnTrackY = 0;
	spawnTrackX = 0;
	spawnPosY = 0;
	spawnPosX = 0;
	action::zero();
}

void projectile::step()
{
	avatar::step();
	if(cMove == die){
		if(cMove->currentFrame == cMove->frames - 1){
			dead = true;
		}
	}
	if (meter[0] <= 0) dead = true;
}

void projectile::tick()
{
	meter[0]--;
}

bool summon::setParameter(char * buffer)
{
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);

	char * token = strtok(buffer, "\t: \n-");

	if(!strcmp("SpawnPosition", token)){
		token = strtok(NULL, "\t: \n");
		spawnPosX = atoi(token);

		token = strtok(NULL, "\t: \n");
		spawnPosY = atoi(token);
		return 1;
	} else if(!strcmp("SpawnsOn", token)){
		token = strtok(NULL, "\t: \n");
		spawnFrame = atoi(token);
		return 1;
	} else if(!strcmp("Payload", token)){
		token = strtok(NULL, "\t: \n");
		tempPayload = new char[strlen(token)+1];
		strcpy(tempPayload, token);
		return 1;
	} else return action::setParameter(savedBuffer);
}

int summon::arbitraryPoll(int q)
{
	if(currentFrame == spawnFrame){
		switch(q){
		case 50:
			return 1;
		case 51:
			if(spawnTrackX) return 1;
			else break;
		case 52:
			if(spawnTrackY) return 1;
			else break;
		case 53:
			return spawnPosX;
		case 54:
			return spawnPosY;
		default:
			break;
		}
	}
	return action::arbitraryPoll(q);
}

void airSummon::zero()
{
	summon::zero();
	airMove::zero();
}

bool airSummon::setParameter(char * buffer)
{
	bool x;
	char savedBuffer[100];
	strcpy(savedBuffer, buffer);
	x = summon::setParameter(buffer);
	if (!x) x = airMove::setParameter(savedBuffer);
	return x;
}

char * summon::request(int code, int i){
	if(code == 4)
		return tempPayload;
	else return action::request(code, i);
}

void summon::generate(const char* directory, const char* name)
{
	payload = new projectile(directory, name);
	if(tempPayload) delete [] tempPayload;
	tempPayload = NULL;
}

char* airSummon::request(int code, int i)
{
	char * x;
	x = summon::request(code, i);
	if(x == NULL) x = airMove::request(code, i);
	return x;
}

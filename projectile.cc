#include "interface.h"
projectile::projectile(const char* directory, const char* file)
{
	head = new actionTrie;
	airHead = new actionTrie;
	build(directory, file);
	meter = new int[4];
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

void projectile::init(action *& cMove)
{
	cMove = neutral;
	meter[3] = 60 * 30;
	freeze = 0;
	dead = false;
	freeze = 0;
	aerial = 0;
}

summon::summon(const char * n)
{
	build(n);
}

airSummon::airSummon(const char * n)
{
	build(n);
}

avatar * summon::spawn()
{
	payload->init(payload->neutral);
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

bool projectile::step(action *& cMove, int &f)
{
	avatar::step(cMove, f);
	if(cMove == die){
		if(f == cMove->frames - 1){
			return true;
		}
	}
	if (meter[3] <= 0) return true;
	return false;
}

void projectile::tick()
{
	meter[3]--;
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

int summon::arbitraryPoll(int q, int f)
{
	switch(q){
	case 50:
		if(f == spawnFrame) return 1;
		else break;
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
	return action::arbitraryPoll(q, f);
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

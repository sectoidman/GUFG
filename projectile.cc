#include "interface.h"
projectile::projectile(const char* directory, const char* file)
{
	head = new actionTrie;
	airHead = new actionTrie;
	neutral = NULL;
	build(directory, file);
	meter = new int[4];
}

void projectile::build(const char* directory, const char* file)
{
	getName(directory, file);

	char buffer[101];

	sprintf(buffer, "%s/die", name);
	die = new action(buffer);
	head->insert(die);
	avatar::build(directory, file);
}

bool projectile::acceptTarget(action * c, int f)
{
	if(c->stats[c->calcCurrentHit(f)].hitsProjectile) return 1;
	else return 0;
}

void projectile::init(action *& cMove)
{
	cMove = neutral;
	aerial = 0;
}

void projectile::processMove(action * m)
{
	if(neutral == NULL){ 
		neutral = m;
	}
	avatar::processMove(m);
	if(m->dies){ 
		m->feed(die, 2, m->hits-1);
	}
}

summon::summon(const char * n)
{
	build(n);
}

airSummon::airSummon(const char * n)
{
	build(n);
}

instance * summon::spawn()
{
	payload->init(payload->neutral);
	instance * ret = new instance(payload);
	return ret;
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

bool projectile::death(action *& cMove, int f)
{
	if(cMove == die){
		if(f == cMove->frames - 1){
			return true;
		}
	}
	return false;
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
		if(f == spawnFrame){ 
			return 1;
		}
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

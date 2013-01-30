#include "interface.h"
projectile::projectile(const char* directory, const char* file)
{
	head = new actionTrie;
	neutral = NULL;
	build(directory, file);
}

void projectile::build(const char* directory, const char* file)
{
	getName(directory, file);

	char buffer[101];

	sprintf(buffer, "%s/die", name);
	die = new action(buffer);
	head->insert(die, 0);
	avatar::build(directory, file);
	lifespan = -1;
}

int projectile::acceptTarget(action * c, int f)
{
	if(c->stats[c->calcCurrentHit(f)].hitsProjectile || c->stats[c->calcCurrentHit(f)].killsProjectile || c->stats[c->calcCurrentHit(f)].turnsProjectile)
		return 1;
	return 0;
}

void projectile::init(int *& meter)
{
	meter[0] = 600;
	meter[1] = 0;
	meter[2] = 1;
	meter[3] = 1;
}

void projectile::processMove(action * m)
{
	if(neutral == NULL){ 
		neutral = m;
	}
	avatar::processMove(m);
	if(m->dies){ 
		m->feed(die, 2, m->hits-1);
		m->feed(die, 0, 0);
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
	instance * ret = new instance(payload);
	return ret;
}

void summon::zero()
{
	spawnFrame = 0;
	spawnTrackY = 0;
	spawnTrackX = 0;
	spawnTrackFloor = 0;
	spawnPosY = 0;
	spawnPosX = 0;
	lifespan = -1;
	allegiance = 1;
	action::zero();
}

bool projectile::turn(int &ID)
{
	ID = ID % 2 + 1;
	return 1;
}

int projectile::takeHit(action *& cMove, hStat & s, int blockType, int &frame, int &connectFlag, int &hitFlag, int &hitType, bool &aerial, int *& meter)
{
	if(s.killsProjectile || cMove->hittable){ 
		die->execute(cMove, meter, frame, connectFlag, hitFlag);
		cMove = die;
		return 1;
	} else return 0;
}

bool projectile::death(action *& cMove, int f, int counter)
{
	if(cMove == die){
		if(f == cMove->frames - 1){
			return true;
		}
	}
	if(cMove != die && lifespan > 0 && counter > lifespan){
		cMove = die;
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
	} else if(!strcmp("Track", token)){
		token = strtok(NULL, "\t: \n");
		for(unsigned int i = 0; i < strlen(token) + 1; i++){
			switch(token[i]){
			case 'x': 
				spawnTrackX = true;
				break;
			case 'y':
				spawnTrackY = true;
				break;
			case 'f':
				spawnTrackFloor = true;
				break;
			}
		}
		return 1;
	} else if(!strcmp("SpawnsOn", token)){
		token = strtok(NULL, "\t: \n");
		spawnFrame = atoi(token);
		return 1;
	} else if(!strcmp("Lifespan", token)){
		token = strtok(NULL, "\t: \n");
		lifespan = atoi(token);
		return 1;
	} else if(!strcmp("Allegiance", token)){
		token = strtok(NULL, "\t: \n");
		allegiance = atoi(token);
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
		if(spawnTrackFloor) return 1;
		else break;
	case 54:
		return spawnPosX;
	case 55:
		return spawnPosY;
	case 56:
		return allegiance;
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
	if(lifespan) payload->lifespan = lifespan;
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

#include "interface.h"

projectile::projectile(string directory, string file)
{
	head = new actionTrie;
	neutral = nullptr;
	build(directory, file);
}

void projectile::build(string directory, string file)
{
	getName(directory.c_str(), file.c_str());
	die = mandateMove("die");
	avatar::build(directory.c_str(), file.c_str());
	head->insert(die, 0);
	lifespan = -1;
}

int projectile::acceptTarget(action * c, int f)
{
	if(c->stats[c->calcCurrentHit(f)].hitsProjectile || c->stats[c->calcCurrentHit(f)].killsProjectile || c->stats[c->calcCurrentHit(f)].turnsProjectile)
		return 1;
	return 0;
}

void projectile::pollStats(hStat & s, status &current)
{
	s.isProjectile = true;
	current.move->pollStats(s, current.frame, current.counter);
	if(s.pause == -1)
		s.pause = 4;
}

void projectile::init(vector<int>& meter)
{
	meter[0] = 600;
	meter[1] = 0;
	meter[2] = 1;
	meter[3] = 1;
}

void projectile::processMove(action * m)
{
	if(neutral == die || neutral == nullptr){ 
		neutral = m;
	}
	avatar::processMove(m);
	if(m->dies){ 
		m->feed(die, 2, m->hits-1);
		m->feed(die, 0, 0);
	}
}

bool projectile::turn(int &ID)
{
	ID = ID % 2 + 1;
	return 1;
}

int projectile::takeHit(status &current, hStat &s, int blockType, int &hitType, vector<int>& meter)
{
	if(s.killsProjectile || current.move->hittable){ 
		die->execute(current.move, meter, current.frame, current.connect, current.hit);
		current.move = die;
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


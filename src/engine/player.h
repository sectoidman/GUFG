/*Header file for "Player" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see the file "COPYING" for details
 */
#include "../charlist.h"
#include "controller.h"
#ifndef PLAYER
#define PLAYER
class frame;
class instance{
public:
	instance();
	instance(avatar*);
	virtual ~instance();
	virtual avatar * pick() { return v; }

	SDL_Rect collision;
	std::vector<SDL_Rect> hitbox, hitreg, momentum;
	bool secondInstance;
	int ID;
	int inputBuffer[30];
	void checkFacing(instance*);
	virtual void neutralize();
	virtual bool acceptTarget(instance*);
	virtual bool CHState() { return 0; }
	virtual void init();
	virtual void step();

	virtual void getMove(std::vector<int>, std::vector<bool>, SDL_Rect&, bool&);
	virtual int takeHit(int, hStat&, SDL_Rect&);
	virtual void enforceGravity(int, int);
	virtual void invertVectors(int);
	virtual void pullVolition();
	virtual void setPosition(int, int);
	virtual void updateRects();
	virtual void draw();
	virtual void drawBoxen();
	virtual void combineDelta();
	virtual bool spriteCheck();
	virtual void connect(int, hStat&);
	virtual void pushInput(unsigned int);
	virtual int passSignal(int);
	virtual int dragBG(int, int);
	virtual void follow(instance*);
	void enforceAttractor(attractor*);
	int middle();
	void flip();
	void print();

	status current;

	int * meter;
	int particleType;
	int blockType;
	int counter;
	bool boxen:1;
	bool sprite:1;
protected:
	avatar * v;
};

class player : public instance, public controller{
public:
	player();
	player(int);
	~player();
	virtual character * pick() { return v; }

	int rounds;		//How many rounds has this player won this match?
	int padding[400];	//More magic. Do not touch
	virtual void characterSelect(int);
	virtual void drawHitParticle();

	virtual void readEvent(SDL_Event &, frame &);
	virtual void land();
	virtual void readScripts();
	virtual void enforceGravity(int, int);
	virtual void drawMeters(int);
	virtual void roundInit();
	virtual void macroCheck(SDL_Event&);
	virtual int takeHit(int, hStat&, SDL_Rect&);
//	virtual void connect(int, hStat&);
	virtual bool CHState();

	void setKey(int);
	bool setKey(int, SDL_Event);

	bool elasticX;
	bool elasticY;
	bool slide;
	bool stick;
	int hover;

	char * name;

	int particleLife;

	/*Helper functions for "resolve" tick*/
	virtual void checkBlocking();
	virtual void checkCorners(int, int);
	virtual void enforceFloor(int);
	virtual void getThrown(action*, int, int);

	virtual void init();
	int wins;
	bool search:1;
	script * record;
private:
	character * v;
};
#endif

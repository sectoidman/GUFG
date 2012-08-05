/*Header file for character class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "trie.h"

class avatar{
public:
	virtual void init() = 0;
	bool spriteCheck();
	virtual void draw(int, int, int);//Pass sprite information up.
	virtual action * createMove(char*);
	virtual void processMove(action * m);
	virtual void build(const char*);

	action * cMove;
	action * bMove;
	int freeze;
	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	bool aerial; 	//Flags whether the character is considered "in the air"
	actionTrie * head;	//Trie for ground actions
	actionTrie * airHead;	//Trie for air actions
};

class character : public avatar{
public:
	character(const char*);
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	virtual void build(const char*);//This will *eventually* be the function that parses the character constructor file.

	virtual void drawMeters(int);
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, bool);	//Take input from the game and propagate it to the appropriate actionTrie.
	virtual void init();
	virtual void resetAirOptions();
	virtual void tick() {}
	virtual void step();
	virtual void connect(hStat&);
	virtual void land();
	virtual int takeHit(hStat&, int);
	//BRB prepping my hooks

	action * neutral;
	action * airNeutral;
	action * crouch;
	hitstun * reel;
	untechState * untech;
	airLooping * fall;
	utility * down;
	mash * tech;
	hitstun * crouchReel;
	hitstun * crouchBlock;
	hitstun * standBlock;
	hitstun * airBlock;
	utility * throwBreak;

	int health;
	int * meter;
private:
	int state; 
};

class projectile : virtual public avatar {
public:
	projectile(const char*);
	projectile();

	int ID;
	int posX, posY;
	
	int hitComplexity, regComplexity, momentumComplexity;
	SDL_Rect *momentum, *hitbox, *hitreg, collision, spr;
};

class summon : virtual public action {
public:
	summon() {}
	summon(const char*);
	virtual int arbitraryPoll(int);
	virtual bool setParameter(char*);

	avatar * payload;
	int spawnFrame;
};

class airSummon : public airMove, public summon {
public:
	airSummon() {}
	virtual bool setParameter(char*);
	airSummon(const char*);
};


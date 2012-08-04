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
	action * cMove;
	action * bMove;
	int freeze;
	bool spriteCheck();
	virtual void draw(int, int, int);//Pass sprite information up.
};

class character : public avatar{
public:
	character(const char*);
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	void build(const char*);	//This will *eventually* be the function that parses the character constructor file.
	actionTrie * head;	//Trie for ground actions
	actionTrie * airHead;	//Trie for air actions

	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	virtual void drawMeters(int);
	virtual void prepHooks(int[], bool[], bool[], SDL_Rect &, bool);	//Take input from the game and propagate it to the appropriate actionTrie.
	virtual void init();
	virtual void resetAirOptions();
	virtual void tick() {}
	virtual void step();
	virtual void connect(hStat&);
	virtual void land();
	virtual int takeHit(hStat&, int);
	virtual action * createMove(char*);
	virtual void processMove(action * m);
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
	int jumpOptions;
	bool aerial; 		//Flags whether the character is considered "in the air"
private:
	int state; 

};

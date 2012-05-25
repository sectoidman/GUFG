/*Header file for character class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see "COPYING" for details
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "trie.h"

class character{
public:
	character();		//Load the entire character into memory. This should happen once per player per match.
	virtual ~character();	//Free stuff

	void build(const char*);	//This will *eventually* be the function that parses the character constructor file.
	moveTrie * head;	//Trie for ground moves
	moveTrie * airHead;	//Trie for air moves

	char * name; //The name of the directory from which the character spawns. This is important for loading into memory
	virtual void drawMeters(SDL_Surface *&, int);
	SDL_Surface * draw(int);//Pass sprite information up.
	virtual void prepHooks(int[], bool[], bool[]);	//Take input from the game and propagate it to the appropriate moveTrie.
	virtual void init();
	virtual void resetAirOptions();
	virtual void tick() {}
	virtual void connect(hStat&);
	virtual void land();
	virtual int takeHit(hStat&);
	virtual move * createMove(char*, char*);
	//BRB prepping my hooks

	move * neutral;
	move * airNeutral;
	move * crouch;
	hitstun * reel;
	hitstun * untech;
	airLooping * fall;
	utility * down;
	airMove * tech;
	hitstun * crouchReel;
	hitstun * crouchBlock;
	hitstun * standBlock;
	hitstun * airBlock;

	move * cMove;
	move * bMove;

	int freeze;
	int health;
	int * meter;
	int rounds;
	int jumpOptions;
	SDL_Rect * volition;
	bool aerial; 		//Flags whether the character is considered "in the air"
private:

	int state; 

};

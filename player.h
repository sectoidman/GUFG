/*Header file for "Player" class in project Ground Up Fighting Game
 *
 *Written by Alex Kelly in 2012
 *Under MIT OSI license, see the file "COPYING" for details
 */

#include "charlist.h"
class player{
public:
	player();
	player(int);
	~player();

	char * inputName[10];   //Input names. This is really just for housekeeping.
	SDL_Event input[10];    //Inputs. These are the SDL_Events tied to the 10 buttons in the actual game
	SDL_Surface * sprite;
	character * pick;
	int deltaX, deltaY; 
	int posX, posY;
	int rounds;		//How many rounds has this player won this match?
	int facing;
	int padding[400];   //More magic. Do not touch
	void writeConfig();
	bool readConfig();
	void characterSelect(int);
	void pushInput(bool*, bool*, bool*);
	void draw();
	void spriteInit();
	void pullVolition();

	SDL_Rect spr, *hitbox, *hitreg, collision, *momentum, lock;
	int hitComplexity, regComplexity, momentumComplexity;
	bool rCorner, lCorner;

	/*Helper functions for "resolve" tick*/
	void updateRects();
	int checkHit(player*);
	void resolveCollision(player*);
	void conserveMomentum();
	void enforceGravity(int, int);
	void checkBlocking();
	void checkFacing(int);
	int dragBG(int, int);
	void checkCorners(int, int, int);
	void combineDelta();

private:
	int ID;
	int inputBuffer[30];
	void init();
	void addVector(SDL_Rect);
	void removeVector(int);
};

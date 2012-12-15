/*Interface header for GUFG
 *The interface class/struct is intended as an abstraction between the player and the engine
 *This is where things pertaining to inputs should go, as well as sprsibly information like the
 *round timer.
 *
 *Written by Alex Kelly in 2012
 *Licensed under MIT OSI: Additional information in the file "COPYING"
 */

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include "analytics.h"
#ifndef INTERFACE
#define INTERFACE



class gameInstance{
public:
	virtual void resolve() = 0;	/*Every game type requires a resolve loop.*/

	int screenHeight, screenWidth, floor, wall;
	int freeze;
};

class interface : public gameInstance{
public:
	interface();
	~interface();
	void genInput();
	void resolve();		/*The main loop of the game*/
	void draw();		/*The primary function in charge of drawing things on the screen.*/
	void drawGame();
	void cleanup();
	void dragBG(int);
	void doSuperFreeze();
	void unitCollision();
	void resolveHits();
	void resolveThrows();
	void resolveSummons();
	void addThing(instance*);
	void cullThing(int);
	void addAttractor(attractor*);
	void cullAttractor(int);

	GLuint background;
	bool select[2];
	int selection[2];
	int menu[2];
	int configMenu[2];
	int rMenu;
	SDL_Rect bg;
	SDL_Rect prox;
	bool boxen:1;
	bool firstFrame:1;
	bool shortcut:1;
	bool continuous:1;
	bool single:1;
	bool analytics:1;
	int combo[2];
	int damage[2];
	bool illegit[2];
	int ** matchup;
	int numRounds;
	int grav;	//Gravitational constant. 
	instance ** things;
	int thingComplexity;
	attractor ** globals;
	int attractorComplexity;
};
#endif

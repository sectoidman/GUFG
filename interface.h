/*Interface header for GUFG
 *The interface class/struct is intended as an abstraction between the player and the engine
 *This is where things pertaining to inputs should go, as well as sprsibly information like the
 *round timer.
 *
 *Written by Alex Kelly in 2012
 *Licensed under MIT OSI: Additional information in the file "COPYING"
 */

#include <SDL/SDL.h>
#include <stdio.h>
#include "compat.h"
#include "player.h"
#ifndef INTERFACE
#define INTERFACE
class interface
{
public:
	interface();		//Constructor!
	~interface();
	/*Important interface functions that will remain interface functions*/

	void readInput();	//Reads input from players.
	void resolve();		//Main resolution function per frame. Happens exactly once per frame.
	void draw();		//The primary function in charge of drawing things on the screen.
	void runTimer();	//Runs the timer. Currently just a decrementer. May always just be a decrementer.
	void spriteInit();	//Part of the rendering toolchain. Sets up sprites for drawing.
	bool screenInit();
	int drawGlyph(const char *, int, int, int, int, int);
	bool screenInit(int, int);
	void roundInit();
	void matchInit();
	void cSelectMenu();
	void checkWin();
	void dragBG(int);
	void doSuperFreeze();
	void unitCollision();
	void loadMisc();
	void writeConfig(int);
	void resolveHits();
	void resolveThrows();
	void resolveSummons();
	void addThing(instance*);
	void cullThing(int);
	void writeImage(const char*, int, action*);

	player * p[2];
	bool * sAxis[2];	//Initial input buffering.
	bool * posEdge[2];
	bool * negEdge[2];
	bool select[2];
	int selection[2];
	SDL_Surface *screen;
	GLuint glyph[91];
	GLuint background;
	SDL_Rect bg;
	SDL_Rect prox;
	int grav;		//Gravitational constant. 
	bool fullscreen;	//For use with later
	bool initd:1;
	int combo[2];
	int damage[2];
	int numRounds;
	bool gameover;
	float scalingFactor, sf;
	instance ** things;
	int thingComplexity;

//Variables for cSelectMenu 
	int numChars;
	GLuint selectScreen; 
	GLuint cursor [2];
	SDL_Rect wheel;
	int counter[2];

	bool q;
	int timer;
	bool roundEnd:1;
	int endTimer;
private:
	int screenHeight, screenWidth, floor, wall;
	int freeze;
};
#endif

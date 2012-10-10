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
class interface
{
public:
	interface();
	~interface();
	void createPlayers();
	void createDemons();
	void createDemons(replay*);
	void startGame();
	void loadMatchBackground();
/*Important interface functions that will remain interface functions*/

/*The main game loop runs readInput() and resolve() each exactly once per frame loop.*/
	void readInput();	/*Accepts input into input containers, for use by anything that wants it*/
	void genInput();
	void resolve();		/*The main loop of the game*/
	void draw();		/*The primary function in charge of drawing things on the screen.*/
	void drawGame();
	void drawCSelect();
	void drawMainMenu(int);
	void drawRematchMenu();
	void cleanup();
	void runTimer();	/*Currently just a decrementer. May always just be a decrementer.*/
	void spriteInit();	/*Part of the rendering toolchain. Sets up sprites for drawing.*/
	bool screenInit();
	int drawGlyph(const char *, int, int, int, int, int);
	bool screenInit(int, int);
	void roundInit();
	void matchInit();
	void cSelectMenu();
	void mainMenu(int);
	void rematchMenu();
	void checkWin();
	void dragBG(int);
	void doSuperFreeze();
	void unitCollision();
	void loadMisc();
	void readMatchupChart();
	void writeMatchupChart();
	void writeConfig(int);
	void resolveHits();
	void resolveThrows();
	void resolveSummons();
	void addThing(instance*);
	void cullThing(int);
	void addAttractor(attractor*);
	void cullAttractor(int);
	void writeImage(const char*, int, action*);

	player * p[2];
	bool * sAxis[2];
	int * posEdge[2];
	bool * negEdge[2];
	bool select[2];
	int selection[2];
	int menu[2];
	int rMenu;
	SDL_Surface *screen;
	GLuint glyph[91];
	GLuint background;
	SDL_Rect bg;
	SDL_Rect prox;
	bool fullscreen;
	bool initd:1;
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
	bool gameover;
	float scalingFactor, sf;
	int grav;	//Gravitational constant. 
	instance ** things;
	int thingComplexity;
	Mix_Music *menuMusic;
	Mix_Music *matchMusic;
	Mix_Chunk **announceWinner;
	Mix_Chunk *announceRound[3];
	Mix_Chunk *announceDraw[2];
	Mix_Chunk *announceFight;
	Mix_Chunk *announceEnd[2];
	Mix_Chunk *announceSelect;

	attractor ** globals;
	int attractorComplexity;

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
	replay * currentMatch;
private:
	int screenHeight, screenWidth, floor, wall;
	int freeze;
};
#endif

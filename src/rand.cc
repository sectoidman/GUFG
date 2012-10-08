/**GUFG
 *Written by Alex Kelly in 2012
 *Modified by H Forrest Alexander just now.
 *Under the MIT OSI license
 *For detailed licensing information, see the COPYING file in this directory.
 *This file runs a loop of everything that takes place in the GUFG engine. 
 
 **/

#include "compat.h"
#include "engine/interface.h"
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
using namespace internal;

int main(int argc, char* argv[])
{
	/*GUFG uses the `chrono` component of stdlibc++ to frame lock the game*/
	interface game;
	game.createDaemons();
	game.startGame();
	int rounds = 2;
	if(argc > 1) rounds = atoi(argv[1]);
	if(rounds > 0 && rounds < 10) game.numRounds = rounds;

	/*As long as the game doesn't detect a request to quit, it loops over a few basic resolutions of game events*/
	while (!game.gameover){
		game.genInput();
		game.resolve();
		game.cleanup();
	}
	game.writeMatchupChart();
	return 0;
}


/**GUFG
 *Written by Alex Kelly in 2012
 *H Forrest Alexander hasn't yet modified this file, other than two trivial lines in the license.
 *Under the MIT OSI license
 *For detailed licensing information, see the COPYING file in this directory.
 *This file runs a headless, random subset of everything that takes place in the GUFG engine, forcing innocent daemons to fight against eachother. 
 
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


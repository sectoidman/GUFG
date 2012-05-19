/*Key Test for GUFG
 *This tiny program is intended to do some of the initial SDL engine testing for
 *Project: Ground Up Fighting Game.
 *
 *Written by Alex Kelly in 2012
 *Under the MIT OSI license
 *For detailed licensing information, see the COPYING file in this directory.
 */

#include "interface.h"
#include <unistd.h>
#include <stdlib.h>
using namespace std;
int main(int argc, char* argv[])
{
	/*Spawn the interface*/
	interface game;
	int rounds = 2;
	int count = 0;
	if(argc > 1) rounds = atoi(argv[1]);
	if(rounds > 0 && rounds < 10) game.numRounds = rounds;

	/*Loop of everything*/
	while (!game.gameover){
		while(SDL_GetTicks() % 17 != 1);
		count = SDL_GetTicks();
		game.readInput();
		game.resolve();
		game.draw();
		if(SDL_GetTicks() - count > 15) printf("Warning: Framerate questionable\n");
		while(SDL_GetTicks() % 17 != 0);
	}
}

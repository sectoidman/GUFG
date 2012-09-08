/*Key Test for GUFG
 *This tiny program is intended to do some of the initial SDL engine testing for
 *Project: Ground Up Fighting Game.
 *
 *Written by Alex Kelly in 2012
 *Under the MIT OSI license
 *For detailed licensing information, see the COPYING file in this directory.
 */

#include "interface.h"
#include "compat.h"
#include <unistd.h>
#include <stdlib.h>
#include <SDL/SDL_opengl.h>
using namespace internal;

int main(int argc, char* argv[])
{
	/*Spawn the interface*/
	typedef chrono::duration<int,micro> microsecs_t ;
	chrono::high_resolution_clock::time_point frameStart;
	chrono::high_resolution_clock::time_point frameEnd;
	interface game;
	int rounds = 2;
//	int count = 0;
	if(argc > 1) rounds = atoi(argv[1]);
	if(rounds > 0 && rounds < 10) game.numRounds = rounds;

	/*Loop of everything*/
	while (!game.gameover){
		frameStart = chrono::high_resolution_clock::now();
		game.readInput();
		game.resolve();
		int dur;
		do {
			frameEnd = chrono::high_resolution_clock::now();
			microsecs_t frameDuration(chrono::duration_cast<microsecs_t>(frameEnd - frameStart));
			microsecs_t duration(16667 - frameDuration.count());
			dur = duration.count();
		} while(dur > 0);
	}
    return 0;
}


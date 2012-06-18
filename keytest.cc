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
#include <chrono>
using namespace std;
int main(int argc, char* argv[])
{
	/*Spawn the interface*/
	interface game;
	int rounds = 2;
//	int count = 0;
	if(argc > 1) rounds = atoi(argv[1]);
	if(rounds > 0 && rounds < 10) game.numRounds = rounds;

	/*Loop of everything*/
	while (!game.gameover){
		std::chrono::high_resolution_clock::time_point frameEnd; 
		std::chrono::high_resolution_clock::time_point frameStart = std::chrono::high_resolution_clock::now(); 
		game.readInput();
		game.resolve();
		int dur;
		typedef std::chrono::duration<int,std::micro> microsecs_t ;
		do{
			frameEnd = std::chrono::high_resolution_clock::now(); 
			microsecs_t frameDuration( std::chrono::duration_cast<microsecs_t>(frameEnd - frameStart) ); 
			dur = frameDuration.count();
		} while (dur < 16667);
		std::cout << "Frame time " << dur << " [us] " << std::endl; 
	}
}


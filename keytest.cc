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
		while(SDL_GetTicks() % 17 != 1);
		std::chrono::high_resolution_clock::time_point lEnd; 
		std::chrono::high_resolution_clock::time_point lStart = std::chrono::high_resolution_clock::now(); 
		game.readInput();
		game.resolve();
		lEnd = std::chrono::high_resolution_clock::now(); 
		typedef std::chrono::duration<int,std::milli> millisecs_t ; 
		typedef std::chrono::duration<int,std::micro> microsecs_t ; 
		typedef std::chrono::duration<double> secs_t ; 
		microsecs_t lUDuration( std::chrono::duration_cast<microsecs_t>(lEnd-lStart) ) ; 
		secs_t lDuration( std::chrono::duration_cast<microsecs_t>(lEnd-lStart) ) ; 
		std::cout << "Time spent " << lUDuration.count() << " [us] " << std::endl; 
		std::cout << "Time spent " << lDuration.count() << " [s] " << std::endl; 
//		while(SDL_GetTicks() % 17 != 0);
	}
}


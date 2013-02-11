/**GUFG
 *Written by Alex Kelly in 2012
 *Modified by H Forrest Alexander just now.
 *Under the MIT OSI license
 *For detailed licensing information, see the COPYING file in this directory.
 *This file runs a loop of everything that takes place in the GUFG engine. 
 **/
#include <chrono>
#include "engine/interface.h"
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
int main(int argc, char* argv[])
{
	/*GUFG uses the `chrono` component of stdlibc++ to frame lock the game*/
	const int FPS = 60;
	typedef std::chrono::duration<float,std::ratio<1,FPS>> frame_t;
        std::chrono::high_resolution_clock::time_point frameStart;
	interface game;
	assert(game.screenInit() != false);
	if(argc > 1){ 
		if(!strcmp(argv[1], "eleven")){ 
			game.killTimer = true;
			game.createPlayers();
		} else game.createPlayers(argv[1]);
	} else game.createPlayers();
	game.loadMisc();
	game.startGame();
	game.numRounds = 2;

	/*As long as the game doesn't detect a request to quit, it loops over a few basic resolutions of game events*/
	while (!game.gameover){
		frameStart = std::chrono::high_resolution_clock::now();
		game.readInput();
		game.resolve();
		game.draw();
		game.cleanup();
		frame_t frameElapsed;
		/*Now eat up the rest of the frame: 
		 *Check the time elapsed thus far in a loop
		 *until it is one frame_t unit.
		 *Then go on to the next frame
		 */
		do frameElapsed = std::chrono::duration_cast<frame_t>(std::chrono::high_resolution_clock::now() - frameStart);
		while(frameElapsed.count() < 1);
	}
	return 0;
}


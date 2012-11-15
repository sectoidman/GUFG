/*Pocket Plagues Battle Client
 *Written by Alex Kelly in 2012
 *Under the MIT OSI license
 *For detailed licensing information, see the COPYING file in this directory.
 *This file runs a loop of everything that takes place in the GUFG engine.
 */
#include "compat.h"
#include "engine/cockfight.h"
#include <SDL/SDL_opengl.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
using namespace internal;

int main(int argc, char* argv[])
{
	/*GUFG uses the `chrono` component of stdlibc++ to frame lock the game*/
	const int FPS = 60;
	typedef chrono::duration<float,std::ratio<1,FPS>> frame_t;
	chrono::high_resolution_clock::time_point frameStart;
	cockfight game;
	//assert(game.screenInit() != false);
	//game.createPlayers();
	//game.loadMisc();
	//game.startGame();

	/*As long as the game doesn't detect a request to quit, it loops over a few basic resolutions of game events*/
	while (!game.gameover){
		frameStart = chrono::high_resolution_clock::now();
		game.readInput();
		game.resolve();
		game.draw();
	//	game.cleanup();
		frame_t frameElapsed;
		/*Now eat up the rest of the frame: 
		 *Check the time elapsed thus far in a loop
		 *until it is one frame_t unit.
		 *Then go on to the next frame
		 */
		do frameElapsed = chrono::duration_cast<frame_t>(chrono::high_resolution_clock::now() - frameStart);
		while(frameElapsed.count() < 1);
	}
	return 0;
}


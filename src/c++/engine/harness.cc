#include "harness.h"
#include "window.h"
#include <GL/glu.h>
harness::harness()
{
	gameover = false;
}

void harness::init()
{
	/*Set up input buffers and joysticks*/
	for(int i = 0; i < SDL_NumJoysticks(); i++)
		SDL_JoystickOpen(i);
}

void harness::processInput(SDL_Event &event)
{
	/*Do stuff with event*/
	switch (event.type){
	/*Kill handler*/
	case SDL_QUIT:
		printf("Sigterm received!\n");
		gameover = 1;
		break;
	/*Keyboard handler. Maybe I'll optimize such that the knows if it even needs to check this (EG if sticks are used)*/
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (event.key.keysym.sym) {
		case SDLK_q:
		case SDLK_ESCAPE:
			gameover = 1;
			break;
		default:
			break;
		}
		break;
	}
}

void harness::readInput()
{
	/*Make our dummy event for polls*/
	SDL_Event event;
	for(int i = 0; i < 20; i++){
		if (SDL_PollEvent(&event)){
			processInput(event);
		}
	}
}

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
	camY = 0.0;
	camX = 0.0;
	camZ = -5.0;
	size = 1.0;
	pan = 0.0;
	tilt = 0.0;
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
		switch (event.key.keysym.sym) {
		case SDLK_q:
		case SDLK_ESCAPE:
			gameover = 1;
			break;
		case SDLK_w:
			printf("%d\n", camZ);
			camZ += 0.1f;
			break;
		case SDLK_s:
			printf("%d\n", camZ);
			camZ -= 0.1f;
			break;
		case SDLK_d:
			camX += 0.1f;
			break;
		case SDLK_a:
			camX -= 0.1f;
			break;
		case SDLK_UP:
			camY += 0.1f;
			break;
		case SDLK_DOWN:
			camY -= 0.1f;
			break;
		case SDLK_LEFT:
			pan += 10.0f;
			break;
		case SDLK_RIGHT:
			pan -= 10.0f;
			break;
		case SDLK_e:
			tilt += 5.0f;
			break;
		case SDLK_r:
			tilt -= 5.0f;
			break;
		case SDLK_z:
			size -= 1.0f;
			break;
		case SDLK_x:
			size += 1.0f;
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

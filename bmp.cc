#include <iostream>
#include <stdio.h>
#include <fstream>
#include "interface.h"
#include <cstring>

void writeImage(const char * movename, int frame, action * move)
{
	int realPosY = move->collision[frame].y;
	int realPosX = 0;
	int maxY = 150, maxX = 65;
	for(int i = 0; i < move->regComplexity[frame]; i++){
		if(move->hitreg[frame][i].y < realPosY) 
			realPosY = move->hitreg[frame][i].y;
		if(move->hitreg[frame][i].x < realPosX) 
			realPosX = move->hitreg[frame][i].x;
		if(move->hitreg[frame][i].x + move->hitreg[frame][i].w > maxX)
			maxX = move->hitreg[frame][i].x + move->hitreg[frame][i].w;
		if(move->hitreg[frame][i].y + move->hitreg[frame][i].h > maxY)
			maxY = move->hitreg[frame][i].y + move->hitreg[frame][i].h;
	}
	for(int i = 0; i < move->hitComplexity[frame]; i++){
		if(move->hitbox[frame][i].y < realPosY) 
			realPosY = move->hitbox[frame][i].y;
		if(move->hitbox[frame][i].x < realPosX) 
			realPosX = move->hitbox[frame][i].x;
		if(move->hitbox[frame][i].x + move->hitbox[frame][i].w > maxX)
			maxX = move->hitbox[frame][i].x + move->hitbox[frame][i].w;
		if(move->hitbox[frame][i].y + move->hitbox[frame][i].h > maxY)
			maxY = move->hitbox[frame][i].y + move->hitbox[frame][i].h;
	}
	char fname[200];
	int w = maxX + 5;
	int h = maxY + 5;
	int x = 0;
	int y = 0;
	if(realPosY < 0){ 
		h -= realPosY;
		y = realPosY;
	}
	if(realPosX < 0){
		w -= realPosX;
		x = realPosX;
	}
	sprintf(fname, "%s#%i.bmp", movename, frame);
	SDL_Rect q = { (Sint16)x, (Sint16)y, (Uint16)w, (Uint16)h };
	SDL_Surface /*a, *b, *c,*/ *d;
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	mask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
/*	a = SDL_CreateRGBSurface(SDL_SWSURFACE, w*2, h*2, 32,
		rmask, gmask, bmask, amask);
	b = SDL_CreateRGBSurface(SDL_SWSURFACE, w*2, h*2, 32,
		rmask, gmask, bmask, amask);
	c = SDL_CreateRGBSurface(SDL_SWSURFACE, w*2, h*2, 32,
		rmask, gmask, bmask, amask);
*/	d = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32,
		rmask, gmask, bmask, amask);
	SDL_FillRect (d, &q, SDL_MapRGBA(d->format, 0, 0, 0, 255));
	SDL_Rect *v = &move->collision[frame];
	SDL_FillRect (d, v, SDL_MapRGBA(d->format, 255, 255, 255, 127));
	for(int i = 0; i < move->regComplexity[frame]; i++){
		v = &move->hitreg[frame][i];
		v->x -= x; v->y -= y;
		SDL_FillRect (d, v, SDL_MapRGBA(d->format, 0, 255, 0, 127));
	}
	for(int i = 0; i < move->hitComplexity[frame]; i++){
		v = &move->hitbox[frame][i];
		v->x -= x; v->y -= y;
		SDL_FillRect (d, v, SDL_MapRGBA(d->format, 255, 0, 0, 127));
	}/*
	SDL_BlitSurface(a, &q, d, &q);
	SDL_BlitSurface(b, &q, d, &q);
	SDL_BlitSurface(c, &q, d, &q);
	*/
	if(SDL_SaveBMP(d, fname)) printf("You dun fucked up\n");
}

int main(int argc, char* argv[])
{
	interface z;
	char buffer[150];
	if(argc < 2) printf("Please supply a move\n");
	else{ 
		for(int i = 1; i < argc; i++){
			strcpy(buffer, argv[i]);
			buffer[strlen(argv[i]) - 3] = '\0';
			printf("%s\n", buffer);
			action * a = new action(buffer);

			for(int i = 0; i < a->frames; i++){
				writeImage(buffer, i, a);
			}
			delete a;
		}
	}
}

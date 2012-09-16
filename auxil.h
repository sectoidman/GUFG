/*Header file for some utility functions used in "GUFG",
 *Written by Ashley Fisher in 2012 for her project, "Picoclash",
 *and used with her permission
 */

#include <SDL/SDL.h>
#include <string>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include "compat.h"
#ifndef AUXIL
#define AUXIL
class aux{
	private:
		static void scale  (SDL_Surface* source, SDL_Surface* destination);
		static void bilinear_scale  (SDL_Surface* source, SDL_Surface* destination);
		static SDL_Surface* scale2x(SDL_Surface* source);
	public:
		static GLuint load_texture(std::string filename); 
		static GLuint surface_to_texture(SDL_Surface* source);
		static SDL_Surface* init_screen(int width, int height, int bpp);
		static void update_screen(SDL_Surface* source, SDL_Surface* destination);

		static SDL_Surface* load_image(std::string filename);
		static void apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination);
		static bool checkCollision(SDL_Rect, SDL_Rect);
		static int defineRectArray(char*, SDL_Rect*&); 
};
#endif

/*Header file for some utility functions used in "GUFG",
 *Written by Ashley Fisher in 2012 for her project, "Picoclash",
 *and used with her permission
 */

#include <SDL/SDL.h>
#include <string>
#include <SDL/SDL_opengl.h>
#include <stdio.h>
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

struct attractor{
	attractor() : x(0), y(0), type(0), radius(0), eventHorizon(0), effectCode(3) {}
	int x, y; //X-Yvalue. For globals this is only influenced by facingness, for local it is attractive force (negative for repulsive) based on mean Y
	unsigned int length; //How long the attractor holds
	int ID; //Who the attractor affects
	unsigned int type:2; //Type check variable. Type 0 is global, type 1 is linear decay, type 2 is half-life, and type 3 is a flat cut-off.
	int posX, posY; //Used to keep track of where local attractors actually are. Not set by the move itself, but used by the game later.
	int radius;	/*The radius at which the vector decay. Irrelevant (unchecked) for type 0, is the distance at which the vector loses one
			 *from its absolute value in type 1, the half-life point for type 2, and the cut-off point for type 3.
			 */
	int eventHorizon;
	int effectCode;
};

class vect
{
public:
	float x, y, z;
	void crossProduct(float, float, float, float, float, float);
};
#endif

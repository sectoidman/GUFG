#ifndef SHADERMAN_H
#define SHADERMAN_H

/* 
 * shader manager class
 *
 * Handles the loading, compiling, and linking of shaders. 
 */

#define NO_SDL_GLEXT

#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>
#include <GL/glew.h>

class shaderman {
	
	public:
		shaderman();
		~shaderman();
		void init();
		void load(const char* path, GLenum type);
		void link();
		void enable();
		void disable();
		void destroy();

	private:
		GLuint programID;
		std::vector<GLuint> shaderObjects;
};

#endif /* ndef SHADERMAN_H */

#ifndef SHADERMAN_H
#define SHADERMAN_H

/* 
 * shader manager class
 *
 * Handles the loading, compiling, and linking of shaders. 
 */

#include <stdexcept>
#include <vector>
#include <iostream>
#include <fstream>
#include <GL/gl.h>

class shaderman {
	
	public:
		shaderman();
		~shaderman();
		void load(const char* path, GLenum type);
		void link();
		void enable();
		void disable();

	private:
		GLuint programID;
		std::vector<GLuint> shaderObjects;
};

#endif /* ndef SHADERMAN_H */

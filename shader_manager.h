#ifndef SHADERMAN_H
#define SHADERMAN_H

/* 
 * shader manager class
 *
 * Handles the loading, compiling, and linking of shaders. 
 */

#include <vector>
#include <GL/glew.h>

class shaderman {
	
	public:
		shaderman();
		~shaderman();
		void load(const char* path, GLenum type);
		void link();
		void enable();
		void disable();

	private:
		const GLchar** read(const char* path);

		GLUint programID;
		vector<GLUint> shaderObjects;
};

#endif /* ndef SHADERMAN_H */

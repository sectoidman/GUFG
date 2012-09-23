#ifndef SHADERMAN_H
#define SHADERMAN_H

/* 
 * shader manager class
 *
 * Handles the loading, compiling, and linking of shaders. 
 */

#include <GL/glew.h>

class shaderman {
	
	public:
		shaderman();
		GLUint loadShader(const char* path, GLenum type);

	private:
		const GLchar** read(const char* path);
		GLUint programID;

};

#endif /* ndef SHADERMAN_H */

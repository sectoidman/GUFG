#include "shader_manager.h"

/* shader manager class */


shaderman::shaderman() {
	programID = glCreateProgram();

	if (programID == 0)
		throw std::runtime_error();
}

GLUint shaderman::loadShader(const char* path, GLenum type) {
	
	GLUint shaderID = 0;
	int status;
	int len;
	char* errormsg;

	shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, read(path), 0);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

	if (!status) {
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &len);
		errormsg = new char[len]; 
		glGetShaderInfoLog(shaderID, len, &len, errormsg);
		throw std::runtime_error();
	}

	return shaderID;
}


const GLchar** shaderman::read(const char* path) {

}


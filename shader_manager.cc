#include "shader_manager.h"

/* shader manager class */


shaderman::shaderman() 
{
	programID = glCreateProgram();

	if (programID == 0)
		throw std::runtime_error();
}

shaderman::~shaderman() 
{
	glDeleteProgram(programID);
}

/* 
 * load()
 *
 * Reads the shader source from file, compiles it into an 
 * object, and attaches it to the program, then 'deletes' 
 * it (that is, it sets the openGL delete flag on the object, 
 * which won't actually be deleted until the program is linked 
 * and the object is detatched).
 *  
 */

void shaderman::load(const char* path, GLenum type) 
{	
	GLUint shaderID;
	int status;
	int size;
	char* buf;
	std::ifstream source;

	source.open(path, std::ios::ate);
	if (source.fail()) {
		return NULL;
	}

	fsize = (int) source.tellg();
	source = new char[size];
	file.seekg(0, std::ios::beg);
	file.read(source, size);

	shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, (const GLchar**)buf, 0);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

	if (!status) {
		delete [] buf;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &size);
		errormsg = new char[size]; 
		glGetShaderInfoLog(shaderID, size, &size, errormsg);
		throw std::runtime_error(errormsg);
	}

	glAttachShader(shaderID, programID);
	shaderObjects.push_back(shaderID);
	glDeleteShader(shaderID);
	delete [] buf;
}

/*
 * link()
 *
 * Links the attached shader objects into the program, and then
 * detaches the shader objects to be garbage collected.
 *
 */

void shaderman::link() 
{
	int status;

	glLinkProgram(programID);
	glGetProgram(programID, GL_LINK_STATUS, &status)

	if (!status) {
		throw std::runtime_error("OpenGL shader linking error.");
	}

	for (int i = 0; i < shaderObjects.size(); i++) {
		glDetachShader(shaderObjects[i]);
	}

	shaderObjects.erase();
}

/*
 * enable()
 *
 * Applies the shader program to the screen. Will stay applied
 * until glUseProgram is called again (see 'disable()')
 *
 */

void shaderman::enable() 
{
	glUseProgram(programID);
}

/*
 * disable()
 *
 * Clears the applied shader program.
 *
 */

void shaderman::disable()
{
	glUseProgram(0);
}

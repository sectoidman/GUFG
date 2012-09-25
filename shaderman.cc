#include "shaderman.h"

/* shader manager class */


shaderman::shaderman() 
{
	programID = 0; 
}

shaderman::~shaderman() 
{

}

/* 
 * glCreateProgram and glDestroyProgram have to be called
 * after an openGL instance is created, or bad things happen.
 */

void shaderman::init()
{
	if (programID != 0) {
		glDeleteProgram(programID);
	}

	glewInit();
	programID = glCreateProgram();

	if (programID == 0)
		throw std::runtime_error("Failed to create shader program.");
}

void shaderman::destroy()
{
	glDeleteProgram(programID);
	programID = 0;	
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
 * Type is either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER.
 *  
 */

void shaderman::load(const char* path, GLenum type) 
{	
	GLuint shaderID;
	int status;
	int size;
	char* buf;
	std::ifstream source;

	source.open(path, std::ios::ate);
	if (source.fail()) {
		throw std::runtime_error("Couldn't open shader files.");
	}

	size = source.tellg();

	buf = new char [size];
	for (int i = 0; i < size; i++) {
		buf[i] = '\0';
	}

	source.seekg(0, std::ios::beg);
	source.clear();
	source.read(buf, size);

	shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, (const GLchar**)&buf, 0);
	glCompileShader(shaderID);
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

	if (!status) {
		delete [] buf;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &size);
		buf = new char[size]; 
		glGetShaderInfoLog(shaderID, size, &size, buf);
		throw new std::runtime_error(buf);
	}

	glAttachShader(programID, shaderID);
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
	int size;
	char* buf;

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &status);

	if (!status) {
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &size);
		buf = new char[size];
		glGetProgramInfoLog(programID, size, &size, buf);
		throw std::runtime_error(buf);
	}

	for (unsigned int i = 0; i < shaderObjects.size(); i++) {
		glDetachShader(programID, shaderObjects[i]);
	}

	shaderObjects.clear();
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

#ifdef ___MAC_OSX
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#define GUFG_TEXTURE_MODE GL_UNSIGNED_INT_8_8_8_8
#endif
#ifndef ___MAC_OSX
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#define GUFG_TEXTURE_MODE GL_UNSIGNED_BYTE
#endif

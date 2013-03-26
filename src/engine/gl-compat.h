#ifdef ___mac_osx
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#define ___gufg_tex_mode GL_UNSIGNED_INT_8_8_8_8
#endif

#ifndef ___mac_osx
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#define ___gufg_tex_mode GL_UNSIGNED_BYTE
#endif

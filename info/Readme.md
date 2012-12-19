#Ground Up Fighting Game 
GUFG is an open-source, gameplay-focused 2D fighting game developed from the bottom up.

The game is built using C++, SDL, and OpenGL without any RNGs.

##Building and testing GUFG
This fork of GUFG has currently been tested only on Linux.

1. Install the dependencies, which are available in most Linux distribution
    package repositories, or can be compiled manually from source.
  * Build time:
      * [SDL](http://www.libsdl.org/download-1.2.php)
      * [SDL\_image](http://www.libsdl.org/projects/SDL_image/)
      * [SDL\_mixer](http://www.libsdl.org/projects/SDL_mixer/)
      * [OpenGL](http://www.opengl.org/wiki/Getting_Started#Linux)
      * [Glut](http://freeglut.sourceforge.net/)
      * [SCons](http://www.scons.org/)
      * [SWIG](http://www.swig.org/)
  * Run time:
      * [Docopt](https://github.com/docopt/docopt)

  The runtime Python dependencies are available via `pip`, 
    and need not be installed before building GUFG.

2. Clone this repository
  (`git clone git@github.com:h-forrest-alexander/GUFG.git`) 
    and `cd` to it.

3. Run `scons`.
   All targets will be built, if necessary, and installed in `./dist/`:
    * `dist/`:
        * `info`: folder containing documentation such as this file
        * `runtime/_engine.so`: the compiled runtime library for the game
        * `gufg`:  a Python script to run a basic instance of the game
        * `content`:  a selection of freely available game data

  Due to SWIG output files that are mistaken by SCons scanners as source files,
   the build may fail if output from a previous build exists.
  This output is found in `.build` and can be cleaned using
    `rm .build` or `scons -c`.

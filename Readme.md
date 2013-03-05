#Ground Up Fighting Game 
GUFG is an open-source, gameplay-focused 2D fighting game engine
developed from the bottom up.

The game is built using C++, SDL, and OpenGL without any RNGs.

##Building and testing GUFG
The current version of GUFG is currently only known to successfully build and run on Linux.

###Building on Linux

1. Install the dependencies, which are available in most Linux distribution
    package repositories, or can be compiled manually from source.
  * To build the native Linux client:
      * [SDL](http://www.libsdl.org/download-1.2.php)
      * [SDL\_image](http://www.libsdl.org/projects/SDL_image/)
      * [SDL\_mixer](http://www.libsdl.org/projects/SDL_mixer/)
      * [OpenGL](http://www.opengl.org/wiki/Getting_Started#Linux)
      * [Glut](http://freeglut.sourceforge.net/)
      * [SCons](http://www.scons.org/)
2. Clone this repository
  (`git clone git@github.com:Advael/GUFG.git`) 
    and `cd` to it.

3. Run `scons` to produce the default target `./gufg`, an executable of the game.
   Other targets can be produced using `scons <target>` and include:
   * `bmpout`, a utility program to export bitmap images of
      character collision and hitboxes for moves.
   * `dist`, a directory containing the game and associated files,
     suitable for moving within the filesystem or including in an archive.

###Building On Linux with Clang
  1. Install all the dependencies above, plus Clang and LLVM.
  2. Build and install [libcxx](http://libcxx.llvm.org/).
  3. Clone the repository and build the targets 'gufg-clang' and 'bmpout-clang'.

###Building on Mac OS X
  * This currently doesn't work.

###Building for Windows
  * To cross compile for Windows (valid Fedora package names given):
      * mingw64-gcc-c++
      * mingw64-SDL
      * mingw64-SDL_image
      * mingw64-SDL_mixer
      * mingw64-freeglut
  * This currently doesn't work.
  * Windows build used to succeed with CMake.

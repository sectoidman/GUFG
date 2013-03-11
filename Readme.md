#Ground Up Fighting Game 

GUFG is an open-source,
           gameplay-focused,
           deterministic,
           2D fighting game engine
        developed from the bottom up.

The game is built using C++, SDL, and OpenGL.

##Building and testing GUFG

The current version of GUFG can be easily built on Linux and Mac OS X.

1. Install the dependencies:
  a) [SDL](http://www.libsdl.org/download-1.2.php),
     [SDL\_image](http://www.libsdl.org/projects/SDL_image/) and
     [SDL\_mixer](http://www.libsdl.org/projects/SDL_mixer/)
      * Linux: Available in most distributions.
      * Mac OS X: Framework packages on website.
  b) [OpenGL](http://www.opengl.org/wiki/Getting_Started#Linux) and 
     [GLUT](http://freeglut.sourceforge.net/)
      * Linux: Available in most distributions (`mesa`, `freeglut`).
      * Mac OS X: System component.
  d) [SCons](http://www.scons.org/)
      * Linux: Available in most distributions.
      * Mac OS X: In [MacPorts](http://www.macports.org/) and
                     [Homebrew](http://mxcl.github.com/homebrew/),
                  or install from source.
  e) Basic build software
      * Linux: Most distributions have a meta-package. For example:
        * Fedora: `su -c "yum groupinstall 'Development Tools'"`
        * Ubuntu: `su -c "apt-get install build-essential"`
      * Mac OS X: You must use clang++. See below.
  f) Compiling with clang++ (Required on Max OS X, optional on Linux):
      1. [clang](http://clang.llvm.org/)
          * Linux: Available in most distributions.
          * Mac OS X: Component of XCode, in the App Store.
      2. [libcxx](http://libcxx.llvm.org/)
          * Requires [`cmake`](http://www.cmake.org/) to build
            (available in most distributions,
             Mac OS X package available).
          * Linux:
            * `libcxx` provides features which are currently
              incompatible between `clang++` and GNU `libstdc++`.
            * Not all Linux systems succeed at building `libcxx`.
              * You will need recent versions of software such as
              `libstdc++` (and `libstdc++-static`) and `clang`.
              * Binary distributions known to provide
                sufficiently recent packages include
                [Fedora 18](docs/libcxx-fedora.md) and Arch Linux.
          * Mac OS X:
            * `libcxx` is a system package, but we use features
              not yet supported by the version installed in 10.8.
            * The existing library is necessary to boot,
              so we do not advise installing a more recent version
              of `libcxx` system-wide.
            * Instead, it can be [installed](docs/libcxx-mac.md)
              in `/usr/local/`.

2. Clone this repository
  (`git clone git@github.com:Advael/GUFG.git`) 
    and `cd` to it.

3. Run `scons` to build the game. Targets:
   * `bmpout`,
      a utility program which generates bitmap images of
      character collision and hitboxes for moves.
   * `gufg`,
      a dynamically linked game executable.
   * `bmpout.cl` and `gufg.cl`,
      executables compiled with clang++, instead of g++.
   * `bmpout.osx` and `gufg.osx`,
      executables which build (only) on Max OS X, and
      are also compiled with clang++.
###Building On Linux with clang++
  1. Install all the dependencies above, plus Clang and LLVM.
  2. Build and install [libcxx](http://libcxx.llvm.org/).
    * You may need to make some additional symlinks for the libs to be found:
      `ln -s /usr/lib{,64}/libc++.so
       ln -s /usr/lib{,64}/libc++.so.1
       ln -s /usr/lib{,64}/libc++.so.1.0`
    * Few Linux distributions provide `libcxx` packages, but it's very easy to build.
      Moreover, it's primarily useful for figuring out why the Mac builds are failing.
  3. Clone the repository and build the targets 'gufg-clang' and 'bmpout-clang'.
    * The build currently succeeds for bmpout and gufg.
    * Bmpout works (probably perfectly).
    * GUFG loads up the character select screen.
      * For some reason, both characters supply an initial input to open up the menu. This seems to be a weird artifact.
      * The game works! With no changes to chrono usage!
      * However, the game segfaults on building yellow.

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

#Ground Up Fighting Game 

GUFG is an open-source,
           gameplay-focused,
           deterministic,
           2D fighting game engine
        developed from the ground up and
        built using C++, SDL, and OpenGL.

##Building and testing GUFG on Linux and Mac OS X

### Windows build?

* We had a Windows build working, using CMake.
* We are trying to get it working using SCons.

The current version of GUFG can be easily built on Linux and Mac OS X.

1. Install the dependencies:

  a) [`SDL`](http://www.libsdl.org/download-1.2.php),
     [`SDL\_image`](http://www.libsdl.org/projects/SDL_image/) and
     [`SDL\_mixer`](http://www.libsdl.org/projects/SDL_mixer/)
      * Linux: Available in most distributions.
      * Mac OS X: Framework packages on website.

  b) [`OpenGL`](http://www.opengl.org/wiki/Getting_Started#Linux) and
     [`GLUT`](http://freeglut.sourceforge.net/)
      * Linux: Available in most distributions (`mesa`, `freeglut`).
      * Mac OS X: System component.

  c) [`Python`](http://www.python.org/).
      Installed on most Linux distributions and Mac OS X by default.

  d) Basic build software
      * Linux: Most distributions have a meta-package. For example:
        * Fedora: `su -c "yum groupinstall 'Development Tools'"`
        * Ubuntu: `su -c "apt-get install build-essential"`
      * Mac OS X: Use `clang++`. See below.

  e) Compiling with `clang++`
    (Required on Max OS X, optional on Linux):
      1. [`clang`](http://clang.llvm.org/)
          * Linux: Available in most distributions.
          * Mac OS X: Component of XCode, in the App Store.
      2. [`libcxx`](http://libcxx.llvm.org/)
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
                [Fedora 18](src/scripts/libcxx-fedora.sh) and Arch Linux.
          * Mac OS X:
            * `libcxx` is a system package, but we use features
              not yet supported by the version installed in 10.8.
            * The existing library is necessary to boot,
              so we do not advise installing a more recent version
              of `libcxx` system-wide.
            * Instead, it can be [installed](src/scripts/libcxx-mac.sh)
              in `/usr/local/`, where our build will find it.


2. Clone this repository
  (`git clone git@github.com:Advael/GUFG.git`) and `cd` to it.


3. Run `./scons` to build the game. Targets:
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

## Running the game
* `cd` into the folder containing the game.
   Character assets are loaded relative to this folder.
* `./gufg` (or `gufg.cl` or `gufg.osx`)
  * Arguments:
    * `eleven`: Play an unlimited time match.
    * `<replay file name>`: Playback a saved replay.

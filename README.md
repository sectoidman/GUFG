#Ground Up Fighting Game 
GUFG is an open-source, gameplay-focused 2D fighting game developed from the bottom up.

The game is built using C++, SDL, and OpenGL without any random number generators.

This repository contain the complete game, including source code and data files such as art assets in current use.

##Building GUFG
GUFG currently works on Linux and Windows.
###Building on Linux
1. Install GUFG's dependencies. 
GUFG should work with up-to-date packages from your Linux distribution or dependencies built from source.
    * [SDL](http://www.libsdl.org/download-1.2.php)
    * [SDL\_image](http://www.libsdl.org/projects/SDL_image/)
    * [OpenGL](http://www.opengl.org/wiki/Getting_Started#Linux)
    * [Glut](http://freeglut.sourceforge.net/)
2. Clone this repository (`git clone git@github.com:Advael/GUFG.git`) and `cd` to it.
3. `cmake .; make; ./gufg`

###Building on Windows
Try out the [installer](https://github.com/Advael/GUFG/blob/installer/GUFG-0.0.1-win32.exe?raw=true)!

Or, you can attempt to build the latest version from source.

1. Download and install [CMake](http://www.cmake.org/files/v2.8/cmake-2.8.8-win32-x86.exe), and add it to your PATH when prompted to do so.
2. Download and install [MinGW](http://sourceforge.net/projects/mingw/files/latest/download), using [this distribution](http://nuwen.net/mingw.html). SDL and SDL\_image depend on zlib and libpng, so make sure your MinGW environment includes these libraries.
3. Add MinGW's `bin` and `include` directories to your PATH manually.
4. Download the development packages for [SDL](http://www.libsdl.org/release/SDL-devel-1.2.15-mingw32.tar.gz) and [SDL\_image](http://www.libsdl.org/projects/SDL_image/release/SDL_image-devel-1.2.12-VC.zip) and install the `.dll` and `.a` library files in your MinGW's `bin` directory, and the `.h` files in MinGW's `include` directory.
5. Download the OpenGL libraries and [install them (see section 2.060)](http://www.opengl.org/archives/resources/faq/technical/gettingstarted.htm).
6. Clone this Git repository (git@github.com:Advael/GUFG.git).
7. Use the CMake GUI to Configure and Generate with the directory to which you cloned the repository selected for both "source directory" and "build directory." 
8. Use `cmd.exe` to `cd` to the repository directory.
9. Run `make`.
10. Run `gufg.exe`.


##Contributing to GUFG

###Discussion on Google Documents
Collaboration via Git and GitHub are the primary means of development for GUFG.
However, design and discussion of game elements such as new characters is largely located in a Google Docs collection.
While some of the information there is being migrated to the Git repository and GitHub wiki, Google Docs collaborative editing has been, and may remain, important to GUFG design.

For access to the collection, please email eshink@gmail.com.


###Playtesting
Try out a build described above! 
If it doesn't work in some context, we'd like to know.

While the Alpha branch currently includes only 2 characters, they are relatively "finished" and can be play tested.

###Character Development
[Starting a new character should be easy](https://github.com/h-forrest-alexander/GUFG/wiki/How-To-Develop-A-Character). Let us know if you run into any problems.

###Engine Development


###Testing and Contributing on Mac OS X and Clang/LLVM
We have only taken a preliminary look at GUFG on Mac OS X.
While GUFG should, in theory, be completely Mac OS X compatible,[Clang/LLVM reject some of the code due to their stricter C++ standards compliance](https://github.com/h-forrest-alexander/GUFG/issues/1).
GUFG has been partially built on Mac OS X 10.8.1, XCode 4.4 and the Command Line Tools (found in the XCode Preferences dialog), CMake 2.8.9, SDL 1.2.15 and SDL\_image 1.2.12. 
Right now, the first block to a working Mac OS X build (and possibly even a bundle) is some variable length arrays necessary to the game logic. 
In addition, [GUFG cannot be currently built with Linux and Clang/LLVM](https://github.com/h-forrest-alexander/GUFG/issues/2) due to upstream incompatibilities with GCC stdlibc++.


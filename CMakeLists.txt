# Copyright (C) 2012 Joseph Woodruff/The GUFG Programming Team
#
# CMake build file, cobbled together from past experience and the SDL wiki


cmake_minimum_required (VERSION 2.6)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

project (gufg)

set (gufg_VERSION_MAJOR 0)
set (gufg_VERSION_MINOR 0)
set (gufg_VERSION_PATCH 1)

set (SOURCES 	keytest.cc 
		auxil.cc 
		character.cc 
		buildchar.cc
		draw.cc 
		hitstun.cc
		interface.cc 
		move.cc 
		player.cc 
		red.cc 
		special.cc 
		trie.cc 
		utility.cc)

set (CMAKE_CXX_FLAGS "-O2 -Wno-write-strings -g -Wall -Woverloaded-virtual -Wctor-dtor-privacy")

find_package (SDL REQUIRED)
find_package (SDL_image REQUIRED)

link_libraries (
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   SDLmain # not included in SDL_LIBRARY variable
)

add_executable (keytest 
		WIN32 
		MACOSX_BUNDLE
		${SOURCES})

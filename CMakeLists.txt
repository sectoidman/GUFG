# Copyright (C) 2012 Joseph Woodruff/The GUFG Programming Team
#
# CMake build file, cobbled together from past experience and the SDL wiki


cmake_minimum_required (VERSION 2.6)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH})

project (gufg)

set (gufg_VERSION_MAJOR 0)
set (gufg_VERSION_MINOR 0)
set (gufg_VERSION_PATCH 1)

set (SOURCES	auxil.cc 
		character.cc 
		draw.cc 
		hitstun.cc
		interface.cc 
		action.cc 
		player.cc 
		red.cc
		yellow.cc 
		special.cc 
		aerial.cc
		utility.cc
		trie.cc) 


set (CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_NANOSLEEP -O2 -g -Wall -Wctor-dtor-privacy -Wno-format-contains-nul --std=c++0x")
set (QT_USE_QTOPENGL TRUE)

find_package (SDL REQUIRED)
find_package (SDL_image REQUIRED)
find_package (GLUT REQUIRED)
find_package (OpenGL REQUIRED)
link_libraries (
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${OPENGL_LIBRARIES}
   SDLmain # not included in SDL_LIBRARY variable
)

if (WIN32)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32")
	find_package (Boost 1.45 COMPONENTS thread chrono system REQUIRED)
	INCLUDE_DIRECTORIES (${BOOST_INCLUDE_DIR})
	LINK_DIRECTORIES (${Boost_LIBRARY_DIRS})
	link_libraries (
		${Boost_THREAD_LIBRARY} 
		${Boost_CHRONO_LIBRARY}
		${Boost_SYSTEM_LIBRARY}
	)
endif (WIN32)

add_executable (gufg 
		WIN32 
		MACOSX_BUNDLE
		keytest.cc
		${SOURCES})

add_executable (bmpout
		WIN32 
		MACOSX_BUNDLE
		bmp.cc
		${SOURCES})

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

if (WIN32)

	INSTALL(TARGETS gufg
		RUNTIME
		DESTINATION .
		COMPONENT gufg)

	INSTALL(DIRECTORY White
		DESTINATION White
		COMPONENT gufg)

	INSTALL(DIRECTORY Red
		DESTINATION Red
		COMPONENT gufg)

	INSTALL(DIRECTORY Yellow
		DESTINATION Yellow
		COMPONENT gufg)

	INSTALL(DIRECTORY Misc
		DESTINATION Misc
		COMPONENT gufg)

	INSTALL(FILES README
		DESTINATION .)

	INSTALL(FILES CMakeLists.txt .gitignore .gitmodules
		DESTINATION src
		COMPONENT source)

	INSTALL(DIRECTORY .
		DESTINATION src
		COMPONENT source
		FILES_MATCHING
			PATTERN "*.cc"
			PATTERN "*.h"
			PATTERN "NSIS/*" EXCLUDE)

	INSTALL(FILES dumpboxes.sh
		DESTINATION .
		COMPONENT utilities)

	INSTALL(TARGETS bmpout
		RUNTIME
		DESTINATION .
		COMPONENT utilities)

	set (CPACK_PACKAGE_NAME "GUFG")
	set (CPACK_PACKAGE_VENDOR "The GUFG Team")
	set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "GUFG - A General Universal Fighting Game")
	set (CPACK_PACKAGE_VERSION "0.0.1")
	set (CPACK_PACKAGE_VERSION_MAJOR "0")
	set (CPACK_PACKAGE_VERSION_MINOR "0")
	set (CPACK_PACKAGE_VERSION_PATCH "1")
	set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/COPYING")
	set (CPACK_RESOURCE_FILE_README "${PROJECT_BINARY_DIR}/README")
	set (CPACK_PACKAGE_INSTALL_DIRECTORY "GUFG ${gufg_VERSION_MAJOR}.${gufg_VERSION_MINOR}.${gufg_VERSION_PATCH}")
	set (CPACK_COMPONENTS_ALL gufg utilities source)

	set (CPACK_COMPONENT_GUFG_DISPLAY_NAME "General Universal Fighting Game")
	set (CPACK_COMPONENT_UTILITIES_DISPLAY_NAME "Character Creation Utilities")
	set (CPACK_COMPONENT_SOURCE_DISPLAY_NAME "Source code")

	set (CPACK_COMPONENT_GUFG_DESCRIPTION 
	     "A fighting game.")
	set (CPACK_COMPONENT_UTILITIES_DESCRIPTION
	     "A set of utilities to aid in creating or modifying characters.")
	set (CPACK_COMPONENT_SOURCE_DESCRIPTION
	     "The source code of this project.")


	if (WIN32 AND NOT UNIX)
	  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/creative\\\\InstallIcon.bmp")
	  set (CPACK_NSIS_INSTALLED_ICON_NAME "gufg.exe")
	  set (CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
	  set (CPACK_NSIS_HELP_LINK "http:\\\\\\\\advael.github.com\\\\gufg")
	  set (CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\advael.github.com\\\\gufg")
	  set (CPACK_NSIS_CONTACT "eshink@gmail.com")
	  set (CPACK_NSIS_MODIFY_PATH ON)
	else (WIN32 AND NOT UNIX)
	  set (CPACK_STRIP_FILES "gufg bmpout")
	endif (WIN32 AND NOT UNIX)

	set (CPACK_PACKAGE_EXECUTABLES "gufg" "General Universal Fighting Game")

	INCLUDE (CPACK)
endif (WIN32)

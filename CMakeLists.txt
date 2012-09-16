# Copyright (C) 2012 Joseph Woodruff/The GUFG Programming Team
#
# CMake build file, cobbled together from past experience and the SDL wiki

cmake_minimum_required(VERSION 2.6)

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
		projectile.cc
		trie.cc) 

set (CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_NANOSLEEP -O2 -g -Wall -Wctor-dtor-privacy -Wno-format-contains-nul -std=c++11 -pedantic")
set (QT_USE_QTOPENGL TRUE)

find_package (SDL REQUIRED)
find_package (SDL_image REQUIRED)
find_package (OpenGL REQUIRED)
find_package (SDL_mixer REQUIRED)
link_libraries (
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${OPENGL_LIBRARIES}
	${SDLMIXER_LIBRARY}
	SDLmain # not included in SDL_LIBRARY variable
)

if (WIN32)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32 -static-libstdc++ -static-libgcc")
	set (CMAKE_EXE_LINKER_FLAGS "-static")

	find_package (PNG REQUIRED)
	find_package (ZLIB REQUIRED)
endif (WIN32)

if (APPLE)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_APPLE -stdlib=libc++")
endif (APPLE)


add_executable (gufg 
		WIN32
		MACOSX_BUNDLE
		keytest.cc
		${SOURCES})

add_executable (bmpout
		WIN32
		MACOSX_BUNDLE
		EXCLUDE_FROM_ALL
		bmp.cc
		${SOURCES})

INSTALL(TARGETS gufg
	RUNTIME
	DESTINATION .
	COMPONENT gufg)

INSTALL(DIRECTORY White
	DESTINATION .
	COMPONENT gufg)

INSTALL(DIRECTORY Red
	DESTINATION .
	COMPONENT gufg)

INSTALL(DIRECTORY Yellow
	DESTINATION .
	COMPONENT gufg)

INSTALL(DIRECTORY Misc
	DESTINATION .
	COMPONENT gufg)

INSTALL(DIRECTORY scripts
	DESTINATION .
	COMPONENT source
	DESTINATION .
	COMPONENT utilities)

LIST(GET SDL_LIBRARY 1 ACTUAL_SDL_LIBRARY)

INSTALL(FILES ${ACTUAL_SDL_LIBRARY} 
		${SDLIMAGE_LIBRARY} 
		${PNG_LIBRARY} 
		${ZLIB_LIBRARY}
		${SDLMIXER_LIBRARY}
	DESTINATION .
	COMPONENT gufg
	DESTINATION .
	COMPONENT utilities)

INSTALL(FILES README COPYING
	DESTINATION .
	COMPONENT gufg
	DESTINATION .
	COMPONENT utilities
	DESTINATION .
	COMPONENT source)

INSTALL(FILES CMakeLists.txt .gitignore .gitmodules
       	DESTINATION src
	COMPONENT source)


INSTALL(DIRECTORY .
	DESTINATION src
	COMPONENT source
		PATTERN "*.cc"
		PATTERN "*.h"
		PATTERN "_CPack_Packages" EXCLUDE
		PATTERN "*cmake*" EXCLUDE
		PATTERN "*CMake*" EXCLUDE
		PATTERN "*.exe" EXCLUDE
		PATTERN "*.txt" EXCLUDE
		PATTERN "*.txt~" EXCLUDE
		PATTERN ".git" EXCLUDE)

set (CPACK_PACKAGE_NAME "GUFG")
set (CPACK_PACKAGE_VENDOR "The GUFG Team")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY
	"GUFG - A Ground Up Fighting Game")
set (CPACK_PACKAGE_VERSION "0.0.1")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "1")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${PROJECT_BINARY_DIR}/README")
set (CPACK_PACKAGE_INSTALL_DIRECTORY "GUFG ${gufg_VERSION_MAJOR}.${gufg_VERSION_MINOR}.${gufg_VERSION_PATCH}")
set (CPACK_COMPONENTS_ALL gufg utilities source)

set (CPACK_COMPONENT_GUFG_DISPLAY_NAME "Ground Up Fighting Game")
set (CPACK_COMPONENT_UTILITIES_DISPLAY_NAME "Character Creation Utilities")
set (CPACK_COMPONENT_SOURCE_DISPLAY_NAME "Source code")
set (CPACK_COMPONENT_GUFG_DESCRIPTION "A fighting game.")
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
	set (CPACK_NSIS_MODIFY_PATH OFF)
	set (CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
else (WIN32 AND NOT UNIX)
	set (CPACK_STRIP_FILES "gufg bmpout")
endif (WIN32 AND NOT UNIX)

set (CPACK_PACKAGE_EXECUTABLES "gufg" "Ground Up Fighting Game")

INCLUDE (CPack)

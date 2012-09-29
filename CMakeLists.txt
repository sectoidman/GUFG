# Copyright (C) 2012 Joseph Woodruff/H Forrest Alexander/The GUFG Programming Team
#
# CMake build file, cobbled together from past experience and the SDL wiki
#And modified by me, probably to the point of not working right.

cmake_minimum_required(VERSION 2.6)

set (CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH} 
	${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

project (gufg)

set (gufg_VERSION_MAJOR 0)
set (gufg_VERSION_MINOR 0)
set (gufg_VERSION_PATCH 1)

aux_source_directory (	src/engine ENGINE)
aux_source_directory (	src/characters CHARACTERS)

set (SOURCES	${ENGINE}
		${CHARACTERS}) 

set (CMAKE_CXX_FLAGS "-D_GLIBCXX_USE_NANOSLEEP -O2 -g -Wall -Wctor-dtor-privacy -Wno-format-contains-nul -std=c++11 -pedantic")
set (QT_USE_QTOPENGL TRUE)

find_package (SDL REQUIRED)
find_package (SDL_image REQUIRED)
find_package (SDL_mixer REQUIRED)
find_package (OpenGL REQUIRED)
link_libraries (
	SDLmain # not included in SDL_LIBRARY variable
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${SDLMIXER_LIBRARY}
	${OPENGL_LIBRARIES})

if (WIN32)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32 -static-libstdc++ -static-libgcc")
	set (CMAKE_EXE_LINKER_FLAGS "-static")

	find_package (PNG REQUIRED)
	find_package (OggVorbis REQUIRED)
	find_package (ZLIB REQUIRED)
endif (WIN32)

if (APPLE)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_APPLE -stdlib=libc++")
endif (APPLE)


add_executable (gufg 
		WIN32
		MACOSX_BUNDLE
		src/game.cc
		${SOURCES})

add_executable (bmpout
		WIN32
		MACOSX_BUNDLE
		EXCLUDE_FROM_ALL
		src/bmp.cc
		${SOURCES})

find_package(Doxygen)
if (DOXYGEN_FOUND)
	configure_file(Doxyfile.in
	${PROJECT_BINARY_DIR}/Doxyfile @ONLY IMMEDIATE)
	add_custom_target(doc
	${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
endif(DOXYGEN_FOUND)

INSTALL(TARGETS gufg
	RUNTIME
	DESTINATION package
	COMPONENT gufg)

INSTALL(DIRECTORY characters
	DESTINATION package
	COMPONENT gufg)

INSTALL(DIRECTORY art
	DESTINATION package
	COMPONENT gufg)

INSTALL(DIRECTORY scripts
	DESTINATION package
	COMPONENT source
	DESTINATION .
	COMPONENT utilities)

LIST(GET SDL_LIBRARY 1 ACTUAL_SDL_LIBRARY)

INSTALL(FILES ${ACTUAL_SDL_LIBRARY} 
		${SDLIMAGE_LIBRARY} 
		${PNG_LIBRARY}
		${ZLIB_LIBRARY}
		${SDLMIXER_LIBRARY}
		${VORBIS_LIBRARY}
	DESTINATION package
	COMPONENT gufg
	DESTINATION package
	COMPONENT utilities)

INSTALL(FILES README COPYING
	DESTINATION package
	COMPONENT gufg
	DESTINATION package
	COMPONENT utilities
	DESTINATION package
	COMPONENT source)

INSTALL(FILES CMakeLists.txt .gitignore .gitmodules
       	DESTINATION package/src
	COMPONENT source)


INSTALL(DIRECTORY src/
	DESTINATION package/src
	COMPONENT source
		PATTERN "*.cc"
		PATTERN "*.h")

set (CPACK_PACKAGE_NAME "GUFG")
set (CPACK_PACKAGE_VENDOR "The GUFG Team")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY
	"GUFG - A Ground Up Fighting Game")
set (CPACK_PACKAGE_VERSION "0.0.1")
set (CPACK_PACKAGE_VERSION_MAJOR "0")
set (CPACK_PACKAGE_VERSION_MINOR "0")
set (CPACK_PACKAGE_VERSION_PATCH "1")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/COPYING")
set (CPACK_RESOURCE_FILE_README "${PROJECT_BINARY_DIR}/README.md")
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

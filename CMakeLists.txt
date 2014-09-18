# Copyright Linkoping University 2011
# SGCT Project Authors see Authors.txt

cmake_minimum_required(VERSION 2.8.8)
include(CheckCXXCompilerFlag)

SET(APP_NAME Achdome)
SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")


PROJECT(${APP_NAME})

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/includes)
set(PROJECT_LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(PROJECT_SHADERS_DIR ${PROJECT_SOURCE_DIR}/shaders)



include_directories(${PROJECT_INCLUDE_DIR})
file(GLOB PROJECT_LIBS_FILES ${PROJECT_LIBS_DIR}/*.cpp)
add_library(INT_LIBS
  ${PROJECT_LIBS_FILES}
)
add_definitions(-DSHADER_PATH=${PROJECT_SHADERS_DIR})

add_executable(${APP_NAME}
${PROJECT_SOURCE_DIR}/main.cpp)

set_target_properties(${APP_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXECUTABLE_OUTPUT_PATH}/debug
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH}/release
)

if( APPLE  )
	option(SGCT_CPP11 "Use c++11" OFF)
	mark_as_advanced(SGCT_CPP11)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
	if(CMAKE_GENERATOR STREQUAL Xcode)
		set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")
	endif()
endif()

find_library(SGCT_RELEASE_LIBRARY
	NAMES sgct libsgct libsgct_cpp11 REQUIRED)
find_library(SGCT_DEBUG_LIBRARY
	NAMES sgctd libsgctd libsgct_cpp11d REQUIRED)

if( WIN32 )
	find_path(SGCT_INCLUDE_DIRECTORY
		NAMES sgct
		PATHS $ENV{SGCT_ROOT_DIR}/include NO_DEFAULT_PATH
		REQUIRED)
else()
	find_path(SGCT_INCLUDE_DIRECTORY
		NAMES sgct
		PATH_SUFFIXES sgct
		PATHS $ENV{SGCT_ROOT_DIR}/include
		REQUIRED)
endif()

set(SGCT_LIBS
	debug ${SGCT_DEBUG_LIBRARY}
	optimized ${SGCT_RELEASE_LIBRARY}
)

find_package(OpenGL REQUIRED)

include_directories(${SGCT_INCLUDE_DIRECTORY})

if( MSVC )
	set(LIBS
		${SGCT_LIBS}
		${OPENGL_LIBRARY}
		ws2_32
	)
elseif( WIN32 ) #MINGW or similar
	set(LIBS
		${SGCT_LIBS}
		${OPENGL_LIBRARY}
		ws2_32
	)
elseif( APPLE )
	find_library(COCOA_LIBRARY Cocoa REQUIRED)
	find_library(IOKIT_LIBRARY IOKit REQUIRED)
	find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)

	set(LIBS
		${SGCT_LIBS}
		${OPENGL_LIBRARY}
		${COCOA_LIBRARY}
		${IOKIT_LIBRARY}
		${COREVIDEO_LIBRARY}
	)
else() #linux
	find_library(X11_LIBRARY X11 REQUIRED)
    find_library(XRANDR_LIBRARY Xrandr REQUIRED)
	find_library(XI_LIBRARY Xi REQUIRED)

	find_library(XXF86_LIBRARY Xxf86vm REQUIRED)
	find_library(PTHREAD_LIBRARY pthread REQUIRED)

	set(LIBS
		${SGCT_LIBS}
		${OPENGL_LIBRARY}
		${X11_LIBRARY}
		${XRANDR_LIBRARY}
		${XI_LIBRARY}
		${XXF86_LIBRARY}
		${PTHREAD_LIBRARY}
	)
endif()

if( MSVC )
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LTCG")

		if( "${MSVC_VERSION}" LESS 1600 ) #less than visual studio 2010
		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags
			${CMAKE_EXE_LINKER_FLAGS_DEBUG})
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${replacementFlags}" )

		STRING(REPLACE "INCREMENTAL:YES" "INCREMENTAL:NO" replacementFlags
			${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${replacementFlags}" )
	else()
		STRING(REPLACE "INCREMENTAL" "INCREMENTAL:NO" replacementFlags
			${CMAKE_EXE_LINKER_FLAGS_DEBUG})
		SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "${replacementFlags}" )

		STRING(REPLACE "INCREMENTAL" "INCREMENTAL:NO" replacementFlags
			${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
		SET(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${replacementFlags}" )
	endif()

	#MESSAGE(STATUS "flags: ${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
endif()

if(MSVC AND NOT "${MSVC_VERSION}" LESS 1400)
	add_definitions( "/MP" )
endif()

if( WIN32 )
	add_definitions(-D__WIN32__)
elseif( APPLE  )
	add_definitions(-D__APPLE__)
	if( SGCT_CPP11 )
    	set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
		add_definitions(-D__SGCT_CPP11__)
	else()
		set(CMAKE_CXX_FLAGS "-std=c++0x -stdlib=libstdc++ ${CMAKE_CXX_FLAGS}")
	endif()
else()
	add_definitions(-D__LINUX__)
endif()

if( CMAKE_COMPILER_IS_GNUCXX )
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -s")
endif()

set(LIBS ${LIBS} INT_LIBS)

target_link_libraries(${APP_NAME} ${LIBS})

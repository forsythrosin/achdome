# Copyright Linkoping University 2011
# SGCT Project Authors see Authors.txt

cmake_minimum_required(VERSION 2.8.12)
SET(APP_NAME Achdome)
include(CheckCXXCompilerFlag)
include(ExternalProject)
project(${APP_NAME} CXX)
#SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
#SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

if (NOT CMAKE_BUILD_TYPE)
	message(STATUS "No build type selected, default to Debug")
	set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(PROJECT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/includes)
set(PROJECT_LIBS_DIR ${PROJECT_SOURCE_DIR}/libs)
set(PROJECT_EXTERNAL_DIR ${PROJECT_SOURCE_DIR}/external)
set(PROJECT_SHADERS_DIR ${PROJECT_SOURCE_DIR}/shaders)
set(PROJECT_FONTS_DIR ${PROJECT_SOURCE_DIR}/fonts)
set(PROJECT_WEBRESOURCES_DIR ${PROJECT_SOURCE_DIR}/webResources)

# First for the generic no-config case (e.g. with mingw)

include_directories(${PROJECT_INCLUDE_DIR})
file(GLOB PROJECT_LIBS_FILES ${PROJECT_LIBS_DIR}/*.cpp)
add_library(INT_LIBS
  ${PROJECT_LIBS_FILES}
)

add_definitions(-DSHADER_PATH=${PROJECT_SHADERS_DIR})
add_definitions(-DFONTS_PATH=${PROJECT_FONTS_DIR})
add_definitions(-DWEBRESOURCES_PATH=${PROJECT_WEBRESOURCES_DIR})

add_executable(${APP_NAME}
${PROJECT_SOURCE_DIR}/main.cpp)

#set_target_properties(${APP_NAME} PROPERTIES
#	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${EXECUTABLE_OUTPUT_PATH}/debug
#	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${EXECUTABLE_OUTPUT_PATH}/release
#)

# Include external project
set(websocketpp_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/websocketpp")
set(websocketpp_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${websocketpp_INSTALL_DIR} ")

ExternalProject_Add(websocketpp
	SOURCE_DIR ${PROJECT_EXTERNAL_DIR}/websocketpp
CMAKE_ARGS -DENABLE_CPP11=NO ${websocketpp_CMAKE_ARGS})

include_directories(${PROJECT_EXTERNAL_DIR}/websocketpp)
include_directories(${PROJECT_EXTERNAL_DIR}/picojson)
include_directories(${PROJECT_EXTERNAL_DIR}/readerwriterqueue)


# Include boost
if(MSVC)
	add_definitions(/Gr)
endif()
set(Boost_USE_STATIC_LIBS ON)
add_definitions(-DBOOST_ALL_NO_LIB)
find_package(Boost COMPONENTS thread system REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

if(${COMPILER_SUPPORTS_CXX11})
    set(SGCT_CPP11 ON)
endif()

if( APPLE  )
	option(SGCT_CPP11 "Use c++11" ON)
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
	if(CMAKE_GENERATOR STREQUAL Xcode)
		set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")
	endif()
endif()

find_library(SGCT_RELEASE_LIBRARY
	NAMES sgct libsgct libsgct_cpp11 REQUIRED)
find_library(SGCT_DEBUG_LIBRARY
	NAMES sgctd libsgctd libsgct_cpp11d REQUIRED)

if( WIN32 OR CYGWIN )
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
elseif( WIN32 OR CYGWIN ) #MINGW or similar
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

if( WIN32 OR CYGWIN )
	add_definitions(-D__WIN32__)
    add_definitions(-D_WIN32)
    add_definitions(-DWIN32)

	if( SGCT_CPP11 )
		set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
		add_definitions(-D__SGCT_CPP11__)
	else()
		set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
	endif()
elseif( APPLE )
	add_definitions(-D__APPLE__)
	add_definitions(-Wno-deprecated)
	add_definitions(-Wno-deprecated-declarations)
	add_definitions(-std=c++11)
else()
	add_definitions(-D__LINUX__)
	if( SGCT_CPP11 )
    set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
		add_definitions(-D__SGCT_CPP11__)
	else()
		set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
	endif()
endif()

set(LIBS ${LIBS} INT_LIBS)
target_link_libraries(INT_LIBS ${Boost_LIBRARIES} )
add_dependencies(INT_LIBS websocketpp)
target_link_libraries(${APP_NAME} ${LIBS} )



set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set( RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
set_target_properties(${APP_NAME}
						PROPERTIES
						CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
		set( RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR} )
		set_target_properties(${APP_NAME}
							PROPERTIES
							RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_BINARY_DIR})

endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )
get_target_property(exec_location ${APP_NAME} "CMAKE_RUNTIME_OUTPUT_DIRECTORY")

add_custom_target(resources)
add_custom_command(TARGET resources PRE_BUILD
										COMMAND ${CMAKE_COMMAND} -E copy_directory
										${PROJECT_SHADERS_DIR} "${exec_location}/shaders"
										)

add_custom_command(TARGET resources PRE_BUILD
										COMMAND ${CMAKE_COMMAND} -E copy_directory
										${PROJECT_FONTS_DIR} "${exec_location}/fonts"
										)

set(PROJECT_CONFIG_DIR ${CMAKE_SOURCE_DIR}/configs)
add_custom_command(TARGET resources PRE_BUILD
										COMMAND ${CMAKE_COMMAND} -E copy_directory
										${PROJECT_CONFIG_DIR} "${exec_location}/configs"
                    )

add_custom_target(npmModules
									COMMAND npm install --prefix ${PROJECT_WEBRESOURCES_DIR} ${PROJECT_WEBRESOURCES_DIR})

add_custom_target(webResources
									COMMAND grunt --base ${PROJECT_WEBRESOURCES_DIR}
																--gruntfile ${PROJECT_WEBRESOURCES_DIR}/Gruntfile.js)
add_dependencies(webResources npmModules)
add_dependencies(resources webResources)


add_dependencies(${APP_NAME} resources)

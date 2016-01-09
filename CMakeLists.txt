if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Do not build in-tree. Create a build folder, and run cmake there.")
endif()

cmake_minimum_required(VERSION 2.8.4)
set(PROJECT_NAME "halfmapper")
project(${PROJECT_NAME})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${${PROJECT_NAME}_SOURCE_DIR}/CMakeModules")

#Find the required packages, and add them to the includes list.
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})

#Set the CXXFLAGS for Clang, GCC or MINGW.
#Should cover OSX, Linux and MINGW Windows
if(UNIX OR MINGW)
  set(CMAKE_CXX_FLAGS "-O2 -Wall -Wpedantic -Wextra")
endif(UNIX OR MINGW)


#Set compiller flags when on MSVC Windows.
if(MSVC)
  add_definitions("/W4")
endif(MSVC)


#Add all source and header files for all platforms.
file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.cpp"
)

file(GLOB TINYXML2_FILES
	"thirdparty/tinyxml2/tinyxml2.cpp"
	"thirdparty/tinyxml2/tinyxml2.h"
)

#Make sure tinyxml is in its own group in Visual Studio, and in the include path.
SOURCE_GROUP(tinyxml2 FILES ${TINYXML2_FILES})
include_directories("thirdparty/tinyxml2")

#Add Windows resources.
if(MSVC OR MINGW)
  file(GLOB RESOURCE_FILES
    "src/*.ico"
    "src/*.rc"
  )
endif(MSVC OR MINGW)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${TINYXML2_FILES} ${RESOURCE_FILES})


#Add link libraries.
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})

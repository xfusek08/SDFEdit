cmake_minimum_required(VERSION 3.13.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 )

project(
  imguiVars
  VERSION 1.0.0
  )

#set these variables to *.cpp, *.c, ..., *.h, *.hpp, ...
set(SOURCES
  src/${PROJECT_NAME}/imguiVars.cpp
  src/${PROJECT_NAME}/addVarsLimits.cpp
  )
set(PRIVATE_INCLUDES
  )
set(PUBLIC_INCLUDES
  src/${PROJECT_NAME}/imguiVars.h
  src/${PROJECT_NAME}/addVarsLimits.h
  )
set(INTERFACE_INCLUDES )

#list of libraries to find
#it should contain list of "lists"
#format:
#set(ExternLibraries
#  A\\ B\\ C\\ D
#  E\\ F\\ G
#  )
#It will be converted into:
#find_package(A B C D)
#find_package(E F G)
#If version is specified, it has to be the second parameter (B)
set(ExternPrivateLibraries )
set(ExternPublicLibraries
  Vars\\ 1.0.0\\ CONFIG\\ REQUIRED
  imguiDormon\\ 1.0.0\\ CONFIG\\ REQUIRED
  glm\\ REQUIRED
  )
set(ExternInterfaceLibraries)

#set these variables to targets
set(PrivateTargets )
set(PublicTargets Vars::Vars glm imguiDormon::imguiDormon)
set(InterfaceTargets )

#set these libraries to variables that are provided by libraries that does not support configs
set(PrivateIncludeVariables )
set(PublicIncludeVariables )
set(InterfaceIncludeVariables )
set(PrivateReleaseLibraryVariables )
set(PublicReleaseLibraryVariables )
set(InterfaceReleaseLibraryVariables )
set(PrivateDebugLibraryVariables )
set(PublicDebugLibraryVariables )
set(InterfaceDebugLibraryVariables )

SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

include(CMakeUtils.cmake)

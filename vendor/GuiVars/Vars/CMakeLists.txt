cmake_minimum_required(VERSION 3.13.0)

project(
  Vars
  VERSION 1.0.1
  )

#set these variables to *.cpp, *.c, ..., *.h, *.hpp, ...
set(SOURCES
  src/${PROJECT_NAME}/Vars.cpp
  src/${PROJECT_NAME}/VarsImpl.cpp
  src/${PROJECT_NAME}/Resource.cpp
  src/${PROJECT_NAME}/Directory.cpp
  src/${PROJECT_NAME}/Caller.cpp
  )
set(PRIVATE_INCLUDES
  src/${PROJECT_NAME}/VarsImpl.h
  src/${PROJECT_NAME}/Directory.h
  )
set(PUBLIC_INCLUDES
  src/${PROJECT_NAME}/Fwd.h
  src/${PROJECT_NAME}/Vars.h
  src/${PROJECT_NAME}/Resource.h
  src/${PROJECT_NAME}/Caller.h
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
set(ExternPublicLibraries )
set(ExternInterfaceLibraries )

#set these variables to targets
set(PrivateTargets )
set(PublicTargets )
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

option(${PROJECT_NAME}_BUILD_TESTS "toggle building of unit tests")
if(${PROJECT_NAME}_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
  add_test(NAME baseTest COMMAND tests)
endif()

include(CMakeUtils.cmake)

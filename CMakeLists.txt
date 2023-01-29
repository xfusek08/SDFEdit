cmake_minimum_required (VERSION 3.18)

Project(SDFEdit)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/RenderBase/RenderBase)
add_subdirectory(vendor/GuiVars)
add_subdirectory(vendor/Json)

file(GLOB_RECURSE
    SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    "src/*.cpp"
    "src/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    RenderBase
    GuiVars
    Json
)

target_include_directories(${PROJECT_NAME}
    PUBLIC src
)

# Load Resource file paths definitions
include(vendor/RenderBase/cmakeUtils/LoadResourceFiles.cmake)
load_resource_definitions(resources RESOURCES_DEBUG_DEFINITIONS RESOURCES_RELEASE_DEFINITIONS)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:DEBUG ${RESOURCES_DEBUG_DEFINITIONS}>
    $<$<CONFIG:Release>:NDEBUG RELEASE NO_ASSERT NO_LOG ${RESOURCES_RELEASE_DEFINITIONS}>
)

# Ensure that on release build we copy resources to build directory because release build is using relative path to resources
# Implementation based on:
#   https://stackoverflow.com/a/46439715
# and on
#   https://stackoverflow.com/a/13429998
set(no_copy $<NOT:$<CONFIG:Release>>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E
        $<${no_copy}:echo>
        $<${no_copy}:"copy omitted for non-release build, command would have been ">
        remove_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    COMMAND "${CMAKE_COMMAND}" -E
        $<${no_copy}:echo>
        $<${no_copy}:"copy omitted for non-release build, command would have been ">
        copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)

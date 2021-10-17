cmake_minimum_required (VERSION 3.18)

Project(SDFEdit)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/$<CONFIG>)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build)

add_subdirectory(vendor/RenderBase/RenderBase)

file(GLOB_RECURSE
    SOURCES
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    "src/*.cpp"
    "src/*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    RenderBase
)

target_include_directories(${PROJECT_NAME}
    PUBLIC src
)

# Load Resource file paths definitions
include(vendor/RenderBase/cmakeUtils/LoadResourceFiles.cmake)
load_resource_definitions(resources RESOURCES_DEBUG_DEFINITIONS RESOURCES_RELEASE_DEFINITIONS)

# message("${RESOURCES_DEBUG_DEFINITIONS}")
# message("${RESOURCES_RELEASE_DEFINITIONS}")

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:DEBUG>:
        "DEBUG"
        ${RESOURCES_DEBUG_DEFINITIONS}
    >
    $<$<CONFIG:RELEASE>:
        "RELEASE" "NO_ASSERT" "NO_LOGS"
        ${RESOURCES_RELEASE_DEFINITIONS}
    >
)

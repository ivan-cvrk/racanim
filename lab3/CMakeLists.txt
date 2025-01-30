cmake_minimum_required(VERSION 3.10)
project(lab3)

# Compiler flags based on platform
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /std:c++17")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -Wextra -Wpedantic -std=c++17")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

# Include directories
include_directories(headers/
                    ../dependencies/assimp/include/
                    ../dependencies/glad/include/
                    ../dependencies/glfw/include/
                    ../dependencies/glm/
                    ../dependencies/stb/)

# Source files
file(GLOB DEPENDENCY_SOURCES ../dependencies/glad/src/glad.c)
file(GLOB PROJECT_HEADERS headers/*.hpp headers/*.h)
file(GLOB PROJECT_SOURCES sources/*.cpp)
file(GLOB PROJECT_SHADERS shaders/*.comp shaders/*.frag shaders/*.geom shaders/*.vert)
file(GLOB_RECURSE PROJECT_RESOURCES resources/*)

# Organize source groups
source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Dependencies" FILES ${DEPENDENCY_SOURCES})
source_group("Resources" FILES ${PROJECT_RESOURCES})
set_source_files_properties(${PROJECT_RESOURCES} PROPERTIES HEADER_FILE_ONLY TRUE)

# Define project-specific flags and source definitions
add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${DEPENDENCY_SOURCES} ${PROJECT_RESOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} assimp glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES})

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

# Define a dummy file that will be used as the output of the copy command
set(DUMMY_SHADERS_OUTPUT_FILE ${CMAKE_BINARY_DIR}/shaders/copied_marker.txt)

# Add custom command to copy the shaders to the target directory
add_custom_command(
    OUTPUT ${DUMMY_SHADERS_OUTPUT_FILE}  # Dummy output file to track the command
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/lab3/shaders
    DEPENDS ${PROJECT_SHADERS}  # List of all shader files
    COMMENT "Copying shaders to target directory"
)

# Create a custom target for copying shaders
add_custom_target(copy_shaders_lab3 ALL DEPENDS ${DUMMY_SHADERS_OUTPUT_FILE})

# Ensure that the shaders are copied when the main project is built
add_dependencies(${PROJECT_NAME} copy_shaders_lab3)

# Define a dummy file that will be used as the output of the copy command
set(DUMMY_RESOURCES_OUTPUT_FILE ${CMAKE_BINARY_DIR}/resources/copied_marker.txt)

# Add custom command to copy the resrources to the target directory
add_custom_command(
    OUTPUT ${DUMMY_RESOURCES_OUTPUT_FILE}  # Dummy output file to track the command
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/lab3/resources
    DEPENDS ${PROJECT_SHADERS}  # List of all shader files
    COMMENT "Copying resources to target directory"
)

# Create a custom target for copying shaders
add_custom_target(copy_resources_lab3 ALL DEPENDS ${DUMMY_RESOURCES_OUTPUT_FILE})

# Ensure that the resources are copied when the main project is built
add_dependencies(${PROJECT_NAME} copy_resources_lab3)

# Specify the minimum required version of CMAKE
cmake_minimum_required(VERSION 3.10)

# Set c standard
set(CMAKE_C_STANDARD 11)

# Specify project name
project(ProjctConsole)

# Include directory list for headers
include_directories(${CMAKE_SOURCE_DIR}/../fileHandler 
                    ${CMAKE_SOURCE_DIR}/../possixOperation 
                    ${CMAKE_SOURCE_DIR}/../possixHandler 
                    ${CMAKE_SOURCE_DIR}/../appTimer 
                    ${CMAKE_SOURCE_DIR}/../appLed
                    ${CMAKE_SOURCE_DIR}/../Console
                    ${CMAKE_SOURCE_DIR}/../Common)

# Search files with specific patterm (or use set function)
file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/../*.c"
                    "${CMAKE_SOURCE_DIR}/../fileHandler/*.c"
                    "${CMAKE_SOURCE_DIR}/../possixOperation/*.c"
                    "${CMAKE_SOURCE_DIR}/../possixHandler/*.c"
                    "${CMAKE_SOURCE_DIR}/../appTimer/*.c"
                    "${CMAKE_SOURCE_DIR}/../appLed/*.c"
                    "${CMAKE_SOURCE_DIR}/../Console/*.c")

# Add executables with project name
add_executable(Project_Release ${SRC_FILES})
target_compile_options(Project_Release PRIVATE -Wall -Werror -Wextra)
set_target_properties(Project_Release PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../Release)

# Add executables for another target
add_executable(Project_Debug ${SRC_FILES})
target_compile_options(Project_Debug PRIVATE -Wall -Werror -Wextra)
set_target_properties(Project_Debug PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../Debug)
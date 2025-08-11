# Specify the minimum required version of CMAKE
cmake_minimum_required(VERSION 3.10)

# Set c standard
set(CMAKE_C_STANDARD 11)

# Specify project name
project(ProjectRpi)

# Setting rpi Operating system
set(CMAKE_SYSTEM_NAME Linux)

# Setting rpi architecture
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# Setting rpi cross compiler
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)

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

# For cross compiled build 
add_executable(ProjectRpi ${SRC_FILES})
target_compile_options(ProjectRpi PRIVATE -Wall -Wextra -Werror)
set_target_properties(ProjectRpi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../Release)
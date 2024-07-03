# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.13)

# Pull in the Pico SDK
include(pico_sdk_import.cmake)

# Set the name of the project
project(pico_fft)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# Create the library target
add_library(${PROJECT_NAME} INTERFACE)

# Specify the source files for the library
target_sources(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/fft.c
)

# Specify the include directories for the library
target_include_directories(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME} INTERFACE
    pico_stdlib
<<<<<<< HEAD
    hardware_adc
    hardware_dma
    kiss_fttr
=======
>>>>>>> 6eb4603840bd719395db3059dfcb735ff3d6ca45
)

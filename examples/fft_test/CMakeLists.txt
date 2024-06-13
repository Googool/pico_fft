# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.13)

# Set the name of the project
project(fft_test)

# Add your source files here
add_executable(${PROJECT_NAME}
    src/main.c
)

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME} PRIVATE pico_fft)

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 1)

# Generate additional output files
pico_add_extra_outputs(${PROJECT_NAME})

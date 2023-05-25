cmake_minimum_required(VERSION 3.26)
set(PROJECT_NAME ocl)

project(${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)

FILE(GLOB_RECURSE source src/*.c)

find_package(OpenCL REQUIRED)

include_directories(
    include
)

add_executable(ocl
    ${source}
)

target_link_libraries(ocl OpenCL::OpenCL)


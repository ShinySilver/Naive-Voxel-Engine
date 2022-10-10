cmake_minimum_required(VERSION 3.19)
project(iVy)

set(CMAKE_CXX_STANDARD 20)

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/../build)

include_directories(includes)

file(COPY ressources DESTINATION ${dir}/../build)

file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp)
add_executable(iVy ${SRC_FILES} src/common/utils/networking.cpp src/common/utils/networking.h)
target_link_libraries(iVy glfw pthread dl freetype)

target_sources(iVy PRIVATE libs/glad/glad.c)
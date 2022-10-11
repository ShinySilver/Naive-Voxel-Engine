cmake_minimum_required(VERSION 3.19)
project(iVy)

set(CMAKE_CXX_STANDARD 20)

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/../build)

include_directories(includes)

file(COPY ressources DESTINATION ${dir}/../build)

add_definitions(-DLOGURU_WITH_STREAMS=1)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(iVy ${SRC_FILES} src/client/client_networking.cpp src/client/client_networking.h src/server/server_networking.cpp src/server/server_networking.h)
target_link_libraries(iVy glfw pthread dl freetype)
#target_compile_options(iVy PRIVATE -fsanitize=address)
#target_link_options(iVy PRIVATE -fsanitize=address)

target_sources(iVy PRIVATE libs/glad/glad.c libs/loguru.cpp)

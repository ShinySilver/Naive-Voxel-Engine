cmake_minimum_required(VERSION 3.19)
project(iVy)

set(CMAKE_CXX_STANDARD 20)
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/../build)

add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)

file(COPY resources DESTINATION ${dir}/../build)
file(GLOB_RECURSE SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.tcc
        libs/glad/glad.c
        libs/loguru.cpp)

include_directories(includes)

add_executable(iVy ${SRC_FILES})
target_compile_definitions(iVy PRIVATE LOGURU_WITH_STREAMS=1)
target_link_libraries(iVy glfw pthread dl freetype)

add_executable(iVy_server ${SRC_FILES})
target_compile_definitions(iVy_server PRIVATE LOGURU_WITH_STREAMS=1 SERVER_ONLY=true SIMPLIFIED_LOGS=true)
target_link_libraries(iVy_server glfw pthread dl freetype)
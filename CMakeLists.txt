cmake_minimum_required(VERSION 3.19)
project(iVy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-mavx2 -mfma")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -static")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-std=c17 -mavx2 -mfma")
set(CMAKE_C_FLAGS_RELEASE "-O3 -static")

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/../build)

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -static")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")

add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
add_link_options(-fsanitize=address,undefined -fno-omit-frame-pointer)

file(COPY resources DESTINATION ${dir}/../build)
file(GLOB_RECURSE SRC_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE LIB_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/loguru/*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/*
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/cplib/*)

include_directories(libs)

# Disable warnings for libraries we do not own
set_source_files_properties(/libs/stb/stb_include.h PROPERTIES COMPILE_FLAGS -w)
set_source_files_properties(/libs/FastNoise/FastNoiseLite.h PROPERTIES COMPILE_FLAGS -w)
set_source_files_properties(/libs/cplib/cpmath.h PROPERTIES COMPILE_FLAGS -w)

add_executable(iVy ${SRC_FILES} ${LIB_FILES} src/client/main.h)
target_compile_definitions(iVy PRIVATE LOGURU_WITH_STREAMS=1)
target_link_libraries(iVy glfw pthread dl freetype)

add_executable(iVy_server ${SRC_FILES} ${LIB_FILES})
target_compile_definitions(iVy_server PRIVATE LOGURU_WITH_STREAMS=1 SERVER_ONLY=true SIMPLIFIED_LOGS=true)
target_link_libraries(iVy_server glfw pthread dl freetype)
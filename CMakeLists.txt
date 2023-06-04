cmake_minimum_required(VERSION 3.19)
project(iVy)

set(CMAKE_CXX_STANDARD 20)

set(dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${dir}/../build)

include_directories(includes)

file(COPY resources DESTINATION ${dir}/../build)

add_definitions(-DLOGURU_WITH_STREAMS=1)
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(iVy ${SRC_FILES} src/client/workers/client_networking.cpp src/client/workers/client_networking.h src/server/workers/server_networking.cpp src/server/workers/server_networking.h src/client/workers/chunk_loading.cpp src/client/workers/chunk_loading.h src/client/utils/chunk_cache.h src/client/utils/meshing/mesh.h src/common/world/chunk.h src/common/utils/positioning.tcc src/client/utils/colors.h src/client/utils/meshing/greedy_mesher.cpp src/common/world/voxels.h src/common/utils/direction.h src/common/utils/stats.h src/client/ui/debug_screen.h src/client/vertex_pool.cpp src/client/vertex_pool.h)
target_link_libraries(iVy glfw pthread dl freetype)
target_compile_options(iVy PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(iVy PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)

target_sources(iVy PRIVATE libs/glad/glad.c libs/loguru.cpp)

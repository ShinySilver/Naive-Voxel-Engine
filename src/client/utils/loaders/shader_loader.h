#ifndef SHADER_HPP
#define SHADER_HPP

#ifndef __gl_h_
#include <glad/glad.h>
#endif

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

#endif

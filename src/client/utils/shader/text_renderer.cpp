/*
 * TextRenderer.cpp
 *
 *  Created on: 21 mai 2020
 *      Author: silverly
 */

#include "text_renderer.h"


#include <freetype/freetype.h>
#include <glm/glm/ext/matrix_float4x4.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <loguru.hpp>

#include "../loaders/shader_loader.h"

#include <iterator>
#include <utility>

GLuint TextRenderer::_programId = 0;

TextRenderer::TextRenderer(std::string path, int fontWidth, int fontHeight, int windowWidth,
                           int windowHeight) {

    // The shader, with the screen size as uniform
    if (!_programId) {
        _programId = LoadShaders("resources/shaders/text/text.vs",
                                 "resources/shaders/text/text.fs");
        setRatio(windowWidth, windowHeight);
    }
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        ABORT_S() << "FREETYPE: Could not init FreeType Library";

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, path.c_str(), 0, &face))
        ABORT_S() << "FREETYPE: Failed to load font";

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            ABORT_S() << "FREETYTPE: Failed to load Glyph";
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
                GL_TEXTURE_2D, 0,
                GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0,
                GL_RED,
                GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        TextRenderer::Character character = {texture, glm::ivec2(
                face->glyph->bitmap.width, face->glyph->bitmap.rows),
                                             glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                                             face->glyph->advance.x};
        _characters.insert(std::pair<char, Character>(c, character));
    }
    // unbind GL_TEXTURE_2D
    glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);

    glGenBuffers(1, &_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::setRatio(int windowWidth, int windowHeight) {
    glUseProgram(_programId);
    glm::mat4 projection = glm::ortho(0.0f, float(windowWidth), 0.0f,
                                      float(windowHeight));
    glUniformMatrix4fv(glGetUniformLocation(_programId, "projection"), 1,
                       GL_FALSE, glm::value_ptr(projection));
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;
}

void TextRenderer::bind() {
    // activate corresponding render state
    glUseProgram(_programId);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_VAO);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void TextRenderer::unbind() {
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}

void TextRenderer::renderText(std::string text, float x, float y, float scale,
                              glm::vec3 color) {
    //x = (float(_windowWidth) -  float(_characters['I'].Bearing.x) * 2.0f * scale) * x
    //		+ float(_characters[0].Bearing.x) * scale / 2;
    x *= float(_windowWidth);

    y = (float(_windowHeight) -  float(_characters['I'].Bearing.y) * 2.0f * scale) * y
    		+ float(_characters[0].Bearing.y) * scale / 2;
    //y *= float(_windowHeight);

    glUniform3f(glGetUniformLocation(_programId, "textColor"), color.x, color.y, color.z);

    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = _characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
                {xpos,     ypos + h, 0.0f, 0.0f},
                {xpos,     ypos,     0.0f, 1.0f},
                {xpos + w, ypos,     1.0f, 1.0f},

                {xpos,     ypos + h, 0.0f, 0.0f},
                {xpos + w, ypos,     1.0f, 1.0f},
                {xpos + w, ypos + h, 1.0f, 0.0f}
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, _VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices),
                        vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) *
             scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
}


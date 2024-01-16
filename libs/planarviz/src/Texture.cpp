#include "Texture.h"
using namespace planarviz;

#include <gl/glew.h>


Texture::Texture(int width, int height) : m_width(width), m_height(height), m_texture(-1) {
    m_data = std::vector<GLubyte>(width * height * 4, (GLubyte)0);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BORDER_COLOR);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_data[0]);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenBuffers(1, &m_pbo);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, m_data.size() * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_texture);
}

void Texture::use() const {
    // glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::update() const {
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, m_data.size() * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);

    GLubyte* dataPtr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (dataPtr) {
        std::copy(m_data.begin(), m_data.end(), dataPtr);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    }

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_pbo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &m_data[0]);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Texture::colorizeBuffer(std::vector<float>& bwBuffer, Color colorA, Color colorB) {
    int w = m_width, wh = m_height * m_width;
    for (int idx = 0; idx < m_height * m_width; idx++) {
        float alpha = bwBuffer[idx]; float beta = 1.f - alpha;
        m_data[idx * 4] = (int)((colorA.r * alpha + colorB.r * beta) * 255.f);
        m_data[idx * 4 + 1] = (int)((colorA.g * alpha + colorB.g * beta) * 255.f);
        m_data[idx * 4 + 2] = (int)((colorA.b * alpha + colorB.b * beta) * 255.f);
        m_data[idx * 4 + 3] = (int)((colorA.a * alpha + colorB.a * beta) * 255.f);
    }
    update();
}
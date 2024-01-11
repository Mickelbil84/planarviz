#include "Shader.h"
using namespace planarviz;

#include <gl/glew.h>

#include <fmt/core.h>
using fmt::print, fmt::format;

Shader::Shader(std::string vsSource, std::string fsSource) {
    GLint success = 0;
    char buffer[2048];

    const GLchar* vsSource_cstr = vsSource.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsSource_cstr, nullptr);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vs, 2048, nullptr, buffer);
        glDeleteShader(vs);
        throw std::runtime_error(format("Could not compile vertex shader:\n\n{}\n", buffer));
    }

    const GLchar* fsSource_cstr = fsSource.c_str();
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsSource_cstr, nullptr);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fs, 2048, nullptr, buffer);
        glDeleteShader(fs);
        throw std::runtime_error(format("Could not compile fragment shader:\n\n{}\n", buffer));
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(m_program, 2048, nullptr, buffer);
        throw std::runtime_error(format("Could not link shader program:\n\n{}\n", buffer));
    }
    glDetachShader(m_program, vs);
    glDetachShader(m_program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
}
Shader::Shader(std::string filename) : Shader(getFileContent(filename + ".vs"), getFileContent(filename + ".fs")) {
}

Shader::~Shader() {
    glDeleteProgram(m_program);
}

void Shader::use() {
    glUseProgram(m_program);
}

GLuint Shader::getUniformLocation(std::string uniformName) {
    if (!m_uniforms.contains(uniformName))
        m_uniforms[uniformName] = glGetUniformLocation(m_program, uniformName.c_str());
    return m_uniforms[uniformName];
}

void Shader::uniform(std::string uniformName, bool b) {
    glUniform1i(getUniformLocation(uniformName), (GLint)b);
}

void Shader::uniform(std::string uniformName, float x) {
    glUniform1f(getUniformLocation(uniformName), x);
}
void Shader::uniform(std::string uniformName, float x, float y) {
    glUniform2f(getUniformLocation(uniformName), x, y);
}
void Shader::uniform(std::string uniformName, float x, float y, float z) {
    glUniform3f(getUniformLocation(uniformName), x, y, z);
}
void Shader::uniform(std::string uniformName, float x, float y, float z, float w) {
    glUniform4f(getUniformLocation(uniformName), x, y, z, w);
}
void Shader::uniform(std::string uniformName, Color color) {
    glUniform4f(getUniformLocation(uniformName), color.r, color.g, color.b, color.a);
}
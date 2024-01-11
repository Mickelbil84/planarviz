#pragma once

#include <map>
#include <string>
#include <memory>

#include "Color.h"
#include "Utils.h"

namespace planarviz {
    class Shader {
    public:
        // Pass shader path without extensions, and there are assumed to be *.vs and *.fs with the given prefix.
        Shader(std::string filename);
        ~Shader();

        void use();

        void uniform(std::string uniformName, bool b);
        void uniform(std::string uniformName, float x);
        void uniform(std::string uniformName, float x, float y);
        void uniform(std::string uniformName, float x, float y, float z);
        void uniform(std::string uniformName, float x, float y, float z, float w);
        void uniform(std::string uniformName, Color color);

    private:
        unsigned int m_program;
        std::map<std::string, unsigned int> m_uniforms;

        unsigned int getUniformLocation(std::string uniformName);
    };
    using ShaderPtr = std::shared_ptr<Shader>;
}
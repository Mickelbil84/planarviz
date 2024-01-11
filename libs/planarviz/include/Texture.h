#pragma once

#include <memory>
#include <vector>

#include "Color.h"

namespace planarviz {

    constexpr float BORDER_COLOR[] = {0.f, 0.f, 0.f, 0.f};

    class Texture {
    public:
        Texture(int widht, int height);
        ~Texture();
        void use() const;
        void update() const;

        // Take BW buffer of values (between 0 and 1), and interpolate between two colors.
        // The higher the value, the more we see of colorA (and less of colorB).
        // NOTE: this function also updates the buffer in GPU.
        void colorizeBuffer(std::vector<float>& bwBuffer, Color colorA, Color colorB);

        inline std::vector<unsigned char>& getData() { return m_data; }
        inline int getWidth() const { return m_width; }
        inline int getHeight() const { return m_height; }
        
    private:
        std::vector<unsigned char> m_data;
        int m_width, m_height;
        unsigned int m_texture, m_pbo;
    };
    using TexturePtr = std::shared_ptr<Texture>;
}
#pragma once

namespace planarviz {
    struct Color {
        float r, g, b, a;
        Color(float r = 0.f, float g = 0.f, float b = 0.f, float a = 1.f) : r(r), g(g), b(b), a(a) {}
    };
}
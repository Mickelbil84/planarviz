#pragma once

namespace planarviz {

    struct Camera {
    public:
        Camera(float aspectRatio, float zoom = 1.f) : x(0.f), y(0.f), zoom(zoom), aspectRatio(aspectRatio) {}
        ~Camera() {}

        inline float getX() const { return x; }
        inline float getY() const { return y; }
        inline void addDelta(float dx, float dy) { x += dx; y += dy; }
        inline void setPosition(float x, float y) { this->x = x; this->y = y; }
        
        inline float getZoom() const { return zoom; }
        inline void addZoom(float dzoom) { zoom += dzoom; if (this->zoom < 0.01f) this->zoom = 0.01f; }
        inline void setZoom(float zoom) { this->zoom = zoom; }

        inline float getAspectRatio() const { return aspectRatio; }
        inline void setAspectRatio(float aspectRatio) { this->aspectRatio = aspectRatio; }

    public:
        float panSpeed = 2.f; float zoomSpeed = 0.01f;

    private:
        float x, y;
        float zoom, aspectRatio;
    };

};
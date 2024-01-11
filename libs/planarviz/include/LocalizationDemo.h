#pragma once

#include "WindowApplication.h"

namespace planarviz {
    
    class LocalizationDemo : public WindowLogic {
    public:
        virtual void init();
        virtual void handleInput(WindowInput input);
        virtual void update(float deltaTime);
        virtual void render();
        virtual void shutdown();

    protected:
        float dragX, dragY;
        float m_elapsedTime = 0.f;

        std::vector<float> probs;
        float sigma = 0.1f;
    };
}
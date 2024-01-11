#pragma once

#include <string>
#include <memory>
#include <stdexcept>

#include "Scene.h"
#include "WindowConstants.h"


namespace planarviz {

    struct WindowInput {
    public:
        bool bIsDrag = false, bIsDragBegin = false;
        int dragX0 = 0, dragY0 = 0;
        float dragDX = 0.f, dragDY = 0.f;

        int mouseX = 0, mouseY = 0, mouseScroll = 0;
        float mouseScreenX = 0.f, mouseScreenY = 0.f;
    };

    class WindowLogic {
    public:
        virtual ~WindowLogic() = default;

        virtual void init() = 0;
        virtual void handleInput(WindowInput input) = 0;
        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;
        virtual void shutdown() = 0;

        inline void updateAspectRatio(float aspectRatio) { m_scene.updateAspectRatio(aspectRatio); } 
        inline void updateWindowSize(int width, int height) { m_windowWidth = width; m_windowHeight = height; }

        // Set to true if logic should ignore cursor hover and drag
        bool m_bCursorOverride = false;
    protected:
        Scene m_scene;
        int m_windowWidth, m_windowHeight;
    };
    using WindowLogicPtr = std::unique_ptr<WindowLogic>;


    class WindowApplication {
    public:
        WindowApplication(WindowLogicPtr pWindowLogic);
        ~WindowApplication();

        void run();

    private:
        WindowLogicPtr m_pWindowLogic;
        
        struct _Internal;
        std::unique_ptr<_Internal> m_internal;

        unsigned int m_prevTime, m_currTime;

        int m_windowWidth, m_windowHeight;
        bool m_bShouldRun;
        WindowInput m_input;

        // Helper methods
        void initSDL();
        void initOpenGL();
    };

}
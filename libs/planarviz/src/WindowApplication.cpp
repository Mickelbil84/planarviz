#include "WindowApplication.h"
using namespace planarviz;

#include <fmt/core.h>

#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

using fmt::print, fmt::format;
using SDL_WindowPtr = std::shared_ptr<SDL_Window>;


struct WindowApplication::_Internal {
    _Internal() {}
    SDL_WindowPtr m_pWindow;
    SDL_GLContext m_glContext;
};

WindowApplication::WindowApplication(WindowLogicPtr pWindowLogic) : 
    m_pWindowLogic(std::move(pWindowLogic)), m_bShouldRun(false), 
    m_internal(std::make_unique<WindowApplication::_Internal>()) {
    initSDL();
    initOpenGL();
    m_bShouldRun = true;
}

WindowApplication::~WindowApplication() {
    if (m_pWindowLogic) m_pWindowLogic->shutdown();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(m_internal->m_glContext);
    SDL_DestroyWindow(m_internal->m_pWindow.get());
    SDL_Quit();
}

void WindowApplication::initSDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        throw std::runtime_error(format("Could not initialize SDL: {}", SDL_GetError()));

    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, planarviz::OPENGL_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, planarviz::OPENGL_MINOR_VERSION);
    m_internal->m_pWindow = std::shared_ptr<SDL_Window>(SDL_CreateWindow(
        "planarviz", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    ), SDL_DestroyWindow);
    if (m_internal->m_pWindow == nullptr)
        throw std::runtime_error(format("Could not create SDL window: {}", SDL_GetError()));
    m_internal->m_glContext = SDL_GL_CreateContext(m_internal->m_pWindow.get());
    m_windowWidth = WINDOW_WIDTH; m_windowHeight = WINDOW_HEIGHT;

    m_prevTime = m_currTime = SDL_GetTicks();
}
void WindowApplication::initOpenGL() {
    if (glewInit() != GLEW_OK)
        throw std::runtime_error(format("Could not init GLEW: {}", (char *)glewGetErrorString(glewInit())));
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(10.f);

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(m_internal->m_pWindow.get(), &m_internal->m_glContext);
    ImGui_ImplOpenGL3_Init();
}

void WindowApplication::run() {
    if (m_pWindowLogic != nullptr) {
        m_pWindowLogic->updateWindowSize(m_windowWidth, m_windowHeight);
        m_pWindowLogic->init();
    }

    while (m_bShouldRun) {
        SDL_Event e;
        m_input.mouseScroll = 0;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) m_bShouldRun = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) m_bShouldRun = false;
            if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                m_windowWidth = e.window.data1;
                m_windowHeight = e.window.data2; 
                if (m_pWindowLogic) {
                    m_pWindowLogic->updateAspectRatio((float)m_windowWidth / (float)m_windowHeight);
                    m_pWindowLogic->updateWindowSize(m_windowWidth, m_windowHeight);
                }
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !m_input.bIsDrag) {
                m_input.bIsDrag = true;
                m_input.bIsDragBegin = true;
                SDL_GetMouseState(&m_input.dragX0, &m_input.dragY0);
            }
            if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT && m_input.bIsDrag) {
                m_input.bIsDrag = false;
                m_input.bIsDragBegin = false;
                m_input.dragX0 = m_input.dragY0 = 0;
            }

            if (e.type == SDL_MOUSEWHEEL) {
                m_input.mouseScroll = e.wheel.y;
            }

            ImGui_ImplSDL2_ProcessEvent(&e);
        }
        SDL_GetMouseState(&m_input.mouseX, &m_input.mouseY);
        m_input.mouseScreenX = 2.f * ((float)m_input.mouseX / (float)m_windowWidth - 0.5f);
        m_input.mouseScreenY = -2.f * ((float)m_input.mouseY / (float)m_windowHeight - 0.5f);

        m_currTime = SDL_GetTicks();
        float deltaTime = (m_currTime - m_prevTime) / 1000.f;
        m_prevTime = m_currTime;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        // ImGui::ShowDemoWindow();

        if (m_pWindowLogic)
            m_pWindowLogic->m_bCursorOverride = ImGui::GetIO().WantCaptureMouse;

        glViewport(0, 0, m_windowWidth, m_windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(1.f, 1.f, 1.f, 1.f);

        if (m_pWindowLogic != nullptr) {
            m_pWindowLogic->handleInput(m_input);
            m_pWindowLogic->update(deltaTime);
            m_pWindowLogic->render();
        }

        if (m_input.bIsDrag) {
            m_input.dragDX = (float)(m_input.mouseX - m_input.dragX0) / (float)m_windowWidth;
            m_input.dragDY = -(float)(m_input.mouseY - m_input.dragY0) / (float)m_windowHeight;
            m_input.bIsDragBegin = false;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(m_internal->m_pWindow.get());
    }
}
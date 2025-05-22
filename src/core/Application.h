#pragma once

#include "Window.h"
#include "../ui/WindowDecorator.h"
#include "../views/ViewManager.h"  // CORRIGIDO: era ../ui/ViewManager.h
#include "../ui/StyleManager.h"
#include <memory>

namespace scummredux {

    class Application {
    public:
        Application();
        ~Application();

        // Main application lifecycle
        bool initialize();
        int run();
        void shutdown();

        // Application control
        void requestClose() { m_shouldClose = true; }
        bool shouldClose() const { return m_shouldClose; }

        // Component access
        Window* getWindow() const { return m_window.get(); }
        WindowDecorator* getWindowDecorator() const { return m_windowDecorator.get(); }
        ViewManager* getViewManager() const { return &ViewManager::getInstance(); }

    private:
        void initializeImGui();
        void shutdownImGui();
        void setupViews();
        void setupEventHandlers();

        // Main loop functions
        void handleEvents();
        void update();
        void render();

        // Rendering phases
        void beginFrame();
        void endFrame();
        void drawMainContent();

        // Core components
        std::unique_ptr<Window> m_window;
        std::unique_ptr<WindowDecorator> m_windowDecorator;

        // Application state
        bool m_initialized = false;
        bool m_shouldClose = false;

        // Timing
        double m_lastFrameTime = 0.0;
        double m_deltaTime = 0.0;
        int m_frameCount = 0;
        double m_fpsUpdateTime = 0.0;
        float m_currentFPS = 0.0f;

        // Settings (removed unused m_showDemo and m_showFPS)
        // These can be re-added when needed
    };

} // namespace scummredux
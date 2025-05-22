#include "Application.h"
#include "Settings.h"
#include "../ui/StyleManager.h"
#include "../utils/Events.hpp"
#include "../views/ExplorerView.h"
#include "../views/EditorView.h"
#include "../views/PropertiesView.h"
#include "../views/ConsoleView.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

namespace scummredux {

    Application::Application() {
        // Load settings first
        Settings::getInstance().load();
    }

    Application::~Application() {
        shutdown();
    }

    bool Application::initialize() {
        if (m_initialized) {
            return true;
        }

        try {
            ConsoleView::info("Initializing SCUMM Redux...");

            // Initialize window
            m_window = std::make_unique<Window>();
            if (!m_window->initialize()) {
                ConsoleView::error("Failed to initialize window");
                return false;
            }
            ConsoleView::success("Window initialized");

            // Initialize ImGui
            initializeImGui();
            ConsoleView::success("ImGui initialized");

            // Initialize style manager and apply theme
            StyleManager::getInstance().setupFonts();
            StyleManager::getInstance().applyCurrentTheme();
            ConsoleView::success("Style system initialized");

            // Create window decorator
            m_windowDecorator = std::make_unique<WindowDecorator>(m_window.get());
            ConsoleView::success("Window decorator created");

            // Setup views
            setupViews();
            ConsoleView::success("Views initialized");

            // Setup event handlers
            setupEventHandlers();
            ConsoleView::success("Event handlers registered");

            // Configure window decorator callback
            m_windowDecorator->setDrawContentCallback([this]() {
                drawMainContent();
            });

            m_initialized = true;
            ConsoleView::success("Application initialization complete!");

            return true;

        } catch (const std::exception& e) {
            ConsoleView::error("Failed to initialize application: " + std::string(e.what()));
            return false;
        }
    }

    void Application::initializeImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = nullptr; // Disable imgui.ini file

        // Initialize ImGui backends
        if (!ImGui_ImplGlfw_InitForOpenGL(m_window->getHandle(), true)) {
            throw std::runtime_error("Failed to initialize ImGui GLFW backend");
        }

        const char* glsl_version = "#version 130";
        if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
            throw std::runtime_error("Failed to initialize ImGui OpenGL backend");
        }
    }

    void Application::shutdownImGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void Application::setupViews() {
        auto& viewManager = ViewManager::getInstance();
        
        // Create all views
        viewManager.addView<ExplorerView>();
        viewManager.addView<EditorView>();
        viewManager.addView<PropertiesView>();
        viewManager.addView<ConsoleView>();

        ConsoleView::info("Created " + std::to_string(viewManager.getViews().size()) + " views");
    }

    void Application::setupEventHandlers() {
        // Handle window close events
        EventWindowClose::subscribe([this](const WindowCloseEvent& event) {
            if (event.shouldClose) {
                ConsoleView::info("Close requested by user");
                requestClose();
            }
        });

        // Handle window resize events
        EventWindowResize::subscribe([](const WindowResizeEvent& event) {
            ConsoleView::debug("Window resized to " + std::to_string(event.width) + "x" + std::to_string(event.height));
        });

        // Handle theme change events
        EventThemeChanged::subscribe([](const ThemeChangedEvent& event) {
            ConsoleView::info("Theme changed to: " + event.themeName);
        });

        // Handle view events
        EventViewOpened::subscribe([](const ViewOpenedEvent& event) {
            ConsoleView::debug("View opened: " + event.viewName);
        });

        EventViewClosed::subscribe([](const ViewClosedEvent& event) {
            ConsoleView::debug("View closed: " + event.viewName);
        });
    }

    int Application::run() {
        if (!initialize()) {
            ConsoleView::error("Failed to initialize application");
            return -1;
        }

        ConsoleView::info("Starting main loop...");
        
        m_lastFrameTime = glfwGetTime();
        
        // Main loop
        while (!m_window->shouldClose() && !m_shouldClose) {
            handleEvents();
            update();
            render();
        }

        ConsoleView::info("Application shutting down...");
        return 0;
    }

    void Application::handleEvents() {
        m_window->pollEvents();
    }

    void Application::update() {
        // Calculate delta time
        double currentTime = glfwGetTime();
        m_deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        
        // Update FPS counter
        m_frameCount++;
        if (currentTime - m_fpsUpdateTime >= 1.0) {
            m_currentFPS = m_frameCount / (currentTime - m_fpsUpdateTime);
            m_frameCount = 0;
            m_fpsUpdateTime = currentTime;
        }

        // Post frame begin event
        EventFrameBegin::post({});
    }

    void Application::render() {
        beginFrame();
        
        // Main rendering
        m_windowDecorator->render();
        
        // Draw ImGui demo if enabled
        if (m_showDemo) {
            ImGui::ShowDemoWindow(&m_showDemo);
        }
        
        // FPS overlay
        if (m_showFPS) {
            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.8f);
            if (ImGui::Begin("FPS", nullptr, 
                ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | 
                ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                ImGuiWindowFlags_NoNav)) {
                ImGui::Text("FPS: %.1f", m_currentFPS);
                ImGui::Text("Frame Time: %.3f ms", m_deltaTime * 1000.0);
            }
            ImGui::End();
        }
        
        endFrame();
        
        // Post frame end event
        EventFrameEnd::post({});
    }

    void Application::beginFrame() {
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Application::endFrame() {
        // Render ImGui
        ImGui::Render();
        
        // Setup viewport
        int display_w, display_h;
        glfwGetFramebufferSize(m_window->getHandle(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        
        // Clear and render
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Swap buffers
        m_window->swapBuffers();
    }

    void Application::drawMainContent() {
        auto& viewManager = ViewManager::getInstance();
        
        // Draw always visible content
        viewManager.drawAlwaysVisibleContent();
        
        // Draw all views
        viewManager.drawViews();
    }

    void Application::shutdown() {
        if (!m_initialized) {
            return;
        }

        ConsoleView::info("Shutting down application...");

        // Save settings
        Settings::getInstance().save();
        ConsoleView::info("Settings saved");

        // Cleanup components
        m_windowDecorator.reset();
        
        // Shutdown ImGui
        shutdownImGui();
        ConsoleView::info("ImGui shutdown complete");

        // Shutdown window
        m_window.reset();
        ConsoleView::info("Window shutdown complete");

        m_initialized = false;
        ConsoleView::success("Application shutdown complete");
    }

} // namespace scummredux
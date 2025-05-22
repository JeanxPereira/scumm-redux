#include "Window.h"
#include "Settings.h"
#include "../utils/Events.hpp"
#include "../utils/Utils.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#include <windowsx.h>
#undef GLFW_EXPOSE_NATIVE_WIN32
#endif

namespace scummredux {

#ifdef _WIN32
    LONG_PTR Window::s_oldWndProc = 0;
    Window* Window::s_instance = nullptr;
#endif

    Window::Window()
        : m_window(nullptr)
        , m_title("SCUMM Redux")
        , m_restorePos(0, 0)
        , m_restoreSize(1280, 720)
        , m_initialized(false) {
#ifdef _WIN32
        s_instance = this;
#endif
    }

    Window::~Window() {
        shutdown();
    }

    bool Window::initialize() {
        if (m_initialized) return true;

        // Configure GLFW
        configureGLFW();

        auto& settings = Settings::getInstance();

        // Get window properties from settings
        int width = settings.get<int>(Settings::App::WINDOW_WIDTH, 1280);
        int height = settings.get<int>(Settings::App::WINDOW_HEIGHT, 720);
        bool maximized = settings.get<bool>(Settings::App::WINDOW_MAXIMIZED, false);
        int posX = settings.get<int>(Settings::App::WINDOW_POS_X, -1);
        int posY = settings.get<int>(Settings::App::WINDOW_POS_Y, -1);

        // Create window
        m_window = glfwCreateWindow(width, height, m_title.c_str(), nullptr, nullptr);
        if (!m_window) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwSetWindowUserPointer(m_window, this);
        setupCallbacks();

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(1); // VSync

        // Setup platform-specific features
        setupPlatformSpecific();

        // Set window position
        if (posX >= 0 && posY >= 0) {
            glfwSetWindowPos(m_window, posX, posY);
        } else {
            center();
        }

        // Set maximized state
        if (maximized) {
            glfwMaximizeWindow(m_window);
        }

        // Show window
        glfwShowWindow(m_window);

        m_initialized = true;
        return true;
    }

    void Window::shutdown() {
        if (!m_initialized) return;

        // Save window state to settings
        auto& settings = Settings::getInstance();

        if (!isMaximized()) {
            auto size = getSize();
            auto pos = getPosition();
            settings.set(Settings::App::WINDOW_WIDTH, (int)size.x);
            settings.set(Settings::App::WINDOW_HEIGHT, (int)size.y);
            settings.set(Settings::App::WINDOW_POS_X, (int)pos.x);
            settings.set(Settings::App::WINDOW_POS_Y, (int)pos.y);
        }
        settings.set(Settings::App::WINDOW_MAXIMIZED, isMaximized());
        settings.save();

        if (m_window) {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }

        glfwTerminate();
        m_initialized = false;
    }

    void Window::configureGLFW() {
        glfwSetErrorCallback(errorCallback);

        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // OpenGL hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        // Window hints for custom title bar
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);        // No OS title bar
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);          // Hidden initially
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);         // Allow resize
        glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);           // Focus on creation
        glfwWindowHint(GLFW_SAMPLES, 4);                   // Multi-sampling
    }

#ifdef _WIN32
    void Window::setupPlatformSpecific() {
        setupWindows();
    }

    void Window::setupWindows() {
        HWND hwnd = getWin32Handle();
        if (!hwnd) return;

        // Enable DWM composition
        DwmEnableMMCSS(TRUE);

        // Set DWM attributes for better rendering
        BOOL value = TRUE;
        DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_ENABLED, &value, sizeof(value));

        DWMNCRENDERINGPOLICY policy = DWMNCRP_ENABLED;
        DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(policy));

        // Extend frame into client area for custom title bar
        MARGINS margins = { 0, 0, 30, 0 }; // 30px top margin for title bar
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        // Setup custom window procedure for drag handling
        s_oldWndProc = SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)windowProc);
    }

    HWND Window::getWin32Handle() const {
        return glfwGetWin32Window(m_window);
    }

    LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (!s_instance) {
            return CallWindowProc((WNDPROC)s_oldWndProc, hwnd, uMsg, wParam, lParam);
        }

        switch (uMsg) {
            case WM_NCHITTEST: {
                POINT cursor = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                ScreenToClient(hwnd, &cursor);

                RECT window;
                GetClientRect(hwnd, &window);

                // Title bar area (top 30px)
                if (cursor.y >= 0 && cursor.y < 30) {
                    // Check if we're over window control buttons (last 150px)
                    if (cursor.x > window.right - 150) {
                        return HTCLIENT; // Let ImGui handle buttons
                    }
                    return HTCAPTION; // Title bar drag area
                }

                // Resize borders
                const int borderWidth = 5;
                bool onLeft = cursor.x < borderWidth;
                bool onRight = cursor.x > window.right - borderWidth;
                bool onTop = cursor.y < borderWidth;
                bool onBottom = cursor.y > window.bottom - borderWidth;

                if (onTop && onLeft) return HTTOPLEFT;
                if (onTop && onRight) return HTTOPRIGHT;
                if (onBottom && onLeft) return HTBOTTOMLEFT;
                if (onBottom && onRight) return HTBOTTOMRIGHT;
                if (onLeft) return HTLEFT;
                if (onRight) return HTRIGHT;
                if (onTop) return HTTOP;
                if (onBottom) return HTBOTTOM;

                return HTCLIENT;
            }

            case WM_NCCALCSIZE: {
                // Remove default window chrome
                if (wParam == TRUE) {
                    return 0;
                }
                break;
            }

            case WM_NCPAINT: {
                // Prevent default non-client painting
                return 0;
            }
        }

        return CallWindowProc((WNDPROC)s_oldWndProc, hwnd, uMsg, wParam, lParam);
    }

#else
    void Window::setupPlatformSpecific() {
        // Linux/macOS specific setup can go here
    }
#endif

    void Window::setupCallbacks() {
        glfwSetWindowSizeCallback(m_window, windowSizeCallback);
        glfwSetWindowPosCallback(m_window, windowPosCallback);
        glfwSetWindowMaximizeCallback(m_window, windowMaximizeCallback);
        glfwSetWindowFocusCallback(m_window, windowFocusCallback);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetCursorPosCallback(m_window, cursorPosCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
        glfwSetCharCallback(m_window, charCallback);
    }

    // Window operations
    void Window::pollEvents() {
        glfwPollEvents();
    }

    bool Window::shouldClose() const {
        return glfwWindowShouldClose(m_window);
    }

    void Window::swapBuffers() {
        glfwSwapBuffers(m_window);
    }

    void Window::setTitle(const std::string& title) {
        m_title = title;
        if (m_window) {
            glfwSetWindowTitle(m_window, title.c_str());
        }
    }

    void Window::setSize(int width, int height) {
        glfwSetWindowSize(m_window, width, height);
    }

    void Window::setPosition(int x, int y) {
        glfwSetWindowPos(m_window, x, y);
    }

    void Window::setPositionSmooth(int x, int y) {
        if (!m_window) return;

        // Get current position
        int currentX, currentY;
        glfwGetWindowPos(m_window, &currentX, &currentY);

        // Calculate distance
        int deltaX = x - currentX;
        int deltaY = y - currentY;

        // Only update if delta is significant enough (reduces jitter)
        if (std::abs(deltaX) > 1 || std::abs(deltaY) > 1) {
            glfwSetWindowPos(m_window, x, y);
        }
    }

    void Window::maximize() {
        if (!isMaximized()) {
            // Store current size and position for restore
            m_restorePos = getPosition();
            m_restoreSize = getSize();
        }
        glfwMaximizeWindow(m_window);
    }

    void Window::minimize() {
        glfwIconifyWindow(m_window);
    }

    void Window::restore() {
        if (isMaximized()) {
            glfwRestoreWindow(m_window);
            // Restore previous size and position
            setPosition((int)m_restorePos.x, (int)m_restorePos.y);
            setSize((int)m_restoreSize.x, (int)m_restoreSize.y);
        }
    }

    void Window::center() {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        if (!monitor) return;

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (!mode) return;

        auto size = getSize();
        int x = (mode->width - (int)size.x) / 2;
        int y = (mode->height - (int)size.y) / 2;

        glfwSetWindowPos(m_window, x, y);
    }

    // Window state queries
    bool Window::isMaximized() const {
        return glfwGetWindowAttrib(m_window, GLFW_MAXIMIZED) == GLFW_TRUE;
    }

    bool Window::isMinimized() const {
        return glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) == GLFW_TRUE;
    }

    bool Window::isFocused() const {
        return glfwGetWindowAttrib(m_window, GLFW_FOCUSED) == GLFW_TRUE;
    }

    ImVec2 Window::getSize() const {
        int width, height;
        glfwGetWindowSize(m_window, &width, &height);
        return ImVec2((float)width, (float)height);
    }

    ImVec2 Window::getPosition() const {
        int x, y;
        glfwGetWindowPos(m_window, &x, &y);
        return ImVec2((float)x, (float)y);
    }

    ImVec2 Window::getFramebufferSize() const {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);
        return ImVec2((float)width, (float)height);
    }

    float Window::getDPIScale() const {
        float xscale, yscale;
        glfwGetWindowContentScale(m_window, &xscale, &yscale);
        return std::max(xscale, yscale);
    }

    ImVec2 Window::getContentScale() const {
        float xscale, yscale;
        glfwGetWindowContentScale(m_window, &xscale, &yscale);
        return ImVec2(xscale, yscale);
    }

    void Window::fullFrame() {
        if (!m_frameRateLocked) {
            return;
        }

        // Frame timing
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(currentTime.time_since_epoch()).count();

        double deltaTime = elapsed - m_lastFrameTime;
        if (deltaTime < m_targetFrameTime) {
            // Sleep for remaining time
            std::this_thread::sleep_for(
                std::chrono::duration<double>(m_targetFrameTime - deltaTime)
            );
        }

        m_lastFrameTime = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }

    void Window::beginFrame() {
        glfwPollEvents();
    }

    void Window::endFrame() {
        glfwSwapBuffers(m_window);
        fullFrame(); // Frame rate limiting

#ifdef _WIN32
        // Flush DWM for smooth rendering
        DwmFlush();
#endif
    }

    // Static GLFW callbacks
    void Window::windowSizeCallback(GLFWwindow* window, int width, int height) {
        EventWindowResize::post({width, height});
    }

    void Window::windowPosCallback(GLFWwindow* window, int x, int y) {
        // Window position changed
    }

    void Window::windowMaximizeCallback(GLFWwindow* window, int maximized) {
        EventWindowMaximize::post({maximized == GLFW_TRUE});
    }

    void Window::windowFocusCallback(GLFWwindow* window, int focused) {
        // Window focus changed
    }

    void Window::windowCloseCallback(GLFWwindow* window) {
        EventWindowClose::post({true});
    }

    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }

    void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        // Key events handled by ImGui
    }

    void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        // Mouse events handled by ImGui
    }

    void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
        // Cursor events handled by ImGui
    }

    void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        // Scroll events handled by ImGui
    }

    void Window::charCallback(GLFWwindow* window, unsigned int codepoint) {
        // Character events handled by ImGui
    }

    void Window::errorCallback(int error, const char* description) {
        std::cerr << "GLFW Error " << error << ": " << description << std::endl;
    }

} // namespace scummredux
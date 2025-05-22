#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <string>
#include <memory>
#include <functional>

// Forward declarations for Windows types
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace scummredux {

    class Window {
    public:
        Window();
        ~Window();

        bool initialize();
        void shutdown();

        // Main loop
        void pollEvents();
        bool shouldClose() const;
        void swapBuffers();
        void beginFrame();
        void endFrame();

        // Window properties
        void setTitle(const std::string& title);
        void setSize(int width, int height);
        void setPosition(int x, int y);
        void setPositionSmooth(int x, int y);

        // Window operations
        void maximize();
        void minimize();
        void restore();
        void center();

        // Window state
        bool isMaximized() const;
        bool isMinimized() const;
        bool isFocused() const;

        ImVec2 getSize() const;
        ImVec2 getPosition() const;
        ImVec2 getFramebufferSize() const;

        // GLFW handle
        GLFWwindow* getHandle() const { return m_window; }

        // DPI scaling
        float getDPIScale() const;
        ImVec2 getContentScale() const;

        // Frame rate control
        void lockFrameRate() { m_frameRateLocked = true; }
        void unlockFrameRate() { m_frameRateLocked = false; }
        void fullFrame();

    private:
        void setupCallbacks();
        void configureGLFW();
        void setupPlatformSpecific();

        // Platform specific setup
#ifdef _WIN32
        void setupWindows();
        static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        HWND getWin32Handle() const;

        // Windows-specific members
        static LONG_PTR s_oldWndProc;
        static Window* s_instance;
        bool m_layeredWindow = false;
#endif

        // Static callbacks for GLFW
        static void windowSizeCallback(GLFWwindow* window, int width, int height);
        static void windowPosCallback(GLFWwindow* window, int x, int y);
        static void windowMaximizeCallback(GLFWwindow* window, int maximized);
        static void windowFocusCallback(GLFWwindow* window, int focused);
        static void windowCloseCallback(GLFWwindow* window);
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
        static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        static void charCallback(GLFWwindow* window, unsigned int codepoint);
        static void errorCallback(int error, const char* description);

        GLFWwindow* m_window;
        std::string m_title;

        // Window state before maximizing (for restore)
        ImVec2 m_restorePos;
        ImVec2 m_restoreSize;

        bool m_initialized;
        bool m_frameRateLocked = false;

        // Frame timing
        double m_lastFrameTime = 0.0;
        double m_targetFrameTime = 1.0 / 60.0; // 60 FPS default
    };

} // namespace scummredux
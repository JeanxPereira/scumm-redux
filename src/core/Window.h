#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <string>
#include <memory>

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
        
        // Window properties
        void setTitle(const std::string& title);
        void setSize(int width, int height);
        void setPosition(int x, int y);
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

    private:
        void setupCallbacks();
        void configureGLFW();
        
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
    };

} // namespace scummredux
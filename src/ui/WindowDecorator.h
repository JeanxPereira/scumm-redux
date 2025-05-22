#pragma once

#include "TitleBar.h"
#include "../views/ViewManager.h"  // Corrigido: deve apontar para views/ não ui/
#include <imgui.h>
#include <memory>
#include <functional>

namespace scummredux {

    class Window;
    class ViewManager;

    class WindowDecorator {
    public:
        explicit WindowDecorator(Window* window);
        ~WindowDecorator() = default;

        // Main rendering function
        void render();

        // Component access
        TitleBar* getTitleBar() const { return m_titleBar.get(); }

        // Configuration
        void setShowTitleBar(bool show) { m_showTitleBar = show; }
        void setMainDockSpaceId(ImGuiID id) { m_mainDockSpaceId = id; }

        // Callbacks
        void setDrawContentCallback(std::function<void()> callback) { m_drawContentCallback = callback; }

        // Window state
        bool isFirstFrame() const { return m_isFirstFrame; }

    private:
        void renderMainFrame();
        void renderDockSpace();
        void setupInitialLayout();
        void drawTitleBarBackdrop();

        // Window* m_window; // Currently unused
        std::unique_ptr<TitleBar> m_titleBar;
        
        // Configuration
        bool m_showTitleBar = true;
        bool m_isFirstFrame = true;
        
        // DockSpace
        ImGuiID m_mainDockSpaceId = 0;
        
        // Content callback
        std::function<void()> m_drawContentCallback;
        
        // Style
        struct WindowStyle {
            ImVec4 backgroundColor = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
            ImVec4 borderColor = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
            float borderSize = 0.0f;
            float windowRounding = 0.0f;
            ImVec2 windowPadding = ImVec2(0, 0);
        } m_style;
    };

} // namespace scummredux
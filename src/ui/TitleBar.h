#pragma once

#include <imgui.h>
#include <string>
#include <memory>
#include <functional>

namespace scummredux {

    class Window;

    class TitleBar {
    public:
        struct ButtonInfo {
            std::string icon;
            std::string tooltip;
            std::function<void()> callback;
            bool enabled = true;
        };

        explicit TitleBar(Window* window);
        ~TitleBar() = default;

        // Main render function
        void render();

        // Button management
        void addButton(const std::string& id, const ButtonInfo& button);
        void removeButton(const std::string& id);
        void setButtonEnabled(const std::string& id, bool enabled);

        // Title management
        void setTitle(const std::string& title);
        void setSubtitle(const std::string& subtitle);
        void setProjectInfo(const std::string& projectName, bool hasUnsavedChanges = false);

        // Configuration
        void setHeight(float height) { m_height = height; }
        void setDraggable(bool draggable) { m_draggable = draggable; }
        void setShowIcon(bool show) { m_showIcon = show; }
        void setShowButtons(bool show) { m_showButtons = show; }

        float getHeight() const { return m_height; }

    private:
        void renderIcon();
        void renderTitle();
        void renderCustomButtons();
        void renderWindowControls();
        void handleDragging();
        void handleDoubleClick();

        // Window control actions
        void minimizeWindow();
        void maximizeWindow();
        void closeWindow();

        // Helper functions
        bool isMouseInDragArea() const;
        ImVec2 calculateTitlePosition() const;
        ImVec2 calculateButtonPosition(int buttonIndex, bool fromRight = true) const;

        Window* m_window;
        
        // Title bar properties
        float m_height = 30.0f;
        bool m_draggable = true;
        bool m_showIcon = true;
        bool m_showButtons = true;
        
        // Content
        std::string m_title = "SCUMM Redux";
        std::string m_subtitle;
        std::string m_projectName;
        bool m_hasUnsavedChanges = false;
        
        // Custom buttons (ordered map to maintain insertion order)
        std::vector<std::pair<std::string, ButtonInfo>> m_customButtons;
        
        // Button sizes and styling
        static constexpr float BUTTON_SIZE = 16.0f;
        static constexpr float BUTTON_PADDING = 8.0f;
        static constexpr float ICON_SIZE = 16.0f;
        static constexpr float TITLE_PADDING = 10.0f;
        
        // Colors
        struct Colors {
            ImVec4 background = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
            ImVec4 border = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
            ImVec4 text = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
            ImVec4 textSecondary = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
            ImVec4 button = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
            ImVec4 buttonHovered = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
            ImVec4 buttonActive = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
            ImVec4 closeButtonHovered = ImVec4(0.8f, 0.1f, 0.1f, 1.0f);
            ImVec4 closeButtonActive = ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
        } m_colors;
    };

} // namespace scummredux
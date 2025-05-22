#include "TitleBar.h"
#include "../core/Window.h"
#include "../res/icons/MaterialSymbols.h"
#include "../utils/Utils.h"
#include "../utils/Events.hpp"
#include <algorithm>

namespace scummredux {

    TitleBar::TitleBar(Window* window)
        : m_window(window) {
    }

    void TitleBar::render() {
        if (!m_window) return;

        // Title bar background
        ImGui::PushStyleColor(ImGuiCol_ChildBg, m_colors.background);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.0f);

        if (ImGui::BeginChild("##titleBar", ImVec2(ImGui::GetWindowWidth(), m_height), false,
                             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {

            // Render components from left to right
            if (m_showIcon) {
                renderIcon();
            }

            renderTitle();

            if (m_showButtons) {
                renderCustomButtons();
                renderWindowControls();
            }

            // Handle dragging and double-click (simplificado para Windows)
            if (m_draggable) {
                handleDragging();
                handleDoubleClick();
            }
        }
        ImGui::EndChild();

        ImGui::PopStyleVar(1);
        ImGui::PopStyleColor(1);

        // Title bar border
        ImGui::PushStyleColor(ImGuiCol_Separator, m_colors.border);
        ImGui::Separator();
        ImGui::PopStyleColor();
    }

    void TitleBar::renderIcon() {
        const float padding = TITLE_PADDING;

        // Position for icon
        ImGui::SetCursorPos(ImVec2(padding, (m_height - ICON_SIZE) * 0.5f));

        // Use Material Design icon for application
        ImGui::PushStyleColor(ImGuiCol_Text, m_colors.text);
        ImGui::Text(ICON_MS_TERMINAL); // Application icon
        ImGui::PopStyleColor();
    }

    void TitleBar::renderTitle() {
        const float iconWidth = m_showIcon ? (ICON_SIZE + TITLE_PADDING * 2) : TITLE_PADDING;
        const float buttonsWidth = m_showButtons ?
            ((BUTTON_SIZE + BUTTON_PADDING) * (3 + m_customButtons.size()) + BUTTON_PADDING) : 0;

        // Calculate available space for title
        const float windowWidth = ImGui::GetWindowWidth();
        const float availableWidth = windowWidth - iconWidth - buttonsWidth;

        // Determine what to display
        std::string displayTitle;
        std::string displaySubtitle;

        if (!m_projectName.empty()) {
            displayTitle = m_projectName;
            if (m_hasUnsavedChanges) {
                displayTitle += " (*)";
            }
            displaySubtitle = m_title;
        } else {
            displayTitle = m_title;
            displaySubtitle = m_subtitle;
        }

        // Limit title length if necessary
        const float maxTitleWidth = availableWidth * 0.8f;
        ImVec2 titleSize = ImGui::CalcTextSize(displayTitle.c_str());

        if (titleSize.x > maxTitleWidth) {
            displayTitle = utils::limitStringLength(displayTitle, 40);
        }

        // Calculate centered position
        titleSize = ImGui::CalcTextSize(displayTitle.c_str());
        float titleX = iconWidth + (availableWidth - titleSize.x) * 0.5f;

        // Main title
        ImGui::SetCursorPos(ImVec2(titleX, (m_height - ImGui::GetTextLineHeight()) * 0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, m_colors.text);
        ImGui::Text("%s", displayTitle.c_str());
        ImGui::PopStyleColor();

        // Subtitle (if any)
        if (!displaySubtitle.empty()) {
            ImVec2 subtitleSize = ImGui::CalcTextSize(displaySubtitle.c_str());
            float subtitleX = iconWidth + (availableWidth - subtitleSize.x) * 0.5f;

            ImGui::SetCursorPos(ImVec2(subtitleX, m_height * 0.5f + 2));
            ImGui::PushStyleColor(ImGuiCol_Text, m_colors.textSecondary);
            ImGui::Text("%s", displaySubtitle.c_str());
            ImGui::PopStyleColor();
        }
    }

    void TitleBar::renderCustomButtons() {
        if (m_customButtons.empty()) return;

        // Calculate starting position for custom buttons
        const float windowWidth = ImGui::GetWindowWidth();
        const float windowControlsWidth = (BUTTON_SIZE + BUTTON_PADDING) * 3;
        float currentX = windowWidth - windowControlsWidth - BUTTON_PADDING;

        // Render custom buttons from right to left
        for (auto it = m_customButtons.rbegin(); it != m_customButtons.rend(); ++it) {
            const auto& [id, button] = *it;

            currentX -= (BUTTON_SIZE + BUTTON_PADDING);

            ImGui::SetCursorPos(ImVec2(currentX, (m_height - BUTTON_SIZE) * 0.5f));

            ImGui::PushStyleColor(ImGuiCol_Button, m_colors.button);
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colors.buttonHovered);
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colors.buttonActive);
            ImGui::PushStyleColor(ImGuiCol_Text, m_colors.text);

            ImGui::BeginDisabled(!button.enabled);
            if (ImGui::Button((button.icon + "##" + id).c_str(), ImVec2(BUTTON_SIZE, BUTTON_SIZE))) {
                if (button.callback) {
                    button.callback();
                }
            }
            ImGui::EndDisabled();

            // Tooltip
            if (!button.tooltip.empty() && ImGui::IsItemHovered()) {
                ImGui::BeginTooltip();
                ImGui::Text("%s", button.tooltip.c_str());
                ImGui::EndTooltip();
            }

            ImGui::PopStyleColor(4);
        }
    }

    void TitleBar::renderWindowControls() {
        const float windowWidth = ImGui::GetWindowWidth();

        // Minimize button
        ImGui::SetCursorPos(ImVec2(windowWidth - BUTTON_SIZE * 3 - BUTTON_PADDING * 3,
                                   (m_height - BUTTON_SIZE) * 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, m_colors.button);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colors.buttonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colors.buttonActive);
        ImGui::PushStyleColor(ImGuiCol_Text, m_colors.text);

        if (ImGui::Button(ICON_MS_MINIMIZE "##minimize", ImVec2(BUTTON_SIZE, BUTTON_SIZE))) {
            minimizeWindow();
        }
        ImGui::PopStyleColor(4);

        // Maximize/Restore button
        ImGui::SetCursorPos(ImVec2(windowWidth - BUTTON_SIZE * 2 - BUTTON_PADDING * 2,
                                   (m_height - BUTTON_SIZE) * 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, m_colors.button);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colors.buttonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colors.buttonActive);
        ImGui::PushStyleColor(ImGuiCol_Text, m_colors.text);

        const char* maximizeIcon = m_window->isMaximized() ? ICON_MS_CROP_DIN : ICON_MS_CROP_SQUARE;
        if (ImGui::Button((std::string(maximizeIcon) + "##maximize").c_str(), ImVec2(BUTTON_SIZE, BUTTON_SIZE))) {
            maximizeWindow();
        }
        ImGui::PopStyleColor(4);

        // Close button (with special hover color)
        ImGui::SetCursorPos(ImVec2(windowWidth - BUTTON_SIZE - BUTTON_PADDING,
                                   (m_height - BUTTON_SIZE) * 0.5f));

        ImGui::PushStyleColor(ImGuiCol_Button, m_colors.button);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_colors.closeButtonHovered);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_colors.closeButtonActive);
        ImGui::PushStyleColor(ImGuiCol_Text, m_colors.text);

        if (ImGui::Button(ICON_MS_CLOSE "##close", ImVec2(BUTTON_SIZE, BUTTON_SIZE))) {
            closeWindow();
        }
        ImGui::PopStyleColor(4);
    }

    void TitleBar::handleDragging() {
        // No Windows, o drag é tratado pelo WM_NCHITTEST nativo
        // Aqui só precisamos desenhar a área de drag
        const float windowWidth = ImGui::GetWindowWidth();
        const float buttonsWidth = m_showButtons ? 150.0f : 0.0f;
        const float dragAreaWidth = windowWidth - buttonsWidth;

        ImGui::SetCursorPos(ImVec2(0, 0));
        ImGui::InvisibleButton("##dragArea", ImVec2(dragAreaWidth, m_height));
    }

    void TitleBar::handleDoubleClick() {
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            maximizeWindow();
        }
    }

    void TitleBar::minimizeWindow() {
        if (m_window) {
            m_window->minimize();
        }
    }

    void TitleBar::maximizeWindow() {
        if (m_window) {
            if (m_window->isMaximized()) {
                m_window->restore();
            } else {
                m_window->maximize();
            }
        }
    }

    void TitleBar::closeWindow() {
        if (m_window) {
            // Post close event - let the application handle cleanup
            EventWindowClose::post({true});
        }
    }

    // Button management
    void TitleBar::addButton(const std::string& id, const ButtonInfo& button) {
        // Remove existing button with same id
        removeButton(id);

        // Add new button
        m_customButtons.emplace_back(id, button);
    }

    void TitleBar::removeButton(const std::string& id) {
        auto it = std::find_if(m_customButtons.begin(), m_customButtons.end(),
            [&id](const auto& pair) { return pair.first == id; });

        if (it != m_customButtons.end()) {
            m_customButtons.erase(it);
        }
    }

    void TitleBar::setButtonEnabled(const std::string& id, bool enabled) {
        auto it = std::find_if(m_customButtons.begin(), m_customButtons.end(),
            [&id](const auto& pair) { return pair.first == id; });

        if (it != m_customButtons.end()) {
            it->second.enabled = enabled;
        }
    }

    // Title management
    void TitleBar::setTitle(const std::string& title) {
        m_title = title;
        if (m_window) {
            m_window->setTitle(title);
        }
    }

    void TitleBar::setSubtitle(const std::string& subtitle) {
        m_subtitle = subtitle;
    }

    void TitleBar::setProjectInfo(const std::string& projectName, bool hasUnsavedChanges) {
        m_projectName = projectName;
        m_hasUnsavedChanges = hasUnsavedChanges;
    }

    // Helper functions
    bool TitleBar::isMouseInDragArea() const {
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 windowPos = ImGui::GetWindowPos();

        return mousePos.x >= windowPos.x &&
               mousePos.x <= windowPos.x + ImGui::GetWindowWidth() - 150.0f &&
               mousePos.y >= windowPos.y &&
               mousePos.y <= windowPos.y + m_height;
    }

    ImVec2 TitleBar::calculateTitlePosition() const {
        const float iconWidth = m_showIcon ? (ICON_SIZE + TITLE_PADDING * 2) : TITLE_PADDING;
        const float buttonsWidth = m_showButtons ? 150.0f : 0.0f;
        
        const float windowWidth = ImGui::GetWindowWidth();
        const float availableWidth = windowWidth - iconWidth - buttonsWidth;
        
        std::string displayTitle = m_projectName.empty() ? m_title : m_projectName;
        ImVec2 titleSize = ImGui::CalcTextSize(displayTitle.c_str());
        
        float titleX = iconWidth + (availableWidth - titleSize.x) * 0.5f;
        float titleY = (m_height - ImGui::GetTextLineHeight()) * 0.5f;
        
        return ImVec2(titleX, titleY);
    }

    ImVec2 TitleBar::calculateButtonPosition(int buttonIndex, bool fromRight) const {
        const float windowWidth = ImGui::GetWindowWidth();
        
        if (fromRight) {
            return ImVec2(windowWidth - (BUTTON_SIZE + BUTTON_PADDING) * (buttonIndex + 1),
                         (m_height - BUTTON_SIZE) * 0.5f);
        } else {
            return ImVec2((BUTTON_SIZE + BUTTON_PADDING) * buttonIndex + BUTTON_PADDING,
                         (m_height - BUTTON_SIZE) * 0.5f);
        }
    }

} // namespace scummredux
#include "WindowDecorator.h"
#include "../core/Window.h"
#include "../utils/Utils.h"
#include <iostream>

namespace scummredux {

    WindowDecorator::WindowDecorator(Window* window)
        : m_titleBar(std::make_unique<TitleBar>(window)) {
        // Store window reference in TitleBar, not here
    }

    void WindowDecorator::render() {
        renderMainFrame();
    }

    void WindowDecorator::renderMainFrame() {
        // Setup main window that covers the entire viewport
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        // Main window flags - no decoration, docking enabled
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoDocking;

        // Add menu bar flag if title bar is shown
        if (m_showTitleBar) {
            windowFlags |= ImGuiWindowFlags_MenuBar;
        }

        // Apply main window styling
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, m_style.windowPadding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, m_style.windowRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, m_style.borderSize);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, m_style.backgroundColor);

        // Begin main window
        if (ImGui::Begin("MainWindow##ScummRedux", nullptr, windowFlags)) {
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);

            // Draw title bar backdrop effect
            if (m_showTitleBar) {
                drawTitleBarBackdrop();
            }

            // Render title bar in menu bar area
            if (m_showTitleBar && ImGui::BeginMenuBar()) {
                m_titleBar->render();
                ImGui::EndMenuBar();
            }

            // Setup and render dock space
            renderDockSpace();

            // Call content callback if set
            if (m_drawContentCallback) {
                m_drawContentCallback();
            }
        } else {
            ImGui::PopStyleVar(3);
            ImGui::PopStyleColor(1);
        }
        ImGui::End();
    }

    void WindowDecorator::renderDockSpace() {
        // Get or generate dock space ID
        if (m_mainDockSpaceId == 0) {
            m_mainDockSpaceId = ImGui::GetID("MainDockSpace##ScummRedux");
        }

        // DockSpace flags
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

        // Get available space for docking
        // ImVec2 dockspaceSize = ImGui::GetContentRegionAvail();

        // Create the dock space
        ImGui::DockSpace(m_mainDockSpaceId, ImVec2(0, 0), dockspaceFlags);

        // Setup initial layout on first frame
        if (m_isFirstFrame) {
            setupInitialLayout();
            m_isFirstFrame = false;
        }
    }

    void WindowDecorator::setupInitialLayout() {
        // For now, we'll use a simpler approach without DockBuilder
        // The views will dock themselves when they are first opened

        // TODO: Implement proper initial layout when DockBuilder is available
        // This is a placeholder that sets up basic docking

        std::cout << "Initial dock layout setup (simplified)" << std::endl;
        m_isFirstFrame = false;
    }

    void WindowDecorator::drawTitleBarBackdrop() {
        // Optional: Draw a subtle backdrop effect behind the title bar
        // This can be used for visual effects like the ImHex shadow circle

        const auto windowPos = ImGui::GetWindowPos();
        const auto windowSize = ImGui::GetWindowSize();

        // For now, just ensure the title bar area has proper background
        // More elaborate effects can be added here later

        auto drawList = ImGui::GetWindowDrawList();
        const float titleBarHeight = m_titleBar->getHeight();

        // Draw title bar background rectangle
        drawList->AddRectFilled(
            windowPos,
            ImVec2(windowPos.x + windowSize.x, windowPos.y + titleBarHeight),
            ImGui::GetColorU32(ImGuiCol_MenuBarBg)
        );
    }

} // namespace scummredux
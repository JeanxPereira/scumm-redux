#include "WindowDecorator.h"
#include "../core/Window.h"
#include "../utils/Utils.h"
#include <iostream>

namespace scummredux {

    WindowDecorator::WindowDecorator(Window* window)
        : m_titleBar(std::make_unique<TitleBar>(window)) {
    }

    void WindowDecorator::render() {
        renderMainFrame();
    }

    void WindowDecorator::renderMainFrame() {
        // Setup main window that covers the entire viewport (exatamente como ImHex)
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Flags da janela principal (baseado no ImHex)
        ImGuiWindowFlags windowFlags =
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse;

        // Add menu bar flag if title bar is shown
        if (m_showTitleBar) {
            windowFlags |= ImGuiWindowFlags_MenuBar;
        }

        // Styling (baseado no ImHex)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        // Begin main window
        if (ImGui::Begin("ImHexDockSpace", nullptr, windowFlags)) {
            ImGui::PopStyleVar(2); // Pop 2 style vars, keep WindowPadding for later

            // Render title bar in menu bar area
            if (m_showTitleBar && ImGui::BeginMenuBar()) {
                m_titleBar->render();
                ImGui::EndMenuBar();
            }

            // Setup and render dock space (método do ImHex)
            renderDockSpace();

            // Call content callback if set
            if (m_drawContentCallback) {
                m_drawContentCallback();
            }
        } else {
            ImGui::PopStyleVar(2);
        }
        ImGui::End();

        ImGui::PopStyleVar(1); // Pop WindowPadding
    }

    void WindowDecorator::renderDockSpace() {
        // Get available space for docking
        ImVec2 dockSpaceSize = ImGui::GetContentRegionAvail();

        // Only proceed if we have valid size
        if (dockSpaceSize.x <= 0 || dockSpaceSize.y <= 0) {
            return;
        }

        // SOLUÇÃO BASEADA NO IMHEX: Deixar o DockSpace gerar e retornar o ID
        // Não pré-gerar o ID, usar o retornado pelo DockSpace
        ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

        // Criar o DockSpace e capturar o ID retornado (como no ImHex)
        auto dockId = ImGui::DockSpace(ImGui::GetID("ScummReduxMainDock"), dockSpaceSize, dockspaceFlags);

        // Armazenar o ID retornado (como no ImHex faz)
        if (m_mainDockSpaceId == 0) {
            m_mainDockSpaceId = dockId;
            std::cout << "DockSpace ID set to: " << dockId << std::endl;
        }

        // Setup initial layout on first frame
        if (m_isFirstFrame) {
            setupInitialLayout();
            m_isFirstFrame = false;
        }
    }

    void WindowDecorator::setupInitialLayout() {
        // Simple initial layout setup (como no ImHex)
        std::cout << "Initial dock layout setup (ImHex style)" << std::endl;
        m_isFirstFrame = false;
    }

    void WindowDecorator::drawTitleBarBackdrop() {
        // Optional: Draw a subtle backdrop effect behind the title bar
        const auto windowPos = ImGui::GetWindowPos();
        const auto windowSize = ImGui::GetWindowSize();

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
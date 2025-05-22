#include "PropertiesView.h"
#include "../res/icons/MaterialSymbols.h"
#include "../core/Settings.h"
#include "../ui/StyleManager.h"
#include <iostream>

namespace scummredux {

    PropertiesView::PropertiesView() : View("Properties") {
        std::cout << "PropertiesView minimal constructor" << std::endl;

        // Initialize minimal state to avoid crashes
        m_showAppearance = true;
        m_showEditor = true;
        m_showProject = true;
        m_showPerformance = false;
        m_settingsChanged = false;
    }

    void PropertiesView::draw() {
        std::cout << "PropertiesView::draw() start" << std::endl;

        try {
            if (ImGui::Begin(getWindowName().c_str(), &getWindowOpenState())) {
                std::cout << "PropertiesView window created successfully" << std::endl;

                // MINIMAL VERSION - just text for now to avoid crashes
                ImGui::Text(ICON_MS_SETTINGS " Settings");
                ImGui::Separator();

                // Simple expandable sections without complex widgets
                if (ImGui::CollapsingHeader(ICON_MS_PALETTE " Appearance")) {
                    ImGui::Indent();
                    ImGui::Text("Theme settings will go here");
                    ImGui::Text("Font settings will go here");
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader(ICON_MS_CODE " Editor")) {
                    ImGui::Indent();
                    ImGui::Text("Editor settings will go here");
                    ImGui::Text("Tab size, word wrap, etc.");
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader(ICON_MS_FOLDER " Project")) {
                    ImGui::Indent();
                    ImGui::Text("Project settings will go here");
                    ImGui::Text("Build configuration, etc.");
                    ImGui::Unindent();
                }

                if (ImGui::CollapsingHeader(ICON_MS_SPEED " Performance")) {
                    ImGui::Indent();
                    ImGui::Text("Performance settings will go here");
                    ImGui::Text("FPS limits, VSync, etc.");
                    ImGui::Unindent();
                }

                ImGui::Separator();

                // Simple buttons
                if (ImGui::Button("Apply", ImVec2(80, 0))) {
                    std::cout << "Apply clicked" << std::endl;
                }
                ImGui::SameLine();

                if (ImGui::Button("Reset", ImVec2(80, 0))) {
                    std::cout << "Reset clicked" << std::endl;
                }

                std::cout << "PropertiesView content drawn successfully" << std::endl;
            } else {
                std::cout << "PropertiesView window failed to create" << std::endl;
            }
            ImGui::End();
        } catch (const std::exception& e) {
            std::cout << "Exception in PropertiesView::draw(): " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception in PropertiesView::draw()" << std::endl;
        }

        std::cout << "PropertiesView::draw() end" << std::endl;
    }

    void PropertiesView::drawAppearanceSettings() {
        ImGui::Text("Appearance settings placeholder");
    }

    void PropertiesView::drawEditorSettings() {
        ImGui::Text("Editor settings placeholder");
    }

    void PropertiesView::drawProjectSettings() {
        ImGui::Text("Project settings placeholder");
    }

    void PropertiesView::drawPerformanceSettings() {
        ImGui::Text("Performance settings placeholder");
    }

} // namespace scummredux
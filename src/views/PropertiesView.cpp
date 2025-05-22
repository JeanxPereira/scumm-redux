#include "PropertiesView.h"
#include "../res/icons/MaterialSymbols.h"
#include "../core/Settings.h"
#include "../ui/StyleManager.h"

namespace scummredux {

    PropertiesView::PropertiesView() : View("Properties") {
        // Load current settings into temp settings
        auto& settings = Settings::getInstance();
        // auto& styleManager = StyleManager::getInstance(); // Unused for now

        m_tempSettings.darkTheme = settings.get<bool>(Settings::UI::DARK_THEME, true);
        m_tempSettings.fontSize = settings.get<float>(Settings::UI::FONT_SIZE, 14.0f);
        m_tempSettings.accentColor = settings.get<ImVec4>(Settings::UI::ACCENT_COLOR, ImVec4(0.43f, 0.43f, 0.50f, 1.0f));
        m_tempSettings.windowRounding = settings.get<float>(Settings::UI::WINDOW_ROUNDING, 6.0f);
        m_tempSettings.frameRounding = settings.get<float>(Settings::UI::FRAME_ROUNDING, 3.0f);

        m_tempSettings.tabSize = settings.get<int>(Settings::Editor::TAB_SIZE, 4);
        m_tempSettings.wordWrap = settings.get<bool>(Settings::Editor::WORD_WRAP, false);
        m_tempSettings.showLineNumbers = settings.get<bool>(Settings::Editor::SHOW_LINE_NUMBERS, true);
        m_tempSettings.autoIndent = settings.get<bool>(Settings::Editor::AUTO_INDENT, true);

        m_tempSettings.projectName = settings.get<std::string>(Settings::App::APP_NAME, "SCUMM Redux");
    }

    void PropertiesView::draw() {
        if (ImGui::Begin(getWindowName().c_str(), &getWindowOpenState())) {
            ImGui::Text(ICON_MS_SETTINGS " Settings");
            ImGui::Separator();

            // Scrollable region for settings
            if (ImGui::BeginChild("SettingsRegion", ImVec2(0, -35), false)) {
                drawAppearanceSettings();
                drawEditorSettings();
                drawProjectSettings();
                drawPerformanceSettings();
            }
            ImGui::EndChild();

            // Apply/Reset buttons
            ImGui::Separator();
            if (ImGui::Button("Apply", ImVec2(80, 0))) {
                // Apply all settings
                auto& settings = Settings::getInstance();
                auto& styleManager = StyleManager::getInstance();

                // UI Settings
                settings.set(Settings::UI::DARK_THEME, m_tempSettings.darkTheme);
                settings.set(Settings::UI::FONT_SIZE, m_tempSettings.fontSize);
                settings.set(Settings::UI::ACCENT_COLOR, m_tempSettings.accentColor);
                settings.set(Settings::UI::WINDOW_ROUNDING, m_tempSettings.windowRounding);
                settings.set(Settings::UI::FRAME_ROUNDING, m_tempSettings.frameRounding);

                // Editor Settings
                settings.set(Settings::Editor::TAB_SIZE, m_tempSettings.tabSize);
                settings.set(Settings::Editor::WORD_WRAP, m_tempSettings.wordWrap);
                settings.set(Settings::Editor::SHOW_LINE_NUMBERS, m_tempSettings.showLineNumbers);
                settings.set(Settings::Editor::AUTO_INDENT, m_tempSettings.autoIndent);

                // App Settings
                settings.set(Settings::App::APP_NAME, m_tempSettings.projectName);

                // Apply theme changes
                styleManager.setTheme(m_tempSettings.darkTheme ? "Dark" : "Light");
                styleManager.setAccentColor(m_tempSettings.accentColor);
                styleManager.setWindowRounding(m_tempSettings.windowRounding);
                styleManager.setFrameRounding(m_tempSettings.frameRounding);
                styleManager.setFontSize(m_tempSettings.fontSize);

                m_settingsChanged = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Reset", ImVec2(80, 0))) {
                // Reset to saved settings
                auto& settings = Settings::getInstance();

                m_tempSettings.darkTheme = settings.get<bool>(Settings::UI::DARK_THEME, true);
                m_tempSettings.fontSize = settings.get<float>(Settings::UI::FONT_SIZE, 14.0f);
                m_tempSettings.accentColor = settings.get<ImVec4>(Settings::UI::ACCENT_COLOR, ImVec4(0.43f, 0.43f, 0.50f, 1.0f));
                m_tempSettings.windowRounding = settings.get<float>(Settings::UI::WINDOW_ROUNDING, 6.0f);
                m_tempSettings.frameRounding = settings.get<float>(Settings::UI::FRAME_ROUNDING, 3.0f);

                m_settingsChanged = false;
            }

            ImGui::SameLine();
            if (m_settingsChanged) {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), "Changes pending...");
            }
        }
        ImGui::End();
    }

    void PropertiesView::drawAppearanceSettings() {
        if (ImGui::CollapsingHeader(ICON_MS_PALETTE " Appearance", m_showAppearance ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
            ImGui::Indent();

            // Theme selection
            if (ImGui::Checkbox("Dark Theme", &m_tempSettings.darkTheme)) {
                m_settingsChanged = true;
            }

            // Font size
            if (ImGui::SliderFloat("Font Size", &m_tempSettings.fontSize, 10.0f, 24.0f, "%.0f px")) {
                m_settingsChanged = true;
            }

            // Accent color
            if (ImGui::ColorEdit4("Accent Color", &m_tempSettings.accentColor.x, ImGuiColorEditFlags_NoAlpha)) {
                m_settingsChanged = true;
            }

            // Rounding
            if (ImGui::SliderFloat("Window Rounding", &m_tempSettings.windowRounding, 0.0f, 15.0f, "%.1f")) {
                m_settingsChanged = true;
            }

            if (ImGui::SliderFloat("Frame Rounding", &m_tempSettings.frameRounding, 0.0f, 10.0f, "%.1f")) {
                m_settingsChanged = true;
            }

            ImGui::Unindent();
        }
    }

    void PropertiesView::drawEditorSettings() {
        if (ImGui::CollapsingHeader(ICON_MS_CODE " Editor", m_showEditor ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
            ImGui::Indent();

            // Tab size
            if (ImGui::SliderInt("Tab Size", &m_tempSettings.tabSize, 1, 8)) {
                m_settingsChanged = true;
            }

            // Editor options
            if (ImGui::Checkbox("Word Wrap", &m_tempSettings.wordWrap)) {
                m_settingsChanged = true;
            }

            if (ImGui::Checkbox("Show Line Numbers", &m_tempSettings.showLineNumbers)) {
                m_settingsChanged = true;
            }

            if (ImGui::Checkbox("Auto Indent", &m_tempSettings.autoIndent)) {
                m_settingsChanged = true;
            }

            ImGui::Unindent();
        }
    }

    void PropertiesView::drawProjectSettings() {
        if (ImGui::CollapsingHeader(ICON_MS_FOLDER " Project", m_showProject ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
            ImGui::Indent();

            // Project name
            char projectNameBuffer[256];
            strncpy(projectNameBuffer, m_tempSettings.projectName.c_str(), sizeof(projectNameBuffer));
            if (ImGui::InputText("Project Name", projectNameBuffer, sizeof(projectNameBuffer))) {
                m_tempSettings.projectName = projectNameBuffer;
                m_settingsChanged = true;
            }

            // Build type
            const char* buildTypes[] = { "Debug", "Release", "RelWithDebInfo", "MinSizeRel" };
            int currentBuildType = 0;
            if (m_tempSettings.buildType == "Release") currentBuildType = 1;
            else if (m_tempSettings.buildType == "RelWithDebInfo") currentBuildType = 2;
            else if (m_tempSettings.buildType == "MinSizeRel") currentBuildType = 3;

            if (ImGui::Combo("Build Type", &currentBuildType, buildTypes, IM_ARRAYSIZE(buildTypes))) {
                m_tempSettings.buildType = buildTypes[currentBuildType];
                m_settingsChanged = true;
            }

            // Output directory
            char outputDirBuffer[512];
            strncpy(outputDirBuffer, m_tempSettings.outputDir.c_str(), sizeof(outputDirBuffer));
            if (ImGui::InputText("Output Directory", outputDirBuffer, sizeof(outputDirBuffer))) {
                m_tempSettings.outputDir = outputDirBuffer;
                m_settingsChanged = true;
            }

            ImGui::Unindent();
        }
    }

    void PropertiesView::drawPerformanceSettings() {
        if (ImGui::CollapsingHeader(ICON_MS_SPEED " Performance", m_showPerformance ? ImGuiTreeNodeFlags_DefaultOpen : 0)) {
            ImGui::Indent();

            // Target FPS
            if (ImGui::SliderFloat("Target FPS", &m_tempSettings.targetFPS, 30.0f, 120.0f, "%.0f")) {
                m_settingsChanged = true;
            }

            // VSync
            if (ImGui::Checkbox("Enable VSync", &m_tempSettings.vsync)) {
                m_settingsChanged = true;
            }

            // Show FPS counter
            if (ImGui::Checkbox("Show FPS Counter", &m_tempSettings.showFPS)) {
                m_settingsChanged = true;
            }

            ImGui::Unindent();
        }
    }

} // namespace scummredux
#pragma once

#include "View.h"
#include <string>

namespace scummredux {

    class PropertiesView : public View {
    public:
        PropertiesView();
        ~PropertiesView() override = default;

        void draw() override;

    private:
        void drawAppearanceSettings();
        void drawEditorSettings();
        void drawProjectSettings();
        void drawPerformanceSettings();

        // Settings categories
        bool m_showAppearance = true;
        bool m_showEditor = true;
        bool m_showProject = true;
        bool m_showPerformance = false;

        // Temporary settings (before applying)
        struct TempSettings {
            bool darkTheme = true;
            float fontSize = 14.0f;
            ImVec4 accentColor = ImVec4(0.43f, 0.43f, 0.50f, 1.0f);
            float windowRounding = 6.0f;
            float frameRounding = 3.0f;
            
            // Editor
            int tabSize = 4;
            bool wordWrap = false;
            bool showLineNumbers = true;
            bool autoIndent = true;
            
            // Project
            std::string projectName = "SCUMM Redux";
            std::string buildType = "Debug";
            std::string outputDir = "build/";
            
            // Performance
            float targetFPS = 60.0f;
            bool vsync = true;
            bool showFPS = false;
        } m_tempSettings;

        // Flags for changes
        bool m_settingsChanged = false;
    };

} // namespace scummredux
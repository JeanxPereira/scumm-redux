#pragma once

#include "View.h"
#include <string>
#include <vector>

namespace scummredux {

    class EditorView : public View {
    public:
        EditorView();
        ~EditorView() override = default;

        void draw() override;

        // File management
        void openFile(const std::string& filePath);
        void saveCurrentFile();
        void closeCurrentFile();

        // Editor state
        bool hasUnsavedChanges() const { return m_hasUnsavedChanges; }
        std::string getCurrentFileName() const { return m_currentFileName; }

    private:
        void drawToolbar();
        void drawEditor();
        void drawStatusBar();
        void drawTabBar();

        // Editor content
        std::string m_content;
        std::string m_currentFileName;
        std::string m_currentFilePath;
        bool m_hasUnsavedChanges = false;

        // Editor settings
        bool m_showLineNumbers = true;
        bool m_wordWrap = false;
        int m_tabSize = 4;
        bool m_autoIndent = true;

        // Multiple files support (tabs)
        struct EditorTab {
            std::string name;
            std::string path;
            std::string content;
            bool hasUnsavedChanges = false;
            bool isActive = false;
        };
        
        std::vector<EditorTab> m_tabs;
        int m_activeTabIndex = -1;

        // Editor state
        ImVec2 m_scrollPosition = ImVec2(0, 0);
        int m_cursorLine = 1;
        int m_cursorColumn = 1;
        int m_totalLines = 1;
    };

} // namespace scummredux
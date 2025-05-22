#include "EditorView.h"
#include "../res/icons/MaterialSymbols.h"
#include "../core/Settings.h"
#include <fstream>
#include <sstream>

namespace scummredux {

    EditorView::EditorView() : View("Editor") {
        // Load editor settings
        auto& settings = Settings::getInstance();
        m_showLineNumbers = settings.get<bool>(Settings::Editor::SHOW_LINE_NUMBERS, true);
        m_wordWrap = settings.get<bool>(Settings::Editor::WORD_WRAP, false);
        m_tabSize = settings.get<int>(Settings::Editor::TAB_SIZE, 4);
        m_autoIndent = settings.get<bool>(Settings::Editor::AUTO_INDENT, true);

        // Default content
        m_content =
            "#include <iostream>\n"
            "#include \"ScummRedux.h\"\n"
            "\n"
            "int main() {\n"
            "    std::cout << \"Welcome to SCUMM Redux!\" << std::endl;\n"
            "    \n"
            "    // Initialize the application\n"
            "    ScummRedux::Application app;\n"
            "    \n"
            "    // Run the main loop\n"
            "    return app.run();\n"
            "}\n";

        m_currentFileName = "main.cpp";
        m_totalLines = std::count(m_content.begin(), m_content.end(), '\n') + 1;
    }

    void EditorView::draw() {
        if (ImGui::Begin(getWindowName().c_str(), &getWindowOpenState())) {
            drawToolbar();

            if (!m_tabs.empty()) {
                drawTabBar();
            }

            drawEditor();
            drawStatusBar();
        }
        ImGui::End();
    }

    void EditorView::drawToolbar() {
        // File operations
        if (ImGui::Button(ICON_MS_FOLDER_OPEN "##open")) {
            // TODO: Open file dialog
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Open File (Ctrl+O)");
        }

        ImGui::SameLine();
        if (ImGui::Button(ICON_MS_SAVE "##save")) {
            saveCurrentFile();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Save File (Ctrl+S)");
        }

        ImGui::SameLine();
        ImGui::BeginDisabled(!m_hasUnsavedChanges);
        if (ImGui::Button(ICON_MS_SAVE_AS "##saveas")) {
            // TODO: Save as dialog
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Save As (Ctrl+Shift+S)");
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        // Edit operations
        if (ImGui::Button(ICON_MS_UNDO "##undo")) {
            // TODO: Undo
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Undo (Ctrl+Z)");
        }

        ImGui::SameLine();
        if (ImGui::Button(ICON_MS_REDO "##redo")) {
            // TODO: Redo
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Redo (Ctrl+Y)");
        }

        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        // Search
        if (ImGui::Button(ICON_MS_SEARCH "##search")) {
            // TODO: Find dialog
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Find (Ctrl+F)");
        }

        ImGui::SameLine();
        if (ImGui::Button(ICON_MS_FIND_REPLACE "##replace")) {
            // TODO: Find and replace dialog
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Find and Replace (Ctrl+H)");
        }

        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        // View options
        ImGui::Checkbox("Line Numbers", &m_showLineNumbers);
        ImGui::SameLine();
        ImGui::Checkbox("Word Wrap", &m_wordWrap);

        ImGui::Separator();
    }

    void EditorView::drawTabBar() {
        if (ImGui::BeginTabBar("EditorTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_AutoSelectNewTabs)) {
            for (size_t i = 0; i < m_tabs.size(); i++) {
                auto& tab = m_tabs[i];

                // Tab name with unsaved indicator
                std::string tabName = tab.name;
                if (tab.hasUnsavedChanges) {
                    tabName += " *";
                }

                bool isOpen = true;
                if (ImGui::BeginTabItem(tabName.c_str(), &isOpen)) {
                    if (static_cast<size_t>(m_activeTabIndex) != i) {
                        // Switch to this tab
                        m_activeTabIndex = i;
                        m_content = tab.content;
                        m_currentFileName = tab.name;
                        m_currentFilePath = tab.path;
                        m_hasUnsavedChanges = tab.hasUnsavedChanges;
                    }
                    ImGui::EndTabItem();
                }

                // Handle tab close
                if (!isOpen) {
                    // TODO: Check for unsaved changes
                    m_tabs.erase(m_tabs.begin() + i);
                    if (static_cast<size_t>(m_activeTabIndex) >= i) {
                        m_activeTabIndex--;
                    }
                    if (m_tabs.empty()) {
                        m_activeTabIndex = -1;
                        m_content.clear();
                        m_currentFileName.clear();
                        m_currentFilePath.clear();
                        m_hasUnsavedChanges = false;
                    }
                    break;
                }
            }
            ImGui::EndTabBar();
        }
    }

    void EditorView::drawEditor() {
        // Calculate editor area
        ImVec2 editorSize = ImGui::GetContentRegionAvail();
        editorSize.y -= 25; // Reserve space for status bar

        // Line numbers column width
        float lineNumberWidth = 0;
        if (m_showLineNumbers) {
            lineNumberWidth = ImGui::CalcTextSize("9999").x + 20; // Max 4 digits + padding
            editorSize.x -= lineNumberWidth;
        }

        // Editor background
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.15f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        // Line numbers
        if (m_showLineNumbers) {
            if (ImGui::BeginChild("LineNumbers", ImVec2(lineNumberWidth, editorSize.y), true, ImGuiWindowFlags_NoScrollbar)) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

                for (int i = 1; i <= m_totalLines; i++) {
                    if (i == m_cursorLine) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
                        ImGui::Text("%4d", i);
                        ImGui::PopStyleColor();
                    } else {
                        ImGui::Text("%4d", i);
                    }
                }

                ImGui::PopStyleColor();
            }
            ImGui::EndChild();
            ImGui::SameLine();
        }

        // Main editor
        if (ImGui::BeginChild("Editor", editorSize, true)) {
            // Multi-line text editor
            ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            if (!m_wordWrap) {
                flags |= ImGuiInputTextFlags_NoHorizontalScroll;
            }

            // Resize buffer if needed
            static std::string editBuffer = m_content;
            if (editBuffer.capacity() < 16384) {
                editBuffer.reserve(16384);
            }

            // Keep buffer synchronized with content
            if (editBuffer != m_content) {
                editBuffer = m_content;
            }

            if (ImGui::InputTextMultiline("##editor", editBuffer.data(), editBuffer.capacity(),
                                         ImGui::GetContentRegionAvail(), flags)) {
                m_content = editBuffer.c_str();
                m_hasUnsavedChanges = true;

                // Update line count
                m_totalLines = std::count(m_content.begin(), m_content.end(), '\n') + 1;

                // Update current tab if exists
                if (m_activeTabIndex >= 0 && static_cast<size_t>(m_activeTabIndex) < m_tabs.size()) {
                    m_tabs[m_activeTabIndex].content = m_content;
                    m_tabs[m_activeTabIndex].hasUnsavedChanges = true;
                }
            }

            // TODO: Calculate cursor position for status bar
            // This would require more advanced text editor implementation
        }
        ImGui::EndChild();

        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void EditorView::drawStatusBar() {
        ImGui::Separator();

        // File info
        ImGui::Text("File: %s", m_currentFileName.empty() ? "Untitled" : m_currentFileName.c_str());

        ImGui::SameLine();
        if (m_hasUnsavedChanges) {
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.0f, 1.0f), " (Modified)");
        } else {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), " (Saved)");
        }

        // Cursor position
        ImGui::SameLine();
        float availableWidth = ImGui::GetContentRegionAvail().x;
        std::string positionText = "Line " + std::to_string(m_cursorLine) +
                                  ", Column " + std::to_string(m_cursorColumn);
        float textWidth = ImGui::CalcTextSize(positionText.c_str()).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + availableWidth - textWidth);
        ImGui::Text("%s", positionText.c_str());
    }

    void EditorView::openFile(const std::string& filePath) {
        // Check if file is already open
        for (size_t i = 0; i < m_tabs.size(); i++) {
            if (m_tabs[i].path == filePath) {
                m_activeTabIndex = i;
                m_content = m_tabs[i].content;
                m_currentFileName = m_tabs[i].name;
                m_currentFilePath = m_tabs[i].path;
                m_hasUnsavedChanges = m_tabs[i].hasUnsavedChanges;
                return;
            }
        }

        // Load file content
        std::ifstream file(filePath);
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();

            // Create new tab
            EditorTab tab;
            tab.path = filePath;
            tab.name = filePath.substr(filePath.find_last_of("/\\") + 1);
            tab.content = buffer.str();
            tab.hasUnsavedChanges = false;
            tab.isActive = true;

            m_tabs.push_back(tab);
            m_activeTabIndex = m_tabs.size() - 1;

            // Update current editor state
            m_content = tab.content;
            m_currentFileName = tab.name;
            m_currentFilePath = tab.path;
            m_hasUnsavedChanges = false;
            m_totalLines = std::count(m_content.begin(), m_content.end(), '\n') + 1;
        }
    }

    void EditorView::saveCurrentFile() {
        if (m_currentFilePath.empty()) {
            // TODO: Show save dialog
            return;
        }

        std::ofstream file(m_currentFilePath);
        if (file.is_open()) {
            file << m_content;
            m_hasUnsavedChanges = false;

            // Update tab state
            if (m_activeTabIndex >= 0 && static_cast<size_t>(m_activeTabIndex) < m_tabs.size()) {
                m_tabs[m_activeTabIndex].hasUnsavedChanges = false;
            }
        }
    }

    void EditorView::closeCurrentFile() {
        if (m_activeTabIndex >= 0 && static_cast<size_t>(m_activeTabIndex) < m_tabs.size()) {
            // TODO: Check for unsaved changes
            m_tabs.erase(m_tabs.begin() + m_activeTabIndex);
            
            if (m_tabs.empty()) {
                m_activeTabIndex = -1;
                m_content.clear();
                m_currentFileName.clear();
                m_currentFilePath.clear();
                m_hasUnsavedChanges = false;
            } else {
                m_activeTabIndex = std::min(m_activeTabIndex, (int)m_tabs.size() - 1);
                auto& tab = m_tabs[m_activeTabIndex];
                m_content = tab.content;
                m_currentFileName = tab.name;
                m_currentFilePath = tab.path;
                m_hasUnsavedChanges = tab.hasUnsavedChanges;
            }
        }
    }

} // namespace scummredux
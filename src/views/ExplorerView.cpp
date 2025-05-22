#include "ExplorerView.h"
#include "../res/icons/MaterialSymbols.h"
#include <functional>

namespace scummredux {

    ExplorerView::ExplorerView() : View("Explorer") {
        // Initialize with some sample project structure
        m_projectFiles = {
            {"src", "src/", true, true, {
                {"main.cpp", "src/main.cpp", false, false, {}},
                {"core", "src/core/", true, false, {
                    {"Application.cpp", "src/core/Application.cpp", false, false, {}},
                    {"Application.h", "src/core/Application.h", false, false, {}},
                    {"Window.cpp", "src/core/Window.cpp", false, false, {}},
                    {"Window.h", "src/core/Window.h", false, false, {}}
                }},
                {"ui", "src/ui/", true, false, {
                    {"TitleBar.cpp", "src/ui/TitleBar.cpp", false, false, {}},
                    {"TitleBar.h", "src/ui/TitleBar.h", false, false, {}}
                }}
            }},
            {"res", "res/", true, false, {
                {"fonts", "res/fonts/", true, false, {}},
                {"icons", "res/icons/", true, false, {}}
            }},
            {"CMakeLists.txt", "CMakeLists.txt", false, false, {}}
        };
    }

    void ExplorerView::draw() {
        if (ImGui::Begin(getWindowName().c_str(), &getWindowOpenState())) {
            // Header with project name
            ImGui::Text(ICON_MS_FOLDER " SCUMM Redux Project");
            ImGui::Separator();

            // Toolbar
            if (ImGui::Button(ICON_MS_REFRESH "##refresh")) {
                // Refresh file tree
            }
            ImGui::SameLine();

            if (ImGui::Button(ICON_MS_CREATE_NEW_FOLDER "##newfolder")) {
                // Create new folder
            }
            ImGui::SameLine();

            if (ImGui::Button(ICON_MS_NOTE_ADD "##newfile")) {
                // Create new file
            }
            ImGui::SameLine();

            ImGui::Checkbox("Hidden", &m_showHiddenFiles);

            ImGui::Separator();

            // File tree
            drawFileTree();
        }
        ImGui::End();
    }

    void ExplorerView::drawFileTree() {
        // Scrollable region for file tree
        if (ImGui::BeginChild("FileTreeRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            drawProjectFiles();
        }
        ImGui::EndChild();

        // Context menu
        drawContextMenu();
    }

    void ExplorerView::drawProjectFiles() {
        std::function<void(const FileNode&)> drawNode = [&](const FileNode& node) {
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

            if (node.name == m_selectedFile) {
                flags |= ImGuiTreeNodeFlags_Selected;
            }

            if (!node.isDirectory || node.children.empty()) {
                flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
            }

            if (node.isExpanded) {
                flags |= ImGuiTreeNodeFlags_DefaultOpen;
            }

            // Icon based on file type
            std::string icon;
            if (node.isDirectory) {
                icon = node.isExpanded ? ICON_MS_FOLDER_OPEN : ICON_MS_FOLDER;
            } else {
                // Determine file type icon
                if (node.name.ends_with(".cpp") || node.name.ends_with(".c")) {
                    icon = ICON_MS_CODE;
                } else if (node.name.ends_with(".h") || node.name.ends_with(".hpp")) {
                    icon = ICON_MS_CODE;
                } else if (node.name.ends_with(".txt")) {
                    icon = ICON_MS_DESCRIPTION;
                } else {
                    icon = ICON_MS_INSERT_DRIVE_FILE;
                }
            }

            std::string label = icon + " " + node.name;
            bool nodeOpen = ImGui::TreeNodeEx(node.path.c_str(), flags, "%s", label.c_str());

            // Handle selection
            if (ImGui::IsItemClicked()) {
                m_selectedFile = node.name;
            }

            // Handle double-click to open file
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                if (!node.isDirectory) {
                    // TODO: Signal to open file in editor
                    // EventFileOpenRequested::post({node.path});
                }
            }

            // Draw children if node is open
            if (nodeOpen && node.isDirectory) {
                for (const auto& child : node.children) {
                    drawNode(child);
                }
                ImGui::TreePop();
            }
        };

        for (const auto& node : m_projectFiles) {
            drawNode(node);
        }
    }

    void ExplorerView::drawContextMenu() {
        if (ImGui::BeginPopupContextWindow("ExplorerContextMenu")) {
            if (ImGui::MenuItem(ICON_MS_CREATE_NEW_FOLDER " New Folder")) {
                // Create new folder
            }

            if (ImGui::MenuItem(ICON_MS_NOTE_ADD " New File")) {
                // Create new file
            }

            ImGui::Separator();

            if (ImGui::MenuItem(ICON_MS_REFRESH " Refresh")) {
                // Refresh file tree
            }

            if (!m_selectedFile.empty()) {
                ImGui::Separator();

                if (ImGui::MenuItem(ICON_MS_EDIT " Rename")) {
                    // Rename selected item
                }

                if (ImGui::MenuItem(ICON_MS_DELETE " Delete")) {
                    // Delete selected item
                }
            }
            
            ImGui::EndPopup();
        }
    }

} // namespace scummredux
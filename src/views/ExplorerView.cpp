#include "ExplorerView.h"
#include "../res/icons/MaterialSymbols.h"
#include <functional>
#include <iostream>

namespace scummredux {

    ExplorerView::ExplorerView() : View("Explorer") {
        std::cout << "ExplorerView minimal constructor" << std::endl;

        // Initialize with minimal data to avoid crashes
        m_projectFiles = {};
        m_selectedFile = "";
        m_showHiddenFiles = false;
    }

    void ExplorerView::draw() {
        std::cout << "ExplorerView::draw() start" << std::endl;

        try {
            if (ImGui::Begin(getWindowName().c_str(), &getWindowOpenState())) {
                std::cout << "ExplorerView window created successfully" << std::endl;

                // MINIMAL VERSION - just text for now to avoid crashes
                ImGui::Text(ICON_MS_FOLDER " SCUMM Redux Project");
                ImGui::Separator();

                // Simple buttons without functionality
                if (ImGui::Button(ICON_MS_REFRESH "##refresh")) {
                    std::cout << "Refresh clicked" << std::endl;
                }
                ImGui::SameLine();

                if (ImGui::Button(ICON_MS_CREATE_NEW_FOLDER "##newfolder")) {
                    std::cout << "New folder clicked" << std::endl;
                }
                ImGui::SameLine();

                if (ImGui::Button(ICON_MS_NOTE_ADD "##newfile")) {
                    std::cout << "New file clicked" << std::endl;
                }
                ImGui::SameLine();

                ImGui::Checkbox("Hidden", &m_showHiddenFiles);

                ImGui::Separator();

                // Simple file list without complex tree
                ImGui::Text("Project Files:");
                ImGui::BulletText("src/main.cpp");
                ImGui::BulletText("src/core/Application.cpp");
                ImGui::BulletText("CMakeLists.txt");

                std::cout << "ExplorerView content drawn successfully" << std::endl;
            } else {
                std::cout << "ExplorerView window failed to create" << std::endl;
            }
            ImGui::End();
        } catch (const std::exception& e) {
            std::cout << "Exception in ExplorerView::draw(): " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception in ExplorerView::draw()" << std::endl;
        }

        std::cout << "ExplorerView::draw() end" << std::endl;
    }

    void ExplorerView::drawFileTree() {
        // Minimal implementation - just text
        ImGui::Text("File tree placeholder");
    }

    void ExplorerView::drawProjectFiles() {
        // Minimal implementation
        ImGui::Text("Project files placeholder");
    }

    void ExplorerView::drawContextMenu() {
        // DISABLED - might cause crashes
        // Context menus can sometimes cause ID conflicts
    }

} // namespace scummredux
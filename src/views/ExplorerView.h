#pragma once

#include "View.h"
#include <vector>
#include <string>

namespace scummredux {

    class ExplorerView : public View {
    public:
        ExplorerView();
        ~ExplorerView() override = default;

        void draw() override;

    private:
        void drawFileTree();
        void drawProjectFiles();
        void drawContextMenu();

        // File tree state
        struct FileNode {
            std::string name;
            std::string path;
            bool isDirectory;
            bool isExpanded;
            std::vector<FileNode> children;
        };

        std::vector<FileNode> m_projectFiles;
        std::string m_selectedFile;
        bool m_showHiddenFiles = false;
    };

} // namespace scummredux
#include "Utils.h"
#include <algorithm>
#include <sstream>

namespace scummredux::utils {

    std::string limitStringLength(const std::string& str, size_t maxLength) {
        if (str.length() <= maxLength) {
            return str;
        }

        if (maxLength <= 3) {
            return "...";
        }

        return str.substr(0, maxLength - 3) + "...";
    }

    std::string replaceStrings(std::string str, const std::string& from, const std::string& to) {
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
        return str;
    }

    bool beginMenuEx(const char* label, const char* icon, bool enabled) {
        std::string menuLabel;
        if (icon && strlen(icon) > 0) {
            menuLabel = std::string(icon) + " " + label;
        } else {
            menuLabel = label;
        }

        if (!enabled) {
            ImGui::BeginDisabled();
        }

        bool result = ImGui::BeginMenu(menuLabel.c_str(), enabled);

        if (!enabled) {
            ImGui::EndDisabled();
        }

        return result;
    }

    void endMenu() {
        ImGui::EndMenu();
    }

    bool menuItemEx(const char* label, const char* icon, const char* shortcut, bool selected, bool enabled) {
        std::string itemLabel;
        if (icon && strlen(icon) > 0) {
            itemLabel = std::string(icon) + " " + label;
        } else {
            itemLabel = label;
        }

        return ImGui::MenuItem(itemLabel.c_str(), shortcut, selected, enabled);
    }

    void menuSeparator() {
        ImGui::Separator();
    }

    // Window utilities - these would be implemented with actual window management
    static ImVec2 s_mainWindowSize(1280, 720);
    static ImVec2 s_mainWindowPosition(100, 100);

    ImVec2 getMainWindowSize() {
        return s_mainWindowSize;
    }

    ImVec2 getMainWindowPosition() {
        return s_mainWindowPosition;
    }

    void setMainWindowSize(int width, int height) {
        s_mainWindowSize = ImVec2((float)width, (float)height);
    }

    void setMainWindowPosition(int x, int y) {
        s_mainWindowPosition = ImVec2((float)x, (float)y);
    }

    ImU32 getColorU32(ImGuiCol idx, float alpha) {
        ImVec4 color = ImGui::GetStyleColorVec4(idx);
        color.w *= alpha;
        return ImGui::ColorConvertFloat4ToU32(color);
    }

    ImVec4 getColorVec4(ImGuiCol idx) {
        return ImGui::GetStyleColorVec4(idx);
    }

    // Font management - placeholder implementation
    void pushFont(const char* fontName, float size) {
        // TODO: Implement font management with StyleManager
        // For now, just use default font
    }

    void popFont() {
        // TODO: Implement font popping
    }

} // namespace scummredux::utils
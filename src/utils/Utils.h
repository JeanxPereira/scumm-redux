#pragma once

#include <string>
#include <imgui.h>

namespace scummredux::utils {

    // Scaling helpers
    constexpr float operator""_scaled(long double value) {
        return float(value); // TODO: Implement proper DPI scaling
    }

    constexpr float operator""_scaled(unsigned long long value) {
        return float(value); // TODO: Implement proper DPI scaling
    }

    // String utilities
    std::string limitStringLength(const std::string& str, size_t maxLength);
    std::string replaceStrings(std::string str, const std::string& from, const std::string& to);

    // ImGui utilities
    bool beginMenuEx(const char* label, const char* icon = nullptr, bool enabled = true);
    void endMenu();
    bool menuItemEx(const char* label, const char* icon = nullptr, const char* shortcut = nullptr, bool selected = false, bool enabled = true);
    void menuSeparator();

    // Window utilities
    ImVec2 getMainWindowSize();
    ImVec2 getMainWindowPosition();
    void setMainWindowSize(int width, int height);
    void setMainWindowPosition(int x, int y);

    // Color utilities
    ImU32 getColorU32(ImGuiCol idx, float alpha = 1.0f);
    ImVec4 getColorVec4(ImGuiCol idx);

    // Layout utilities
    void pushFont(const char* fontName, float size = 0.0f);
    void popFont();

} // namespace scummredux::utils
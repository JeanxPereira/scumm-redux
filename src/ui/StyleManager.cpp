#include "StyleManager.h"
#include "../core/Settings.h"
#include "../utils/Events.hpp"
#include <iostream>

namespace scummredux {

    StyleManager& StyleManager::getInstance() {
        static StyleManager instance;
        return instance;
    }

    void StyleManager::setTheme(const std::string& themeName) {
        m_currentTheme = themeName;
        applyCurrentTheme();

        // Save to settings
        auto& settings = Settings::getInstance();
        settings.set(Settings::UI::DARK_THEME, themeName == "Dark");

        EventThemeChanged::post({themeName});
    }

    void StyleManager::applyCurrentTheme() {
        if (m_currentTheme == "Dark") {
            setupDarkTheme();
        } else if (m_currentTheme == "Light") {
            setupLightTheme();
        } else if (m_currentTheme == "ImHex") {
            setupImHexTheme();
        } else {
            setupDarkTheme(); // Default fallback
        }

        applyCustomizations();
    }

    void StyleManager::setupDarkTheme() {
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();

        // Enhanced dark theme colors
        auto& colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.16f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.21f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.93f, 0.89f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.51f, 0.53f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.18f, 0.21f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16f, 0.16f, 0.19f, 1.00f);
        colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
        colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
        colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
        colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
        colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    void StyleManager::setupLightTheme() {
        ImGui::StyleColorsLight();
        // Additional light theme customizations can be added here
    }

    void StyleManager::setupImHexTheme() {
        // ImHex-style theme (similar to the reference code)
        setupDarkTheme(); // Start with dark base

        ImGuiStyle& style = ImGui::GetStyle();
        auto& colors = style.Colors;

        // ImHex specific colors
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.30f, 0.33f, 1.00f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
        colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.15f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.18f, 0.21f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.21f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.23f, 0.23f, 0.26f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.27f, 0.27f, 0.30f, 1.00f);
    }

    void StyleManager::setupFonts() {
        setupDefaultFonts();

        // Load custom fonts from settings
        auto& settings = Settings::getInstance();
        float fontSize = settings.get<float>(Settings::UI::FONT_SIZE, 14.0f);

        // Apply font size
        setFontSize(fontSize);
    }

    void StyleManager::setupDefaultFonts() {
        // This would load fonts from embedded resources
        // For now, we'll use the default ImGui font

        ImGuiIO& io = ImGui::GetIO();

        // Add default font
        ImFontConfig config;
        config.SizePixels = m_fontSize;
        config.OversampleH = 2;
        config.OversampleV = 1;
        config.PixelSnapH = true;

        ImFont* defaultFont = io.Fonts->AddFontDefault(&config);

        // Store font info
        FontInfo fontInfo;
        fontInfo.font = defaultFont;
        fontInfo.size = m_fontSize;
        fontInfo.path = "default";
        m_fonts[m_defaultFontName] = fontInfo;

        // TODO: Load Geist fonts and Material Symbols from embedded resources
        // loadFont("GeistRegular", "res/fonts/Geist-Regular.ttf", 14.0f);
        // loadFont("GeistBold", "res/fonts/Geist-Bold.ttf", 14.0f);
        // loadFont("MaterialSymbols", "res/fonts/MaterialSymbols.ttf", 16.0f);
    }

    void StyleManager::loadFont(const std::string& name, const std::string& path, float size) {
        ImGuiIO& io = ImGui::GetIO();

        ImFont* font = io.Fonts->AddFontFromFileTTF(path.c_str(), size);
        if (font) {
            FontInfo fontInfo;
            fontInfo.font = font;
            fontInfo.size = size;
            fontInfo.path = path;
            m_fonts[name] = fontInfo;

            std::cout << "Loaded font: " << name << " (" << path << ")" << std::endl;
        } else {
            std::cerr << "Failed to load font: " << path << std::endl;
        }
    }

    ImFont* StyleManager::getFont(const std::string& name) {
        auto it = m_fonts.find(name);
        if (it != m_fonts.end()) {
            return it->second.font;
        }

        // Return default font if not found
        auto defaultIt = m_fonts.find(m_defaultFontName);
        if (defaultIt != m_fonts.end()) {
            return defaultIt->second.font;
        }

        // Fallback to ImGui's default font
        ImGuiIO& io = ImGui::GetIO();
        return io.Fonts->Fonts.empty() ? nullptr : io.Fonts->Fonts[0];
    }

    void StyleManager::setDefaultFont(const std::string& name) {
        if (m_fonts.find(name) != m_fonts.end()) {
            m_defaultFontName = name;
        }
    }

    void StyleManager::applyCustomizations() {
        ImGuiStyle& style = ImGui::GetStyle();

        // Apply custom values from settings
        auto& settings = Settings::getInstance();

        style.WindowRounding = settings.get<float>(Settings::UI::WINDOW_ROUNDING, m_windowRounding);
        style.FrameRounding = settings.get<float>(Settings::UI::FRAME_ROUNDING, m_frameRounding);
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;
        style.WindowPadding = ImVec2(8, 8);
        style.ItemSpacing = ImVec2(8, 4);
        style.ItemInnerSpacing = ImVec2(4, 4);
        style.FramePadding = ImVec2(6, 3);

        // Apply accent color
        ImVec4 accentColor = settings.get<ImVec4>(Settings::UI::ACCENT_COLOR, m_accentColor);
        setAccentColor(accentColor);
    }

    void StyleManager::setAccentColor(const ImVec4& color) {
        m_accentColor = color;

        // Apply accent color to relevant UI elements
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_CheckMark] = color;
        style.Colors[ImGuiCol_SliderGrab] = color;
        style.Colors[ImGuiCol_SliderGrabActive] = colorWithAlpha(color, 0.8f);
        style.Colors[ImGuiCol_ResizeGrip] = colorWithAlpha(color, 0.2f);
        style.Colors[ImGuiCol_ResizeGripHovered] = colorWithAlpha(color, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] = colorWithAlpha(color, 0.95f);
        style.Colors[ImGuiCol_DockingPreview] = colorWithAlpha(color, 0.7f);

        // Save to settings
        auto& settings = Settings::getInstance();
        settings.set(Settings::UI::ACCENT_COLOR, color);
    }

    void StyleManager::setWindowRounding(float rounding) {
        m_windowRounding = rounding;
        ImGui::GetStyle().WindowRounding = rounding;

        auto& settings = Settings::getInstance();
        settings.set(Settings::UI::WINDOW_ROUNDING, rounding);
    }

    void StyleManager::setFrameRounding(float rounding) {
        m_frameRounding = rounding;
        ImGui::GetStyle().FrameRounding = rounding;

        auto& settings = Settings::getInstance();
        settings.set(Settings::UI::FRAME_ROUNDING, rounding);
    }

    void StyleManager::setFontSize(float size) {
        m_fontSize = size;

        auto& settings = Settings::getInstance();
        settings.set(Settings::UI::FONT_SIZE, size);

        // TODO: Rebuild font atlas with new size
    }

    ImVec4 StyleManager::getAccentColor() const {
        return m_accentColor;
    }

    // Static utility functions
    ImU32 StyleManager::colorWithAlpha(ImU32 color, float alpha) {
        return (color & 0x00FFFFFF) | (IM_COL32_A_MASK & ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, alpha)));
    }

    ImVec4 StyleManager::colorWithAlpha(const ImVec4& color, float alpha) {
        return ImVec4(color.x, color.y, color.z, alpha);
    }

} // namespace scummredux
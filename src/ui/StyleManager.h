#pragma once

#include <imgui.h>
#include <string>
#include <unordered_map>

namespace scummredux {

    class StyleManager {
    public:
        static StyleManager& getInstance();

        // Theme management
        void setTheme(const std::string& themeName);
        void applyCurrentTheme();
        void setupFonts();
        
        // Predefined themes
        void setupDarkTheme();
        void setupLightTheme();
        void setupImHexTheme(); // Similar to the ImHex style
        
        // Font management
        struct FontInfo {
            ImFont* font = nullptr;
            float size = 14.0f;
            std::string path;
        };
        
        void loadFont(const std::string& name, const std::string& path, float size);
        ImFont* getFont(const std::string& name);
        void setDefaultFont(const std::string& name);
        
        // Style customization
        void setAccentColor(const ImVec4& color);
        void setWindowRounding(float rounding);
        void setFrameRounding(float rounding);
        void setFontSize(float size);
        
        // Getters
        std::string getCurrentTheme() const { return m_currentTheme; }
        ImVec4 getAccentColor() const;
        
        // Style helpers
        static ImU32 colorWithAlpha(ImU32 color, float alpha);
        static ImVec4 colorWithAlpha(const ImVec4& color, float alpha);
        
    private:
        StyleManager() = default;
        
        void applyCustomizations();
        void setupDefaultFonts();
        
        std::string m_currentTheme = "Dark";
        std::unordered_map<std::string, FontInfo> m_fonts;
        std::string m_defaultFontName = "GeistRegular";
        
        // Style customizations
        ImVec4 m_accentColor = ImVec4(0.43f, 0.43f, 0.50f, 1.0f);
        float m_windowRounding = 6.0f;
        float m_frameRounding = 3.0f;
        float m_fontSize = 14.0f;
    };

} // namespace scummredux
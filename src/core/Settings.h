#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <imgui.h>

namespace scummredux {

    class Settings {
    public:
        using Value = std::variant<bool, int, float, std::string, ImVec4>;

        static Settings& getInstance();

        // Getters
        template<typename T>
        T get(const std::string& key, const T& defaultValue = T{}) const {
            auto it = m_settings.find(key);
            if (it != m_settings.end()) {
                if (std::holds_alternative<T>(it->second)) {
                    return std::get<T>(it->second);
                }
            }
            return defaultValue;
        }

        // Setters
        template<typename T>
        void set(const std::string& key, const T& value) {
            m_settings[key] = value;
            markDirty();
        }

        // File operations
        void load(const std::string& filename = "settings.ini");
        void save(const std::string& filename = "settings.ini");

        // Application settings
        struct App {
            static constexpr const char* WINDOW_WIDTH = "app.window.width";
            static constexpr const char* WINDOW_HEIGHT = "app.window.height";
            static constexpr const char* WINDOW_MAXIMIZED = "app.window.maximized";
            static constexpr const char* WINDOW_POS_X = "app.window.pos_x";
            static constexpr const char* WINDOW_POS_Y = "app.window.pos_y";
            static constexpr const char* SHOW_DOCKSPACE = "app.ui.show_dockspace";
            static constexpr const char* APP_NAME = "app.name";
        };

        // UI settings
        struct UI {
            static constexpr const char* DARK_THEME = "ui.dark_theme";
            static constexpr const char* ACCENT_COLOR = "ui.accent_color";
            static constexpr const char* FONT_SIZE = "ui.font_size";
            static constexpr const char* SHOW_TITLE_BAR = "ui.show_title_bar";
            static constexpr const char* WINDOW_ROUNDING = "ui.window_rounding";
            static constexpr const char* FRAME_ROUNDING = "ui.frame_rounding";
        };

        // Editor settings
        struct Editor {
            static constexpr const char* TAB_SIZE = "editor.tab_size";
            static constexpr const char* WORD_WRAP = "editor.word_wrap";
            static constexpr const char* SHOW_LINE_NUMBERS = "editor.show_line_numbers";
            static constexpr const char* AUTO_INDENT = "editor.auto_indent";
        };

    private:
        Settings() = default;
        void markDirty() { m_dirty = true; }
        void setDefaults();

        std::unordered_map<std::string, Value> m_settings;
        bool m_dirty = false;
    };

} // namespace scummredux
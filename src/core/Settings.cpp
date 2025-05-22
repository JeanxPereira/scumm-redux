#include "Settings.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace scummredux {

    Settings& Settings::getInstance() {
        static Settings instance;
        return instance;
    }

    void Settings::setDefaults() {
        // Application defaults
        set(App::WINDOW_WIDTH, 1280);
        set(App::WINDOW_HEIGHT, 720);
        set(App::WINDOW_MAXIMIZED, false);
        set(App::WINDOW_POS_X, -1);
        set(App::WINDOW_POS_Y, -1);
        set(App::SHOW_DOCKSPACE, true);
        set(App::APP_NAME, std::string("SCUMM Redux"));

        // UI defaults
        set(UI::DARK_THEME, true);
        set(UI::ACCENT_COLOR, ImVec4(0.43f, 0.43f, 0.50f, 1.0f));
        set(UI::FONT_SIZE, 14.0f);
        set(UI::SHOW_TITLE_BAR, true);
        set(UI::WINDOW_ROUNDING, 6.0f);
        set(UI::FRAME_ROUNDING, 3.0f);

        // Editor defaults
        set(Editor::TAB_SIZE, 4);
        set(Editor::WORD_WRAP, false);
        set(Editor::SHOW_LINE_NUMBERS, true);
        set(Editor::AUTO_INDENT, true);
    }

    void Settings::load(const std::string& filename) {
        setDefaults();

        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Settings file not found, using defaults" << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;

            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string valueStr = line.substr(pos + 1);

            // Remove whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            valueStr.erase(0, valueStr.find_first_not_of(" \t"));
            valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

            // Parse value based on type
            if (valueStr == "true" || valueStr == "false") {
                set(key, valueStr == "true");
            } else if (valueStr.find('.') != std::string::npos) {
                try {
                    float value = std::stof(valueStr);
                    set(key, value);
                } catch (...) {
                    set(key, valueStr);
                }
            } else {
                try {
                    int value = std::stoi(valueStr);
                    set(key, value);
                } catch (...) {
                    set(key, valueStr);
                }
            }
        }

        m_dirty = false;
    }

    void Settings::save(const std::string& filename) {
        if (!m_dirty) return;

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to save settings to " << filename << std::endl;
            return;
        }

        file << "# SCUMM Redux Settings\n\n";

        for (const auto& [key, value] : m_settings) {
            file << key << " = ";
            
            std::visit([&](const auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, bool>) {
                    file << (v ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << v;
                } else if constexpr (std::is_same_v<T, ImVec4>) {
                    file << v.x << "," << v.y << "," << v.z << "," << v.w;
                } else {
                    file << v;
                }
            }, value);
            
            file << "\n";
        }

        m_dirty = false;
    }

} // namespace scummredux
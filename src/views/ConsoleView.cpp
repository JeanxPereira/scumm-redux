#include "ConsoleView.h"
#include "../res/icons/MaterialSymbols.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>

namespace scummredux {

    // Static members
    std::deque<LogEntry> ConsoleView::s_logEntries;
    ConsoleView* ConsoleView::s_instance = nullptr;

    LogEntry::LogEntry(const std::string& msg, LogLevel lvl)
        : message(msg), level(lvl) {

        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        timestamp = ss.str();
    }

    ConsoleView::ConsoleView() : View("Console") {
        s_instance = this;
        std::cout << "ConsoleView constructor called" << std::endl;

        // Welcome messages
        log("SCUMM Redux Console initialized", LogLevel::Success);
        log("Type 'help' for available commands", LogLevel::Info);
    }

    void ConsoleView::draw() {
        std::cout << "ConsoleView::draw() start" << std::endl;

        try {
            if (ImGui::Begin(getWindowName().c_str(), &getWindowOpenState())) {
                std::cout << "ConsoleView window created successfully" << std::endl;

                drawToolbar();
                drawLogEntries();
                drawCommandInput();

                std::cout << "ConsoleView content drawn successfully" << std::endl;
            } else {
                std::cout << "ConsoleView window failed to create" << std::endl;
            }
            ImGui::End();
        } catch (const std::exception& e) {
            std::cout << "Exception in ConsoleView::draw(): " << e.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception in ConsoleView::draw()" << std::endl;
        }

        std::cout << "ConsoleView::draw() end" << std::endl;
    }

    void ConsoleView::drawToolbar() {
        std::cout << "ConsoleView::drawToolbar() start" << std::endl;

        // Clear button
        if (ImGui::Button(ICON_MS_CLEAR_ALL "##clear")) {
            clear();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Clear Console");
        }

        ImGui::SameLine();

        // Auto-scroll toggle
        if (ImGui::Checkbox("Auto-scroll", &m_autoScroll)) {
            if (m_autoScroll) {
                m_scrollToBottom = true;
            }
        }

        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        // Search
        ImGui::PushItemWidth(200);
        if (ImGui::InputTextWithHint("##search", ICON_MS_SEARCH " Search...",
                                    m_searchBuffer, sizeof(m_searchBuffer))) {
            // Search functionality would be implemented here
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();
        ImGui::Checkbox("Regex", &m_useRegex);

        // Filters
        drawFilters();
        ImGui::Separator();

        std::cout << "ConsoleView::drawToolbar() end" << std::endl;
    }

    void ConsoleView::drawFilters() {
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        // Log level filters
        ImGui::Text("Filters:");
        ImGui::SameLine();

        if (ImGui::Checkbox("Info", &m_showInfo)) {}
        ImGui::SameLine();

        if (ImGui::Checkbox("Warning", &m_showWarning)) {}
        ImGui::SameLine();

        if (ImGui::Checkbox("Error", &m_showError)) {}
        ImGui::SameLine();

        if (ImGui::Checkbox("Debug", &m_showDebug)) {}
        ImGui::SameLine();

        if (ImGui::Checkbox("Success", &m_showSuccess)) {}
    }

    void ConsoleView::drawLogEntries() {
        std::cout << "ConsoleView::drawLogEntries() start" << std::endl;

        // Calculate available space for log entries
        ImVec2 logRegionSize = ImGui::GetContentRegionAvail();

        // DEFENSIVE: Ensure we have enough space before subtracting
        const float reservedSpace = 30.0f; // Space for command input
        if (logRegionSize.y > reservedSpace + 10.0f) {
            logRegionSize.y -= reservedSpace;
        } else {
            // If window is too small, use minimum viable size
            logRegionSize.y = std::max(50.0f, logRegionSize.y * 0.7f);
        }

        // DEFENSIVE: Ensure we have valid minimum size
        logRegionSize.x = std::max(100.0f, logRegionSize.x);
        logRegionSize.y = std::max(50.0f, logRegionSize.y);

        std::cout << "Using log region size: " << logRegionSize.x << "x" << logRegionSize.y << std::endl;

        if (ImGui::BeginChild("LogRegion", logRegionSize, true, ImGuiWindowFlags_HorizontalScrollbar)) {
            std::cout << "LogRegion child created successfully" << std::endl;

            // Filter and display log entries
            bool hasSearchFilter = strlen(m_searchBuffer) > 0;
            std::regex searchRegex;
            bool regexValid = true;

            if (hasSearchFilter && m_useRegex) {
                try {
                    searchRegex = std::regex(m_searchBuffer, std::regex_constants::icase);
                } catch (const std::regex_error&) {
                    regexValid = false;
                }
            }

            for (const auto& entry : s_logEntries) {
                // Apply level filters
                bool showEntry = false;
                switch (entry.level) {
                    case LogLevel::Info: showEntry = m_showInfo; break;
                    case LogLevel::Warning: showEntry = m_showWarning; break;
                    case LogLevel::Error: showEntry = m_showError; break;
                    case LogLevel::Debug: showEntry = m_showDebug; break;
                    case LogLevel::Success: showEntry = m_showSuccess; break;
                }

                if (!showEntry) continue;

                // Apply search filter
                if (hasSearchFilter) {
                    if (m_useRegex && regexValid) {
                        if (!std::regex_search(entry.message, searchRegex)) continue;
                    } else {
                        std::string lowerMessage = entry.message;
                        std::string lowerSearch = m_searchBuffer;
                        std::transform(lowerMessage.begin(), lowerMessage.end(), lowerMessage.begin(), ::tolower);
                        std::transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
                        if (lowerMessage.find(lowerSearch) == std::string::npos) continue;
                    }
                }

                // Draw log entry
                ImVec4 color = getLogLevelColor(entry.level);
                const char* icon = getLogLevelIcon(entry.level);

                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::Text("[%s] %s %s", entry.timestamp.c_str(), icon, entry.message.c_str());
                ImGui::PopStyleColor();

                // DISABLED CONTEXT MENU - might be causing the crash
                /*
                // Context menu for copying
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Copy")) {
                        ImGui::SetClipboardText(entry.message.c_str());
                    }
                    ImGui::EndPopup();
                }
                */
            }

            // Auto-scroll to bottom
            if (m_scrollToBottom) {
                ImGui::SetScrollHereY(1.0f);
                m_scrollToBottom = false;
            }
        } else {
            std::cout << "ERROR: Failed to create LogRegion child window!" << std::endl;
        }
        ImGui::EndChild();

        std::cout << "ConsoleView::drawLogEntries() end" << std::endl;
    }

    void ConsoleView::drawCommandInput() {
        std::cout << "ConsoleView::drawCommandInput() start" << std::endl;

        ImGui::Separator();

        // Command input
        ImGui::Text(ICON_MS_CHEVRON_RIGHT);
        ImGui::SameLine();

        ImGui::PushItemWidth(-1);
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue |
                                   ImGuiInputTextFlags_CallbackHistory;

        if (ImGui::InputTextWithHint("##command", "Enter command...",
                                    m_commandBuffer, sizeof(m_commandBuffer), flags,
                                    ConsoleView::handleInputCallback, this)) {
            executeCommand(m_commandBuffer);
            m_commandBuffer[0] = '\0';
        }
        ImGui::PopItemWidth();

        // Keep input focused
        if (ImGui::IsItemHovered() || (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive())) {
            ImGui::SetKeyboardFocusHere(-1);
        }

        std::cout << "ConsoleView::drawCommandInput() end" << std::endl;
    }

    // Static callback for ImGui
    int ConsoleView::handleInputCallback(ImGuiInputTextCallbackData* data) {
        ConsoleView* console = static_cast<ConsoleView*>(data->UserData);
        return console->handleInputCallbackImpl(data);
    }

    int ConsoleView::handleInputCallbackImpl(ImGuiInputTextCallbackData* data) {
        switch (data->EventFlag) {
            case ImGuiInputTextFlags_CallbackHistory: {
                const int prevHistoryPos = m_historyIndex;
                if (data->EventKey == ImGuiKey_UpArrow) {
                    if (m_historyIndex == -1) m_historyIndex = m_commandHistory.size() - 1;
                    else if (m_historyIndex > 0) m_historyIndex--;
                } else if (data->EventKey == ImGuiKey_DownArrow) {
                    if (m_historyIndex != -1) {
                        if (++m_historyIndex >= static_cast<int>(m_commandHistory.size())) {
                            m_historyIndex = -1;
                        }
                    }
                }

                if (prevHistoryPos != m_historyIndex) {
                    const char* historyStr = (m_historyIndex >= 0) ?
                        m_commandHistory[m_historyIndex].c_str() : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, historyStr);
                }
                break;
            }
        }
        return 0;
    }

    void ConsoleView::executeCommand(const std::string& command) {
        if (command.empty()) return;

        // Add to history
        if (m_commandHistory.empty() || m_commandHistory.back() != command) {
            m_commandHistory.push_back(command);
            if (m_commandHistory.size() > MAX_HISTORY) {
                m_commandHistory.erase(m_commandHistory.begin());
            }
        }
        m_historyIndex = -1;

        // Echo command
        log("> " + command, LogLevel::Info);

        // Process command
        processCommand(command);
    }

    void ConsoleView::processCommand(const std::string& command) {
        std::string cmd = command;
        std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

        if (cmd == "help") {
            log("Available commands:", LogLevel::Info);
            log("  help       - Show this help message", LogLevel::Info);
            log("  clear      - Clear console output", LogLevel::Info);
            log("  version    - Show application version", LogLevel::Info);
            log("  test       - Run test command", LogLevel::Info);
            log("  settings   - Show current settings", LogLevel::Info);
        } else if (cmd == "clear") {
            clear();
        } else if (cmd == "version") {
            log("SCUMM Redux v1.0.0", LogLevel::Success);
            log("Built with ImGui and GLFW", LogLevel::Info);
        } else if (cmd == "test") {
            log("Testing all log levels:", LogLevel::Info);
            info("This is an info message");
            warning("This is a warning message");
            error("This is an error message");
            debug("This is a debug message");
            success("This is a success message");
        } else if (cmd == "settings") {
            log("Current application settings:", LogLevel::Info);
            // TODO: Display current settings
        } else {
            error("Unknown command: " + command);
            log("Type 'help' for available commands", LogLevel::Info);
        }
    }

    ImVec4 ConsoleView::getLogLevelColor(LogLevel level) const {
        switch (level) {
            case LogLevel::Info:    return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
            case LogLevel::Warning: return ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
            case LogLevel::Error:   return ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
            case LogLevel::Debug:   return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
            case LogLevel::Success: return ImVec4(0.3f, 1.0f, 0.3f, 1.0f);
            default:                return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }

    const char* ConsoleView::getLogLevelIcon(LogLevel level) const {
        switch (level) {
            case LogLevel::Info:    return ICON_MS_INFO;
            case LogLevel::Warning: return ICON_MS_WARNING;
            case LogLevel::Error:   return ICON_MS_ERROR;
            case LogLevel::Debug:   return ICON_MS_BUG_REPORT;
            case LogLevel::Success: return ICON_MS_CHECK_CIRCLE;
            default:                return ICON_MS_CIRCLE;
        }
    }

    void ConsoleView::clear() {
        s_logEntries.clear();
        log("Console cleared", LogLevel::Info);
    }

    void ConsoleView::scrollToBottom() {
        m_scrollToBottom = true;
    }

    // Static logging functions
    void ConsoleView::log(const std::string& message, LogLevel level) {
        s_logEntries.emplace_back(message, level);

        // Also log to cout for debugging
        std::cout << "[LOG " << static_cast<int>(level) << "] " << message << std::endl;
        
        // Limit number of entries
        if (s_logEntries.size() > MAX_LOG_ENTRIES) {
            s_logEntries.pop_front();
        }
        
        // Auto-scroll if enabled
        if (s_instance && s_instance->m_autoScroll) {
            s_instance->m_scrollToBottom = true;
        }
    }

    void ConsoleView::info(const std::string& message) {
        log(message, LogLevel::Info);
    }

    void ConsoleView::warning(const std::string& message) {
        log(message, LogLevel::Warning);
    }

    void ConsoleView::error(const std::string& message) {
        log(message, LogLevel::Error);
    }

    void ConsoleView::debug(const std::string& message) {
        log(message, LogLevel::Debug);
    }

    void ConsoleView::success(const std::string& message) {
        log(message, LogLevel::Success);
    }

} // namespace scummredux
#pragma once

#include "View.h"
#include <string>
#include <vector>
#include <deque>

namespace scummredux {

    enum class LogLevel {
        Info,
        Warning,
        Error,
        Debug,
        Success
    };

    struct LogEntry {
        std::string message;
        LogLevel level;
        std::string timestamp;
        
        LogEntry(const std::string& msg, LogLevel lvl);
    };

    class ConsoleView : public View {
    public:
        ConsoleView();
        ~ConsoleView() override = default;

        void draw() override;

        // Logging functions
        static void log(const std::string& message, LogLevel level = LogLevel::Info);
        static void info(const std::string& message);
        static void warning(const std::string& message);
        static void error(const std::string& message);
        static void debug(const std::string& message);
        static void success(const std::string& message);

        // Console management
        void clear();
        void executeCommand(const std::string& command);

    private:
        void drawToolbar();
        void drawLogEntries();
        void drawCommandInput();
        void drawFilters();

        // Helper functions
        ImVec4 getLogLevelColor(LogLevel level) const;
        const char* getLogLevelIcon(LogLevel level) const;
        void scrollToBottom();
        void processCommand(const std::string& command);

        // Static callback for ImGui
        static int handleInputCallback(ImGuiInputTextCallbackData* data);

    private:
        int handleInputCallbackImpl(ImGuiInputTextCallbackData* data);

        // Log storage
        static std::deque<LogEntry> s_logEntries;
        static ConsoleView* s_instance;
        static constexpr size_t MAX_LOG_ENTRIES = 1000;

        // UI State
        char m_commandBuffer[512] = "";
        bool m_autoScroll = true;
        bool m_scrollToBottom = false;
        
        // Filters
        bool m_showInfo = true;
        bool m_showWarning = true;
        bool m_showError = true;
        bool m_showDebug = false;
        bool m_showSuccess = true;
        
        // Search
        char m_searchBuffer[256] = "";
        bool m_useRegex = false;
        
        // Command history
        std::vector<std::string> m_commandHistory;
        int m_historyIndex = -1;
        static constexpr size_t MAX_HISTORY = 50;
    };

} // namespace scummredux
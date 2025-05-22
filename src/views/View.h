#pragma once

#include <string>
#include <imgui.h>

namespace scummredux {

    // Base class for all views/panels in the application
    class View {
    public:
        explicit View(std::string name);
        virtual ~View() = default;

        // Main draw function - must be implemented by derived classes
        virtual void draw() = 0;

        // Optional: Draw content that should always be visible (even when view is closed)
        virtual void drawAlwaysVisibleContent() {}

        // View management
        const std::string& getName() const { return m_name; }
        std::string getWindowName() const;

        // Window state
        bool& getWindowOpenState() { return m_isOpen; }
        bool isOpen() const { return m_isOpen; }
        void setOpen(bool open) { m_isOpen = open; }

        // Focus management
        bool isFocused() const { return m_isFocused; }
        void setFocused(bool focused) { m_isFocused = focused; }

        // View properties
        bool hasViewMenuItemEntry() const { return m_hasMenuEntry; }
        void setHasMenuEntry(bool hasEntry) { m_hasMenuEntry = hasEntry; }

        bool shouldProcess() const { return m_shouldProcess; }
        void setShouldProcess(bool process) { m_shouldProcess = process; }

        // Window state tracking
        bool didWindowJustOpen();
        void trackViewOpenState();

        // Helper for creating unique window names
        static std::string toWindowName(const std::string& viewName);

    protected:
        // Helper functions for derived classes
        void beginChild(const char* id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);
        void endChild();

        // Styling helpers
        void pushStyleColor(ImGuiCol idx, const ImVec4& color);
        void popStyleColor(int count = 1);
        void pushStyleVar(ImGuiStyleVar idx, float val);
        void pushStyleVar(ImGuiStyleVar idx, const ImVec2& val);
        void popStyleVar(int count = 1);

        // View state
        std::string m_name;
        bool m_isOpen = true;
        bool m_isFocused = false;
        bool m_hasMenuEntry = true;
        bool m_shouldProcess = true;

    private:
        // Window state tracking
        bool m_previousOpenState = false;
        bool m_windowJustOpened = false;
    };

} // namespace scummredux
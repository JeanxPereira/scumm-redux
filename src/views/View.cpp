#include "View.h"
#include "../utils/Events.hpp"

namespace scummredux {

    View::View(std::string name) 
        : m_name(std::move(name)) {
    }

    std::string View::getWindowName() const {
        return toWindowName(m_name);
    }

    std::string View::toWindowName(const std::string& viewName) {
        return viewName + "##ScummRedux";
    }

    bool View::didWindowJustOpen() {
        return m_windowJustOpened;
    }

    void View::trackViewOpenState() {
        // Check if window state changed
        if (m_isOpen != m_previousOpenState) {
            if (m_isOpen && !m_previousOpenState) {
                // Window just opened
                m_windowJustOpened = true;
                EventViewOpened::post({m_name});
            } else if (!m_isOpen && m_previousOpenState) {
                // Window just closed
                m_windowJustOpened = false;
                EventViewClosed::post({m_name});
            }
        } else {
            m_windowJustOpened = false;
        }

        m_previousOpenState = m_isOpen;
    }

    // Helper functions for derived classes
    void View::beginChild(const char* id, const ImVec2& size, bool border, ImGuiWindowFlags flags) {
        ImGui::BeginChild(id, size, border, flags);
    }

    void View::endChild() {
        ImGui::EndChild();
    }

    void View::pushStyleColor(ImGuiCol idx, const ImVec4& color) {
        ImGui::PushStyleColor(idx, color);
    }

    void View::popStyleColor(int count) {
        ImGui::PopStyleColor(count);
    }

    void View::pushStyleVar(ImGuiStyleVar idx, float val) {
        ImGui::PushStyleVar(idx, val);
    }

    void View::pushStyleVar(ImGuiStyleVar idx, const ImVec2& val) {
        ImGui::PushStyleVar(idx, val);
    }

    void View::popStyleVar(int count) {
        ImGui::PopStyleVar(count);
    }

} // namespace scummredux
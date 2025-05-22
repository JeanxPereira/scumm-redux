#include "ViewManager.h"
#include "../utils/Events.hpp"
#include <algorithm>
#include <iostream>

namespace scummredux {

    ViewManager& ViewManager::getInstance() {
        static ViewManager instance;
        return instance;
    }

    void ViewManager::removeView(const std::string& name) {
        auto it = std::find_if(m_views.begin(), m_views.end(),
            [&name](const auto& view) { return view->getName() == name; });
        
        if (it != m_views.end()) {
            // Remove from name map
            m_viewsByName.erase(name);
            
            // Clear focus if this view was focused
            if (m_focusedView == it->get()) {
                m_focusedView = nullptr;
            }
            
            // Remove from views vector
            m_views.erase(it);
            
            std::cout << "Removed view: " << name << std::endl;
        }
    }

    void ViewManager::removeView(View* view) {
        if (!view) return;
        removeView(view->getName());
    }

    View* ViewManager::getView(const std::string& name) {
        auto it = m_viewsByName.find(name);
        return (it != m_viewsByName.end()) ? it->second : nullptr;
    }

    void ViewManager::drawViews() {
        for (auto& view : m_views) {
            if (!view->shouldProcess()) continue;

            // Set up window class for consistent docking behavior
            ImGuiWindowClass windowClass = {};
            windowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoCloseButton;
            
            // Count open views to determine if we should show tab bar
            const auto openViewCount = std::count_if(m_views.begin(), m_views.end(),
                [](const auto& v) { return v->hasViewMenuItemEntry() && v->shouldProcess(); });
            
            if (openViewCount <= 1) {
                windowClass.DockNodeFlagsOverrideSet |= ImGuiDockNodeFlags_NoTabBar;
            }

            ImGui::SetNextWindowClass(&windowClass);

            // Set background alpha for undocked windows
            auto window = ImGui::FindWindowByName(view->getWindowName().c_str());
            if (window != nullptr && window->DockNode == nullptr) {
                ImGui::SetNextWindowBgAlpha(1.0F);
            }

            // Draw the view
            view->draw();
            view->trackViewOpenState();

            // Handle focus tracking
            if (view->getWindowOpenState()) {
                if (window != nullptr && (window->Flags & ImGuiWindowFlags_Popup) != ImGuiWindowFlags_Popup) {
                    auto windowName = view->getWindowName();
                    ImGui::Begin(windowName.c_str());

                    // Check if window is focused
                    const bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows | ImGuiFocusedFlags_NoPopupHierarchy);
                    view->setFocused(focused);

                    if (focused && m_focusedView != view.get()) {
                        m_focusedView = view.get();
                    }

                    // Handle window just opened
                    if (view->didWindowJustOpen() && !ImGui::IsWindowDocked()) {
                        // Dock the window to main dock space
                        // This would require access to the main dock space ID
                        // ImGui::DockBuilderDockWindow(windowName.c_str(), mainDockSpaceId);
                    }

                    ImGui::End();
                }
            }
        }
    }

    void ViewManager::drawAlwaysVisibleContent() {
        for (auto& view : m_views) {
            view->drawAlwaysVisibleContent();
        }
    }

    void ViewManager::setViewOpen(const std::string& name, bool open) {
        auto* view = getView(name);
        if (view) {
            view->setOpen(open);
        }
    }

    bool ViewManager::isViewOpen(const std::string& name) const {
        auto it = m_viewsByName.find(name);
        return (it != m_viewsByName.end()) ? it->second->isOpen() : false;
    }

    View* ViewManager::getFocusedView() const {
        return m_focusedView;
    }

    void ViewManager::setFocusedView(View* view) {
        if (m_focusedView != view) {
            if (m_focusedView) {
                m_focusedView->setFocused(false);
            }
            
            m_focusedView = view;
            
            if (m_focusedView) {
                m_focusedView->setFocused(true);
            }
        }
    }

    void ViewManager::showView(const std::string& name) {
        setViewOpen(name, true);
    }

    void ViewManager::hideView(const std::string& name) {
        setViewOpen(name, false);
    }

    void ViewManager::toggleView(const std::string& name) {
        auto* view = getView(name);
        if (view) {
            view->setOpen(!view->isOpen());
        }
    }

    void ViewManager::saveLayout(const std::string& layoutName) {
        // TODO: Implement layout saving
        // This would save the current docking configuration and view states
        std::cout << "Saving layout: " << layoutName << std::endl;
    }

    void ViewManager::loadLayout(const std::string& layoutName) {
        // TODO: Implement layout loading
        // This would restore a saved docking configuration and view states
        std::cout << "Loading layout: " << layoutName << std::endl;
    }

    void ViewManager::resetToDefaultLayout() {
        // Reset all views to their default state
        for (auto& view : m_views) {
            view->setOpen(true);
            view->setShouldProcess(true);
        }
        
        // TODO: Reset docking layout to default
        std::cout << "Reset to default layout" << std::endl;
    }

} // namespace scummredux
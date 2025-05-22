#pragma once

#include "../views/View.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace scummredux {

    class ViewManager {
    public:
        static ViewManager& getInstance();

        // View management
        template<typename T, typename... Args>
        T* addView(Args&&... args) {
            static_assert(std::is_base_of_v<View, T>, "T must inherit from View");

            auto view = std::make_unique<T>(std::forward<Args>(args)...);
            T* viewPtr = view.get();

            m_views.push_back(std::move(view));
            m_viewsByName[viewPtr->getName()] = viewPtr;

            return viewPtr;
        }

        void removeView(const std::string& name);
        void removeView(View* view);

        // View access
        View* getView(const std::string& name);
        template<typename T>
        T* getView(const std::string& name) {
            auto* view = getView(name);
            return dynamic_cast<T*>(view);
        }

        // View iteration
        const std::vector<std::unique_ptr<View>>& getViews() const { return m_views; }

        // Rendering
        void drawViews();
        void drawAlwaysVisibleContent();

        // View state management
        void setViewOpen(const std::string& name, bool open);
        bool isViewOpen(const std::string& name) const;

        // Focus management
        View* getFocusedView() const;
        void setFocusedView(View* view);

        // Utility
        void showView(const std::string& name);
        void hideView(const std::string& name);
        void toggleView(const std::string& name);

        // Layout management
        void saveLayout(const std::string& layoutName);
        void loadLayout(const std::string& layoutName);
        void resetToDefaultLayout();

    private:
        ViewManager() = default;

        std::vector<std::unique_ptr<View>> m_views;
        std::unordered_map<std::string, View*> m_viewsByName;
        View* m_focusedView = nullptr;
    };

} // namespace scummredux
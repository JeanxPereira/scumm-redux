#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <memory>

namespace scummredux {

    // Sistema simples de eventos inspirado no ImHex
    template<typename T>
    class Event {
    public:
        using Callback = std::function<void(const T&)>;
        using Handle = size_t;

        static Handle subscribe(Callback callback) {
            auto handle = s_nextHandle++;
            s_callbacks[handle] = std::move(callback);
            return handle;
        }

        static void unsubscribe(Handle handle) {
            s_callbacks.erase(handle);
        }

        static void post(const T& event) {
            for (const auto& [handle, callback] : s_callbacks) {
                callback(event);
            }
        }

    private:
        static inline std::unordered_map<Handle, Callback> s_callbacks;
        static inline Handle s_nextHandle = 1;
    };

    // Eventos específicos do SCUMM Redux
    struct WindowResizeEvent {
        int width, height;
    };

    struct WindowCloseEvent {
        bool shouldClose = true;
    };

    struct WindowMaximizeEvent {
        bool maximized;
    };

    struct ThemeChangedEvent {
        std::string themeName;
    };

    struct ViewOpenedEvent {
        std::string viewName;
    };

    struct ViewClosedEvent {
        std::string viewName;
    };

    struct FrameBeginEvent {};
    struct FrameEndEvent {};

    // Aliases para facilitar o uso
    using EventWindowResize = Event<WindowResizeEvent>;
    using EventWindowClose = Event<WindowCloseEvent>;
    using EventWindowMaximize = Event<WindowMaximizeEvent>;
    using EventThemeChanged = Event<ThemeChangedEvent>;
    using EventViewOpened = Event<ViewOpenedEvent>;
    using EventViewClosed = Event<ViewClosedEvent>;
    using EventFrameBegin = Event<FrameBeginEvent>;
    using EventFrameEnd = Event<FrameEndEvent>;

} // namespace scummredux
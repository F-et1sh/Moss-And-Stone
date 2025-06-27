#pragma once
#include <forcore.hpp>
#include <utility>

namespace FE2D {
    template<typename T>
    class Cached {
    public:
        Cached() : _value() {}
        ~Cached() = default;

        template<typename... Args>
        explicit Cached(Args&&... args) : _value(std::forward<Args>(args)...) {}

        void set_value(const T& value) noexcept {
            _value = value;
            make_dirty();
        }

        void set_value(T&& value) noexcept {
            _value = std::move(value);
            make_dirty();
        }

        T& get_value() noexcept { return _value; }
        const T& get_value() const noexcept { return _value; }

        void make_dirty() noexcept { _is_dirty = true; }
        void clean() noexcept { _is_dirty = false; }
        bool is_dirty() const noexcept { return _is_dirty; }

    private:
        T _value;
        bool _is_dirty = true;
    };
#define FOR_RETURN_IF_CLEAN(cached) if (!cached.is_dirty()) return cached.get_value();
}

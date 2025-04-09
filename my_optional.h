#ifndef MY_OPTIONAL_H
#define MY_OPTIONAL_H

#include <utility>
#include <new>
#include <compare>
#include <type_traits>
#include <exception>
#include <functional>
#include <memory>

class my_bad_optional_access final : std::exception
{
public:
    const char* what() const noexcept override
    {
        return "bad my_optional access";
    }
};

template <typename T>
class my_optional
{
public:
    using value_type = T;

    my_optional() = default;

    my_optional(const T& data) noexcept :
        m_hasVal{ true }
    {
        new(m_data) T(data);
    }

    my_optional(T&& data) noexcept :
        m_hasVal{ true }
    {
        new(m_data) T(std::move(data));
    }

    my_optional(const my_optional& optional) noexcept :
        m_hasVal{ optional.m_hasVal }
    {
        if (optional.m_hasVal)
        {
            new(m_data) T(optional.value());
        }
    }

    my_optional(my_optional&& optional) noexcept :
        m_hasVal{ optional.m_hasVal }
    {
        if (optional.m_hasVal)
        {
            new(m_data) T(std::move(optional.value()));
            optional.m_hasVal = false;
        }
    }

    template <typename... Ts>
    my_optional(std::in_place_t, Ts&&... args) :
        m_hasVal{ true }
    {
        new(m_data) T(std::forward<Ts>(args)...);
    }

    ~my_optional() noexcept
    {
        if (m_hasVal)
        {
            value().~T();
        }
    }

    my_optional& operator=(const my_optional& optional) noexcept
    {
        if (this != &optional)
        {
            my_optional tmp(optional);
            swap(tmp);
        }

        return *this;
    }

    my_optional& operator=(my_optional&& optional) noexcept
    {
        if (this != &optional)
        {
            my_optional tmp(std::move(optional));
            swap(tmp);
        }

        return *this;
    }

    bool operator==(const my_optional& optional) const noexcept
    {
        if (m_hasVal && optional.m_hasVal)
        {
            return value() == optional.value();
        }

        return !m_hasVal && !optional.m_hasVal;
    }

    template <typename U = T, typename = std::enable_if_t<std::three_way_comparable<U>>>
    auto operator<=>(const my_optional& optional) const noexcept
    {
        if (m_hasVal && optional.m_hasVal)
        {
            return value() <=> optional.value();
        }

        if (m_hasVal)
        {
            return std::compare_three_way_result_t<T>::greater;
        }

        if (optional.m_hasVal)
        {
            return std::compare_three_way_result_t<T>::less;
        }

        return std::compare_three_way_result_t<T>::equal;
    }

    T& value()
    {
        if (!m_hasVal)
        {
            throw my_bad_optional_access{};
        }

        return *std::launder(reinterpret_cast<T*>(m_data));
    }

    const T& value() const
    {
        if (!m_hasVal)
        {
            throw my_bad_optional_access{};
        }

        return *std::launder(reinterpret_cast<const T*>(m_data));
    }

    void reset() noexcept
    {
        if (m_hasVal)
        {
            value().~T();
            m_hasVal = false;
        }
    }

    void swap(my_optional& optional) noexcept
    {
        if (this != &optional)
        {
            if (m_hasVal && optional.m_hasVal)
            {
                std::swap(value(), optional.value());
            }
            else if (!m_hasVal && optional.m_hasVal)
            {
                new(m_data) T(std::move(optional.value()));
                std::swap(m_hasVal, optional.m_hasVal);
            }
            else if (m_hasVal && !optional.m_hasVal)
            {
                new(optional.m_data) T(std::move(value()));
                std::swap(m_hasVal, optional.m_hasVal);
            }
        }
    }

    template <typename... Ts>
    void emplace(Ts&&... args)
    {
        my_optional tmp(std::in_place, std::forward<Ts>(args)...);
        swap(tmp);
    }

    T* operator->()
    {
        return &value();
    }

    const T* operator->() const
    {
        return &value();
    }

    T& operator*()
    {
        return value();
    }

    const T& operator*() const
    {
        return value();
    }

    T value_or(const T& def) const
    {
        if (!m_hasVal)
        {
            return def;
        }

        return value();
    }

    T value_or(T&& def) const
    {
        if (!m_hasVal)
        {
            return def;
        }

        return value();
    }

    template <class Func>
    constexpr auto and_then(Func&& f)
    {
        if (!m_hasVal)
        {
            return my_optional{};
        }

        return my_optional(std::invoke(std::forward<Func>(f), value()));
    }

    bool has_value() const noexcept
    {
        return m_hasVal;
    }

    explicit operator bool() const noexcept
    {
        return has_value();
    }

private:
    alignas(T) std::byte m_data[sizeof(T)];
    bool m_hasVal = false;
};

#endif

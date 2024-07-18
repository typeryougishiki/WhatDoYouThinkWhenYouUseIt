#pragma once
template <typename T>
class Singleton
{
protected:
    inline constexpr Singleton() noexcept = default;
    inline constexpr ~Singleton() noexcept = default;

public:
    inline constexpr Singleton(const Singleton&) = delete;
    inline constexpr Singleton(Singleton&&) = delete;

    inline constexpr auto operator=(const Singleton&) = delete;
    inline constexpr auto operator=(Singleton&&) = delete;

    inline static constexpr auto GetSingleton() noexcept
    {
        static T singleton;
        return std::addressof(singleton);
    }
};
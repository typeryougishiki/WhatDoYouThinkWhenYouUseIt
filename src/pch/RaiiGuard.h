#pragma once
class RaiiGuard {
private:
    std::function<void()> OnDestructor;
public:
    inline RaiiGuard(const std::function<void()>& c, const std::function<void()>& d) :OnDestructor(d) {
        c();
    }
    inline ~RaiiGuard() {
        OnDestructor();
    }
    inline constexpr RaiiGuard(const RaiiGuard&) = delete;
    inline constexpr RaiiGuard(RaiiGuard&&) = delete;

    inline constexpr auto operator=(const RaiiGuard&) = delete;
    inline constexpr auto operator=(RaiiGuard&&) = delete;

};
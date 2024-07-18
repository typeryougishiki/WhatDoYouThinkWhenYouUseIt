#pragma once
#include"utils/ThreadSafeObjPkg.h"
class AdditionEditorIDMap{
protected:
    inline constexpr AdditionEditorIDMap() noexcept = default;
    inline constexpr ~AdditionEditorIDMap() noexcept = default;

public:
    inline constexpr AdditionEditorIDMap(const AdditionEditorIDMap&) = delete;
    inline constexpr AdditionEditorIDMap(AdditionEditorIDMap&&) = delete;

    inline constexpr auto operator=(const AdditionEditorIDMap&) = delete;
    inline constexpr auto operator=(AdditionEditorIDMap&&) = delete;

    inline static auto &GetSingleton() noexcept
    {
        static ThreadSafeObjPkj<std::unordered_map<uint32_t, std::string>> singleton{
            std::make_unique<std::unordered_map<uint32_t, std::string>>()
        };
        return singleton;
    }
};
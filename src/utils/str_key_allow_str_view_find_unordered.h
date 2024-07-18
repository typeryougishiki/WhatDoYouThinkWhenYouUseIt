#pragma once
template<typename CharType>
class str_key_allow_str_view_find_unordered {
private:
    inline constexpr str_key_allow_str_view_find_unordered(const str_key_allow_str_view_find_unordered&) = delete;
    inline constexpr str_key_allow_str_view_find_unordered(str_key_allow_str_view_find_unordered&&) = delete;
    inline constexpr str_key_allow_str_view_find_unordered() = delete;
    inline constexpr auto operator=(const str_key_allow_str_view_find_unordered&) = delete;
    inline constexpr auto operator=(str_key_allow_str_view_find_unordered&&) = delete;
public:
    using StringType = std::basic_string<CharType>;
    using StringViewType = std::basic_string_view<CharType>;
private:
    struct Compare {
        using is_transparent = void;  // Pred to use
        inline bool operator()(const StringType& key, const StringViewType& txt) const { return key == txt; }
        inline bool operator()(const StringViewType& key, const StringViewType& txt) const { return key == txt; }
    };

    struct Hash {
        using is_transparent = void;  // Pred to use
        using transparent_key_equal = Compare;
        inline size_t operator()(StringViewType txt) const { return std::hash<StringViewType>{}(txt); }
        inline size_t operator()(const StringType& txt) const { return std::hash<StringType>{}(txt); }
    };
public:
    template<typename ValueType>
    using map = std::unordered_map<StringType, ValueType, Hash, Compare>;
    template<typename ValueType>
    using set = std::unordered_set<StringType, Hash, Compare>;
};
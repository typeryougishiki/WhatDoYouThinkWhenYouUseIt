#pragma once

namespace Plugin
{
    using namespace std::string_view_literals;

    static constexpr std::string_view Name{ SF_PLUGIN_NAME };
    static constexpr std::string_view Author{ SF_PLUGIN_AUTHOR };
    static constexpr std::string_view Email{ SF_PLUGIN_EMAIL };
    static constexpr std::string_view License{ SF_PLUGIN_LICENSE };
    static constexpr std::string_view ConfigPath{ R"(.\Data\SFSE\Plugins\)" SF_PLUGIN_NAME ".ini"};
    static constexpr auto Version{
        REL::Version{
            SF_PLUGIN_VERSION_MAJOR, 
            SF_PLUGIN_VERSION_MINOR, 
            SF_PLUGIN_VERSION_PATCH, 0
        }
    };
}
extern "C" __declspec(dllexport)
constinit auto SFSEPlugin_Version = []() noexcept {
    SFSE::PluginVersionData data{};

    data.PluginVersion(Plugin::Version);
    data.PluginName(Plugin::Name);
    data.AuthorName(Plugin::Author);
    data.UsesAddressLibrary(true);
    data.IsLayoutDependent(true);
    data.CompatibleVersions({ SFSE::RUNTIME_LATEST });

    return data;
}();
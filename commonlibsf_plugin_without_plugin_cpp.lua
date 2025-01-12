-- Usage:
--
-- add_rules("@commonlibsf/plugin", {
--     author = "Author Name",
--     description = "Plugin Description",
--     email = "user@site.com"
-- })


local VERSION_FILE = [[
#include <winres.h>

1 VERSIONINFO
FILEVERSION ${PLUGIN_VERSION_MAJOR}, ${PLUGIN_VERSION_MINOR}, ${PLUGIN_VERSION_PATCH}, 0
PRODUCTVERSION ${PROJECT_VERSION_MAJOR}, ${PROJECT_VERSION_MINOR}, ${PROJECT_VERSION_PATCH}, 0
FILEFLAGSMASK 0x17L
#ifdef _DEBUG
    FILEFLAGS 0x1L
#else
    FILEFLAGS 0x0L
#endif
FILEOS 0x4L
FILETYPE 0x1L
FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040904b0"
        BEGIN
            VALUE "FileDescription", "${PLUGIN_DESCRIPTION}"
            VALUE "FileVersion", "${PLUGIN_VERSION}.0"
            VALUE "InternalName", "${PLUGIN_NAME}"
            VALUE "LegalCopyright", "${PLUGIN_AUTHOR} | ${PLUGIN_LICENSE}"
            VALUE "ProductName", "${PROJECT_NAME}"
            VALUE "ProductVersion", "${PROJECT_VERSION}.0"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x409, 1200
    END
END]]

rule("commonlibsf_plugin_without_plugin_cpp")
    add_deps("win.sdk.resource")

    on_config(function(target)
        import("core.base.semver")
        import("core.project.depend")
        import("core.project.project")

        target:set("arch", "x64")
        target:set("kind", "shared")

        local configs = target:extraconf("rules", "commonlibsf_plugin_without_plugin_cpp")
        local config_dir = path.join(target:autogendir(), "rules", "commonlibsf", "plugin")

        if configs.options then
            if configs.options.sig_scanning then
                configs.options.address_library = false
            else
                configs.options.sig_scanning = false
                if configs.options.address_library == nil then
                    configs.options.address_library = true
                end
            end
            if configs.options.no_struct_use then
                configs.options.layout_dependent = false
            else
                configs.options.no_struct_use = false
                if configs.options.layout_dependent == nil then
                    configs.options.layout_dependent = true
                end
            end
        else
            configs.options = {
                sig_scanning = false,
                address_library = true,
                no_struct_use = false,
                layout_dependent = true
            }
        end

        local config_map = {
            PLUGIN_AUTHOR           = configs.author or "",
            PLUGIN_DESCRIPTION      = configs.description or "",
            PLUGIN_EMAIL            = configs.email or "",
            PLUGIN_LICENSE          = (target:license() or "Unknown") .. " License",
            -- PLUGIN_NAME             = configs.name or target:name(),
            PLUGIN_NAME             = target:name(),
            PLUGIN_VERSION          = target:version() or "0.0.0",
            PLUGIN_VERSION_MAJOR    = semver.new(target:version() or "0.0.0"):major(),
            PLUGIN_VERSION_MINOR    = semver.new(target:version() or "0.0.0"):minor(),
            PLUGIN_VERSION_PATCH    = semver.new(target:version() or "0.0.0"):patch(),
            PROJECT_NAME            = project.name() or "",
            PROJECT_VERSION         = project.version() or "0.0.0",
            PROJECT_VERSION_MAJOR   = semver.new(project.version() or "0.0.0"):major(),
            PROJECT_VERSION_MINOR   = semver.new(project.version() or "0.0.0"):minor(),
            PROJECT_VERSION_PATCH   = semver.new(project.version() or "0.0.0"):patch(),
            -- OPTION_SIG_SCANNING     = configs.options.sig_scanning,
            -- OPTION_ADDRESS_LIBRARY  = configs.options.address_library,
            -- OPTION_NO_STRUCT_USE    = configs.options.no_struct_use,
            -- OPTION_LAYOUT_DEPENDENT = configs.options.layout_dependent
        }
        local add_macro_var = function(name,value)
            target:add("defines",name.."="..value);
        end
        local add_macro_var_str = function(name,value)
            target:add("defines",name.."=\""..value.."\"");
        end
        add_macro_var_str("SF_PLUGIN_NAME",config_map["PLUGIN_NAME"]);
        add_macro_var_str("SF_PLUGIN_AUTHOR",config_map["PLUGIN_AUTHOR"]);
        add_macro_var_str("SF_PLUGIN_EMAIL",config_map["PLUGIN_EMAIL"]);
        add_macro_var_str("SF_PLUGIN_LICENSE",config_map["PLUGIN_LICENSE"]);
        add_macro_var("SF_PLUGIN_VERSION_MAJOR",config_map["PLUGIN_VERSION_MAJOR"]);
        add_macro_var("SF_PLUGIN_VERSION_MINOR",config_map["PLUGIN_VERSION_MINOR"]);
        add_macro_var("SF_PLUGIN_VERSION_PATCH",config_map["PLUGIN_VERSION_PATCH"]);

        local config_parse = function(a_str)
            return a_str:gsub("(%${([^\n]-)})", function(_, a_var)
                local result = config_map[a_var:trim()]
                assert(result ~= nil, "cannot get variable(%s)", a_var)
                if type(result) ~= "string" then
                    result = tostring(result)
                end
                return result
            end)
        end

        local add_file = function(a_path, a_data)
            local file_path = path.join(config_dir, a_path)
            depend.on_changed(function()
                local file = io.open(file_path, "w")
                if file then
                    file:write(config_parse(a_data), "\n")
                    file:close()
                end
            end, { dependfile = target:dependfile(file_path), files = project.allfiles()})
            target:add("files", file_path)
        end

        add_file("version.rc", VERSION_FILE)
    end)

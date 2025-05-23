add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

if is_mode("debug") then
    add_requires("webui", {configs = {log = false}, debug = true})
    add_requires("date", {debug = true})
    add_requires("nlohmann_json", {debug = true})
    if is_config("target_type", "server") then
        add_requires("levilamina", {configs = {target_type = "server"}, debug = true})
    else
        add_requires("levilamina", {configs = {target_type = "client"}, debug = true})
    end
else
    add_requires("webui", {configs = {log = false}})
    add_requires("date")
    add_requires("nlohmann_json")
    if is_config("target_type", "server") then
        add_requires("levilamina", {configs = {target_type = "server"}})
    else
        add_requires("levilamina", {configs = {target_type = "client"}})
    end
end

add_requires("levibuildscript")

option("target_type")
    set_default("server")
    set_showmenu(true)
    set_values("server", "client")
option_end()

includes("ui/xmake.lua")

target("PlayerGuardian")
    add_rules("@levibuildscript/linkrule")
    add_rules("@levibuildscript/modpacker")
    add_cxflags("/EHa", "/utf-8")
    add_defines("NOMINMAX", "UNICODE")
    set_warnings("allextra")
    add_packages(
        "webui",
        "date",
        "nlohmann_json",
        "levilamina"
    )
    set_exceptions("none") -- To avoid conflicts with /EHa.
    if is_mode("debug") then
        set_symbols("debug", "edit")
        add_defines("NO_BUILD_UI")
    end
    set_kind("shared")
    add_deps("ui")
    add_includedirs("src")
    add_headerfiles("src/**.h", "src/**.hpp")
    add_files("src/**.cpp")
    set_languages("clatest", "cxxlatest")
    
    after_build(function (target)
        try
        {
            -- try 代码块
            function ()
                local plugin_packer = import("script.after_build")
                if plugin_packer then
                    plugin_packer.copy_files(target:targetfile())
                end
            end,

            -- catch 代码块
            catch
            {
                -- 发生异常后，被执行
                function (errors)
                    print("please create after_build.lua in script directory")
                end
            }
        }
    end)

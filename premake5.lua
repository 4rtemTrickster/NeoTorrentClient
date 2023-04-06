workspace "NeoTorrentClient"
    architecture "x86_64"
    startproject "NeoTorrentClient"

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    include "Vendor/Walnut"
    include "Vendor/ImGuiFileDialog-For-NeoTorrentClient"



project "NeoTorrentClient"
    location "NeoTorrentClient"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "bin/%{cfg.buildcfg}"
    staticruntime "off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "NTCpch.h"
    pchsource "%{prj.name}/src/NTCpch.cpp"

    files
    {
        "NeoTorrentClient/src/**.h",
        "NeoTorrentClient/src/**.cpp",
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
        "Vendor/Walnut/vendor/imgui",
        "Vendor/Walnut/vendor/glfw/include",
  
        "Vendor/Walnut/Walnut/src",
  
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.glm}",



        "%{prj.name}/src",
        "Vendor/spdlog/include",
        "Vendor/ImGuiFileDialog-For-NeoTorrentClient"
    }

    links
    {
        "Walnut",
        "ImGuiFileDialog"
    }

    filter "system:windows"
    systemversion "latest"
    defines { "NTC_PLATFORM_WINDOWS" }

    filter "configurations:Debug"
    defines 
    {
        "NTC_DEBUG",
        "NTC_PROFILE"
    }
    runtime "Debug"
    symbols "on"

filter "configurations:Release"
    defines 
    {
        "NTC_RELEASE",
        "NTC_PROFILE"
    }
    runtime "Release"
    optimize "on"
    symbols "On"

filter "configurations:Dist"
    defines "NTC_DIST"
    runtime "Release"
    optimize "on"
    symbols "Off"
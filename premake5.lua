outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

if os.target() == "windows" then
    SDLlibname = "SDL3.dll"
    SDLimagelibname = "SDL3_image.dll"

elseif os.target() == "linux" then
    SDLlibname = "libSDL3.so.0"
    SDLimagelibname = "libSDL3_image.so.0"
end

workspace "crimson"
    architecture "x64"
    startproject "launcher"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

project "launcher"
    location "launcher"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch/pch.cpp"

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src/include",
        "%{prj.name}/src/pch",

        "engine/src/include"
    }

    postbuildcommands {
        ("{MKDIR} %{cfg.targetdir}"),

        ("{COPY} ../vendor/lib/sdl3/x64/" .. SDLlibname .. " %{cfg.targetdir}"),
        ("{COPY} ../vendor/lib/sdl3/x64/" .. SDLimagelibname .. " %{cfg.targetdir}"),
    }

    dependson "crimson"
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines "LAUNCHER_PLATFORM_WINDOWS"

    filter "system:linux"
        cppdialect "C++17"
        staticruntime "Off"
        toolset "gcc"

        defines "LAUNCHER_PLATFORM_LINUX"
    
    filter "configurations:Debug"
        defines "LAUNCHER_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "LAUNCHER_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "LAUNCHER_DIST"
        optimize "On"

project "crimson"
    location "games/crimson"
    kind "SharedLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/games/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/games/%{prj.name}")

    pchheader "pch.h"
    pchsource "games/%{prj.name}/src/pch/pch.cpp"

    files {
        "games/%{prj.name}/src/**.h",
        "games/%{prj.name}/src/**.cpp"
    }

    includedirs {
        "games/%{prj.name}/src/pch",
        "games/%{prj.name}/src/include",

        "engine/src/include",

        "engine/vendor/json/include",

        "engine/vendor/sdl3/include",
        "engine/vendor/sdl3-image/include"
    }

    libdirs {
        "bin/" .. outputdir .. "/engine",
        "vendor/lib/sdl3/x64"
    }

    links {
        "engine",
        "SDL3",
        "SDL3_image"
    }

    postbuildcommands {
        ("{MKDIR} %{cfg.targetdir}"),

        ("{COPYDIR} config %{cfg.targetdir}/config"),
        ("{COPYDIR} assets %{cfg.targetdir}/assets")
    }

    dependson "engine"
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines "GAME_PLATFORM_WINDOWS"

    filter "system:linux"
        cppdialect "C++17"
        staticruntime "Off"
        toolset "gcc"

        defines "GAME_PLATFORM_LINUX"
        
        linkoptions {
            "-Wl,-rpath,'$$ORIGIN'",
            "-Wl,-rpath-link,'$$ORIGIN'",
            "-Wl,-rpath,'$$ORIGIN/../../'",  
            "-Wl,-rpath-link,'$$ORIGIN/../../'" 
        }
    
    filter "configurations:Debug"
        defines "GAME_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "GAME_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "GAME_DIST"
        optimize "On"

project "engine"
    location "engine"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/engine")
    objdir ("bin-int/" .. outputdir .. "/engine")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch/pch.cpp"

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src/pch",
        "%{prj.name}/src/include",

        "%{prj.name}/vendor/json/include",

        "%{prj.name}/vendor/sdl3/include",
        "%{prj.name}/vendor/sdl3-image/include"
    }

    libdirs "vendor/lib/sdl3/x64"

    links {
        "SDL3",
        "SDL3_image"
    }
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines "ENGINE_PLATFORM_WINDOWS"

    filter "system:linux"
        cppdialect "C++17"
        staticruntime "Off"
        toolset "gcc"

        defines "ENGINE_PLATFORM_LINUX"

        buildoptions { "-fPIC" }
    
    filter "configurations:Debug"
        defines "ENGINE_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "ENGINE_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "ENGINE_DIST"
        optimize "On"

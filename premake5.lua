outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "crimson"
    architecture "x64"

    configurations {
        "Debug",
        "Release",
        "Dist"
    }

project "crimson"
    location "crimson"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs "%{prj.name}/vendor/sdl3/include"
    libdirs "vendor/lib/sdl3/x64"
    links "SDL3"

    postbuildcommands {
        "{MKDIR} ../bin/" .. outputdir,
        "{COPY} vendor/lib/sdl3/x64/SDL3.dll ../bin/" .. outputdir
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines "CRIMSON_PLATFORM_WINDOWS"

        links {
            "SDL3",
            "SDL3main"
        }
    
    filter "configurations:Debug"
        defines "CRIMSON_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "CRIMSON_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "CRIMSON_DIST"
        optimize "On"

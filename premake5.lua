outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

if os.target() == "windows" then
    SDLlibname = "SDL3.dll"
    SDLimagelibname = "SDL3_image.dll"

elseif os.target() == "linux" then
    SDLlibname = "libSDL3.so.0"
end

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

    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir)

    files {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src/include",
        "%{prj.name}/vendor/sdl3/include",
        "%{prj.name}/vendor/json/include",
        "%{prj.name}/vendor/sdl3-image/include"
    }

    postbuildcommands {
        ("{MKDIR} %{cfg.targetdir}"),

        ("{COPY} ../vendor/lib/sdl3/x64/" .. SDLlibname .. " %{cfg.targetdir}"),
        ("{COPY} ../vendor/lib/sdl3/x64/" .. SDLimagelibname .. " %{cfg.targetdir}"),

        ("{COPYDIR} config %{cfg.targetdir}/config"),
        ("{COPYDIR} assets %{cfg.targetdir}/config")
    }

    links {
        "SDL3",
        "SDL3_image"
    }
    
    libdirs "vendor/lib/sdl3/x64"
    
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines "CRIMSON_PLATFORM_WINDOWS"

    filter "system:linux"
        cppdialect "C++17"
        staticruntime "Off"
        toolset "gcc"

        defines "CRIMSON_PLATFORM_LINUX"
        
        linkoptions {
            "-Wl,-rpath,'$$ORIGIN'",
            "-Wl,-rpath-link,'$$ORIGIN'"
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

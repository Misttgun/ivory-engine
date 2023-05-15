workspace "Ivory"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
	}

outputdir = "%{cfg.buildcfg}"

project "Ivory"
    location "Ivory"
    kind "ConsoleApp"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/imgui/*.cpp",
        "%{prj.name}/vendor/imgui/backends/imgui_impl_sdl2.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_sdlrenderer.cpp"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{prj.name}/vendor/SDL2/include",
        "%{prj.name}/vendor/glm",
        "%{prj.name}/vendor/imgui",
        "%{prj.name}/vendor/imgui/backends"
    }

    libdirs 
    {
        "%{prj.name}/vendor/SDL2/lib/x64"
    }

    links
    {
        "SDL2",
        "SDL2main",
        "SDL2_image",
        "SDL2_mixer",
        "SDL2_ttf"
    }

    filter "system:windows"
        cppdialect "C++17"
        staticruntime "On"
        systemversion "latest"

        defines
        {
            "IV_PLATFORM_WINDOWS"
        }

        postbuildcommands 
        {
            "{COPYFILE} ..\\Ivory\\vendor\\SDL2\\lib\\x64\\SDL2.dll %{cfg.targetdir}",
            "{COPYFILE} ..\\Ivory\\vendor\\SDL2\\lib\\x64\\SDL2_image.dll %{cfg.targetdir}",
            "{COPYFILE} ..\\Ivory\\vendor\\SDL2\\lib\\x64\\SDL2_ttf.dll %{cfg.targetdir}",
            "{COPYFILE} ..\\Ivory\\vendor\\SDL2\\lib\\x64\\SDL2_mixer.dll %{cfg.targetdir}",
            "{COPYDIR} ..\\Ivory\\assets %{cfg.targetdir}\\assets"
        }

    filter "configurations:Debug"
        defines {"IV_DEBUG"}
        symbols "On"

    filter "configurations:Release"
        defines {"IV_RELEASE"}
        optimize "On"
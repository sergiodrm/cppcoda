--premake5.lua

outputdir = "%{wks.location}/bin/%{cfg.buildcfg}"
temporaldir = "%{wks.location}/temp/%{cfg.buildcfg}_%{cfg.architecture}"
libdir = "%{wks.location}/lib/%{cfg.buildcfg}"

newaction {
    trigger = "clear",
    description = "clear project files",
    execute = function()
        os.execute("del /q /s /f /a:h .vs")
        os.execute("del /q /s *.vcxproj*")
        os.execute("del /q /s *.sln")
        os.execute("del /q /s bin")
        os.execute("del /q /s temp")
        os.execute("rmdir /q /s bin")
        os.execute("rmdir /q /s temp")
        os.execute("rmdir /q /s .vs")
    end
}

workspace "cppcoda"
    configurations {"Debug", "Release"}
    platforms {"Win64"}
    startproject "cppcoda_test"
    flags {"MultiProcessorCompile"}

    -- filter config
    filter "platforms:Win64"
        system "Windows"
        architecture "x86_64"

    filter "configurations:Debug"
        defines {"_DEBUG"}
        symbols "On"
        optimize "Off"
    
    filter "configurations:Release"
        defines {"_NDEBUG"}
        symbols "On"
        optimize "On"

    -- deactive filter
    filter{}
        
    project "cppcoda"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"

        targetdir "%{outputdir}"
        targetname "cppcoda"
        objdir "%{temporaldir}"
        location "%{wks.location}/cppcoda/"

        defines {  }
        files { 
            "cppcoda/**.h", "cppcoda/**.cpp",
        }

        includedirs {
        }
        links {
        }
        
        filter "configurations:Debug"
        links {
            }
            targetsuffix "d"
            
        filter "configurations:Release"
        links {
            }
        
    
    project "cppcoda_test"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        
        targetdir "%{outputdir}"
        objdir "%{temporaldir}"
        location "%{wks.location}/test"
        
        links { "cppcoda" }
        files { "test/**.h", "test/**.cpp"}

        defines {  }
        includedirs {
            "test/",
            "cppcoda/",
            "thirdparty/googletest/googletest/include"
        }

        filter "configurations:Debug"
            links { "thirdparty/googletest/build/lib/debug/gtest", "thirdparty/googletest/build/lib/debug/gmock" }
            targetname "cppcoda_test_dbg"
        filter "configurations:Release"
            links { "thirdparty/googletest/build/lib/release/gtest", "thirdparty/googletest/build/lib/release/gmock" }
            targetname "cppcoda_test"



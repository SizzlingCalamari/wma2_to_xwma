
solution "wma2_to_xwma"
    basedir ".."
    location (_ACTION)
    targetdir "../bin"
    startproject "wma2_to_xwma"
    configurations { "Debug", "Release" }
    platforms "x64"
    flags { "MultiProcessorCompile", "Symbols" }

    defines "_CRT_SECURE_NO_WARNINGS"
    configuration "Debug"
        defines { "DEBUG" }
    configuration "Release"
        defines { "NDEBUG" }
        optimize "Full"
    configuration {}

    project "wma2_to_xwma"
        kind "ConsoleApp"
        language "C++"

        files
        {
            "../src/**.h",
            "../src/**.cpp"
        }
        includedirs
        {
        }
        libdirs
        {
            
        }
        links
        {
        }
        debugargs
        {
            "D:/projects/wma2_to_xwma/sound/beep1.wma D:/projects/wma2_to_xwma/sound/beep1_out.xwma"
        }
    project "*"

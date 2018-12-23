-- Directory where you've installed OVR and Source SDK Base 2013
include("buildconfig.lua")

workspace "gmcl_openvr"
    configurations { "Release" }
    location ( "projects/" .. os.target() )

project "gmcl_openvr"
    kind         "SharedLib"
    architecture "x86"
    language     "C++"
	links {
		sdkPaths.."/openvr/lib/win32/openvr_api.lib"
	}
	includedirs  {
		"include/",
		sdkPaths.."/openvr/headers",
		sdkPaths.."/source-sdk-2013/mp/src/public",
	}
    targetdir    "build"
    symbols      "On"

    if os.istarget( "windows" ) then targetsuffix "_win32" end
    if os.istarget( "macosx" )  then targetsuffix "_osx"   end
    if os.istarget( "linux" )   then targetsuffix "_linux" end

    configuration "Release"
        optimize "Speed"
        staticruntime "On"

    files
    {
      "src/**.*",
      "include/**.*"
    }

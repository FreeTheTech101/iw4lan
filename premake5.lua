solution "iw4lan"
	configurations { "Debug", "Release" }
	
	flags { "StaticRuntime", "No64BitChecks", "Symbols", "Unicode" }
	
	flags { "NoIncrementalLink", "NoEditAndContinue" }

	includedirs { "deps/include/" }
		
	libdirs { "deps/lib/" }
	
	configuration "Debug*"
		targetdir "bin/debug"
		defines "NDEBUG"
		
	configuration "Release*"
		targetdir "bin/release"
		defines "NDEBUG"
			
	project "steam_api"
		targetname "iw4lan"
		language "C++"
		kind "SharedLib"

		defines { "WIN32", "NDEBUG", "_WINDOWS", "_WINDLL", "_USRDLL", "STEAM_API_EXPORTS" }

		flags { "NoMinimalRebuild" }
		
		files
		{
			"steam_api/**.cpp",
			"steam_api/**.h",
			"steam_api/StdInc.cpp"
		}

		pchsource "steam_api/StdInc.cpp"
		pchheader "StdInc.h"

		libdirs { "libcef/lib/" }

		includedirs { "deps/include/osw/", "deps/include/python/", "libcef/" }

		links
		{
			"tomcrypt",
			"tommath",
			"tinyxml",
			"zlib",
			"ws2_32",
			"winmm",
			"wldap32",
			"dbghelp",
			"python32",
			"python32_socket",
			"shlwapi",
			"libcurl",
			"pdcurses",
			"iphlpapi",
			"mono",
			"mongoose",
			"libcef",
			"libcef_dll_wrapper"
		}

		configuration "windows"
			linkoptions "/IGNORE:4248 /IGNORE:4049 /IGNORE:4099 /DYNAMICBASE:NO /SAFESEH:NO"

		prebuildcommands
		{
			"pushd \"$(SolutionDir)\\deps\\tools\"",
			"call \"$(SolutionDir)\\deps\\tools\\gitrev.cmd\"",
			"popd"
		}

	project "InfinityScript"
		targetname "InfinityScript"
		language "C#"
		kind "SharedLib"

		files
		{
			"InfinityScript/**.cs"
		}

		links
		{
			"System",
			"System.Core",
			"System.Data",
			"System.Xml",
			"Microsoft.CSharp"
		}
		
	project "libcef_dll_wrapper"
		targetname "libcef_dll_wrapper"
		language "C++"
		kind "StaticLib"

		defines { "USING_CEF_SHARED", "NOMINMAX", "WIN32" }

		flags { "NoIncrementalLink", "NoMinimalRebuild" }

		includedirs { "libcef/" }

		buildoptions "/MP"

		files
		{
			"libcef/libcef_dll/**.cc",
			"libcef/libcef_dll/**.cpp",
			"libcef/libcef_dll/**.h"
		}
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
		optimize "Speed"
			
	project "clientdll"
		targetname "iw4lan"
		language "C++"
		kind "SharedLib"

		defines { "WIN32", "NDEBUG", "_WINDOWS", "_WINDLL", "_USRDLL", "STEAM_API_EXPORTS", "PRE_RELEASE_DEMO", "D3D_EXPEROMINTS", "OLD_CRUFTY_CRUFT" }

		flags { "NoMinimalRebuild" }
		
		files
		{
			"src/**.cpp", "src/**.h", "src/StdInc.cpp"
		}

		pchsource "src/StdInc.cpp"
		pchheader "StdInc.h"
		
		includedirs { "deps/include/osw/", "deps/include/python/" }
		
		libdirs { "deps/lib/" }
	
		links { "ws2_32.lib", "winmm.lib", "libcurl.lib", "wldap32.lib", "dbghelp.lib", "pdcurses.lib", "shlwapi.lib" }

		configuration "windows"
			linkoptions "/DYNAMICBASE:NO /SAFESEH:NO /LARGEADDRESSAWARE"
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
			
	project "clientdll"
		targetname "iw4lan"
		language "C++"
		kind "SharedLib"

		defines { "WIN32", "NDEBUG", "_WINDOWS", "_WINDLL", "_USRDLL", "STEAM_API_EXPORTS" }

		flags { "NoMinimalRebuild" }
		
		files
		{
			"src/**.cpp", "src/**.h", "src/StdInc.cpp"
		}

		pchsource "src/StdInc.cpp"
		pchheader "StdInc.h"
		
		includedirs { "deps/include/osw/", "deps/include/python/" }
		
		libdirs { "deps/lib/" }
	
		links { "ws2_32.lib", "winmm.lib", "wldap32.lib", "dbghelp.lib", "shlwapi.lib", "libcurl.lib", "pdcurses.lib", "iphlpapi.lib" }

		configuration "windows"
			linkoptions "/IGNORE:4248 /IGNORE:4049 /IGNORE:4099 /DYNAMICBASE:NO /SAFESEH:NO"

		local buildHost = os.getenv("COMPUTERNAME") or 'dummy'

		if buildHost == 'win32-pc' then
			postbuildcommands { "copy \"$(TargetPath)\" \"C:\\Documents and Settings\\Administrator\\Desktop\\Games\\Modern Warfare 2\\iw4lan.dll\"" }
		end
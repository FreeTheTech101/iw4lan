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

		defines { "WIN32", "NDEBUG", "_WINDOWS", "_WINDLL", "_USRDLL", "STEAM_API_EXPORTS", "PRE_RELEASE_DEMO", "D3D_EXPEROMINTS", "OLD_CRUFTY_CRUFT" }

		flags { "NoMinimalRebuild" }
		
		files
		{
			"steam_api/**.cpp", "steam_api/**.h", "steam_api/StdInc.cpp"
		}

		pchsource "steam_api/StdInc.cpp"
		pchheader "StdInc.h"
		
		includedirs { "deps/include/osw/", "deps/include/python/" }
		
		libdirs { "deps/lib/" }

		links { "tomcrypt.lib", "tommath.lib", "tinyxml.lib", "zlib.lib", "ws2_32.lib", "winmm.lib", "wldap32.lib", "dbghelp.lib", "python32.lib", "python32_socket.lib", "shlwapi.lib", "libcurl.lib", "pdcurses.lib", "iphlpapi.lib", "mono.lib", "mongoose.lib" }

		configuration "windows"
			linkoptions "/IGNORE:4248 /IGNORE:4049 /IGNORE:4099 /DYNAMICBASE:NO /SAFESEH:NO"

		local buildHost = os.getenv("COMPUTERNAME") or 'dummy'

		if buildHost == 'win32-pc' then
			postbuildcommands { "copy \"$(TargetPath)\" \"C:\\Documents and Settings\\Administrator\\Desktop\\Games\\Modern Warfare 2\\iw4lan.dll\"" }
		end

	project "InfinityScript"
		targetname "InfinityScript"
		language "C#"
		kind "SharedLib"

		files
		{
			"InfinityScript/**.cs"
		}

		links { "System", "System.Core", "System.Data", "System.Xml", "System.Web.Razor.Unofficial", "Community.CsharpSqlite", "Microsoft.CSharp", "Nancy", "Nancy.ViewEngines.Razor", "Nancy.ViewEngines.Razor.BuildProviders" }

		libdirs { "InfinityScript/deps/" }

		local buildHost = os.getenv("COMPUTERNAME") or 'dummy'

		if buildHost == 'win32-pc' then
			postbuildcommands { "copy \"$(TargetPath)\" \"C:\\Documents and Settings\\Administrator\\Desktop\\Games\\Modern Warfare 2\\InfinityScript.dll\"" }
		end
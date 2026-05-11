solution "GMad"

	newoption {
	   trigger     = "outdir",
	   value       = "bin/",
	   description = "Output directory"
	}
	
	newoption {
	   trigger     = "bootil_lib",
	   value       = "lib/",
	   description = "The folder containing the bootil lib"
	}
	
	newoption {
	   trigger     = "bootil_inc",
	   value       = "include/",
	   description = "The folder containing the bootil h"
	}
	
	language "C++"

	symbols "On"
	editandcontinue "Off"
	enablepch "Off"
	staticruntime "On"
	vectorextensions "SSE"

	targetdir ( _OPTIONS.outdir )
	includedirs { "include/", _OPTIONS.bootil_inc }
	libdirs { _OPTIONS.bootil_lib }
		
	if os.ishost( "linux" ) or os.ishost( "macosx" ) then
		buildoptions { "-fPIC" }
		linkoptions  { "-fPIC" }
	end
	
	if os.ishost( "linux" ) then
		links { "pthread" }
	end
	
	configurations
	{ 
		"Release"
	}
	
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "Speed"
		floatingpoint "Fast"

	project "GMad"
		uuid ( "AB8E7B19-A70C-4737-88DE-F02160737C2E" )
		files { "src/**.*", "include/**.*" }
		kind "ConsoleApp"
		targetname( "gmad" )
		
		if os.ishost( "linux" ) then
			targetname( "gmad_linux" )
		end
		
		if os.ishost( "macosx" ) then
			targetname( "gmad_osx" )
		end
		
		links( { "bootil_static" } )
-- Create Solution

solution "SimpleENG"
configurations{"Debug","Release"}
location("build")
targetdir("data")



project "SimpleENGEngine"
	kind "ConsoleApp"
	language "C++"
	objdir "build/AwesomeEngine"
	defines {"_GLFW_WIN32","_GLFW_WGL","_GLFW_USE_OPENGL","GLEW_STATIC","WINDOWS_PLATFORM"}
	libdirs{"lib"}
	links{"opengl32"}
	includedirs{"include","deps"}
	files{"include/**.h",
		  "include/**.hpp",
		  "src/**.cc",
		  "src/**.c",
		  "src/**.cpp",
		  "src/**.h",
		  }
	files{"examples/main.cc"}
	debugdir("data")
	configuration "Debug"
	defines { "DEBUG" }
	flags "Symbols"

project "SimpleENGBuild"
	kind "StaticLib"
	language "C++"
	targetdir("lib")
	objdir "build/SimpleENGBuild"
	defines {"_GLFW_WIN32","_GLFW_WGL","_GLFW_USE_OPENGL","GLEW_STATIC","WINDOWS_PLATFORM"}
	links{"opengl32"}
	includedirs{"include","deps"}
	files{"include/**.h",
	  "deps/**.h",
	  "deps/**.hpp",
	  "src/**.cc",
	  "src/**.c",
	  "src/**.cpp",
	  "src/**.h",
	  }
	configuration ("Debug")
	defines { "DEBUG" }
	flags "Symbols"



project "SimpleENGTest"
	kind "ConsoleApp"
	language "C++"
	objdir "build/SimpleENGTest"
	libdirs{"lib"}
	links{"opengl32","SimpleENGBuild"}
	defines {"_GLFW_WIN32","_GLFW_WGL","_GLFW_USE_OPENGL","GLEW_STATIC","WINDOWS_PLATFORM"}
	includedirs{"include","deps"}
	files{"include/**.h"}
	files{"examples/main.cc"}
    debugdir("data")
	configuration ("Debug")
	defines { "DEBUG" }
	flags "Symbols"







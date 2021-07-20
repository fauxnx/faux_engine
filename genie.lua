solution("faux_engine")
  configurations { "Debug", "Release" }
  language "C++"
  flags { "Cpp17" }
  platforms "x64"
  location "./build"
  objdir "./build/obj"
  defines { "_CRT_SECURE_NO_WARNINGS" }
  libdirs { "$(VULKAN_SDK)/Lib"}

  project("demo")
    kind "ConsoleApp"
    files {
      "./examples/*.cpp",
    }
    includedirs {
      "./include/",
      "$(VULKAN_SDK)/Include",
    }
    links { "faux_engine", "vulkan-1" }

    configuration "Debug"
      flags {"Symbols" }
      defines { "DEBUG_"}
      targetdir("./bin/debug/demo")
      libdirs { "./bin/debug" }

    configuration "Release"
      flags { "Optimize" }
      targetdir("./bin/release/demo")
      libdirs { "./bin/release" }

  project("faux_engine")
    kind "StaticLib"
    targetextension(".lib")

    includedirs {
      "$(VULKAN_SDK)/Include",
      "./include/",
      "./src/",
      "./deps/",
      "./deps/glm/",
      "./deps/glfw/include/",
      "./deps/KHR/",
      "./deps/glad/",
      "./deps/easyloggingpp/",
    }

    files {
  		"./include/**.h",
      "./src/**.h",
      "./src/**.cpp",
      "./deps/glfw/src/context.c",
      "./deps/glfw/src/egl_context.c",
      "./deps/glfw/src/egl_context.h",
      "./deps/glfw/src/init.c",
      "./deps/glfw/src/input.c",
      "./deps/glfw/src/internal.h",
      "./deps/glfw/src/mappings.h",
      "./deps/glfw/src/monitor.c",
      "./deps/glfw/src/osmesa_context.c",
      "./deps/glfw/src/osmesa_context.h",
      "./deps/glfw/src/vulkan.c",
      "./deps/glfw/src/wgl_context.c",
      "./deps/glfw/src/wgl_context.h",
      "./deps/glfw/src/win32_init.c",
      "./deps/glfw/src/win32_joystick.c",
      "./deps/glfw/src/win32_joystick.h",
      "./deps/glfw/src/win32_monitor.c",
      "./deps/glfw/src/win32_platform.h",
      "./deps/glfw/src/win32_thread.c",
      "./deps/glfw/src/win32_time.c",
      "./deps/glfw/src/win32_window.c",
      "./deps/glfw/src/window.c",
      "./deps/glm/**",
      "./deps/glad/*",
      "./deps/easyloggingpp/*",
      "./genie.lua",
    }

    configuration "windows"
      links { "vulkan-1", "opengl32" }
      defines { "FAUXENGINE_WIN32", "GLM_FORCE_AVX2", "_GLFW_WIN32" }

    configuration "linux"
      links { "vulkan-1", "GL", "X11", "Xcursor", "Xrandr", "Xi", "dl", "pthread" }
      defines { "FAUXENGINE_LINUX", "_GLFW_X11" }

    configuration "Debug"
      flags { "Symbols", "EnableAVX2" }
      targetdir("./bin/debug")
      defines { "DEBUG_" }

    configuration "Release"
      flags { "Optimize", "EnableAVX2" }
      targetdir("./bin/release")

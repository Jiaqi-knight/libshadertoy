cmake_minimum_required(VERSION 3.10)

project(libshadertoy-examples)
include(FindPkgConfig)

# OpenGL libraries
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

if (NOT EPOXY_FOUND)
	pkg_search_module(EPOXY REQUIRED epoxy)
	link_directories(${EPOXY_LIBDIR})
endif()

if (NOT GLFW3_FOUND)
	find_package(GLFW3 QUIET)

	if (NOT GLFW3_FOUND)
		pkg_search_module(GLFW3 glfw3)
		if (GLFW3_FOUND)
			link_directories(${GLFW3_LIBDIR})
		endif()
	endif()

	if (GLFW3_FOUND AND NOT GLFW3_LIBRARIES)
		set(GLFW3_LIBRARIES glfw)
	endif()
endif()

# Extra libraries for shadertoy API examples
find_package(Boost 1.58 COMPONENTS filesystem date_time program_options)

find_package(CURL)

pkg_search_module(JSONCPP jsoncpp)
if (JSONCPP_FOUND)
	link_directories(${JSONCPP_LIBDIR})
endif()

# Extra libraries for 60-xscreensaver
find_package(X11)

# libshadertoy
if (NOT TARGET shadertoy-static)
	find_package(shadertoy 1.0.0 REQUIRED)
endif()

# Directories
set(INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SHADERS_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Configure examples
set(ST_INC_DIR ${CMAKE_CURRENT_BINARY_DIR})

function(convert_to_cstring_literal var value)
	string(REGEX REPLACE "([\\\$\"])" "\\\\\\1" escaped "${value}")
	set("${var}" "\"${escaped}\"" PARENT_SCOPE)
endfunction()
convert_to_cstring_literal(ST_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
configure_file(${INCLUDE_ROOT}/test_config.hpp.in ${ST_INC_DIR}/test_config.hpp)

if (OpenGL_FOUND AND EPOXY_FOUND AND GLFW3_FOUND)
	add_subdirectory(src/00-build)
	add_subdirectory(src/10-gradient)
	add_subdirectory(src/11-image)
	add_subdirectory(src/15-uniforms)
	add_subdirectory(src/20-geometry)
else()
	message(STATUS "Not building examples 00-build, 10-gradient, 11-image, 15-uniforms and 20-geometry")
	message(STATUS "You might want to install libgl-mesa-dev, libepoxy-dev and libglfw-dev")
endif()

if (OpenGL_FOUND AND EPOXY_FOUND AND GLFW3_FOUND AND Boost_FOUND AND CURL_FOUND AND JSONCPP_FOUND)
	add_subdirectory(src/50-shadertoy)
else()
	message(STATUS "Not building example 50-shadertoy")
	message(STATUS "You also need libcurl-dev, libboost-all-dev and libjsoncpp-dev")
endif()

if (OpenGL_FOUND AND EPOXY_FOUND AND Boost_FOUND AND CURL_FOUND AND JSONCPP_FOUND AND X11_FOUND
		AND X11_Xft_LIB AND X11_Xmu_LIB AND X11_Xt_LIB)
	add_subdirectory(3rdparty/xscreensaver)
	add_subdirectory(src/60-xscreensaver)
endif()


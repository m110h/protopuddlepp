cmake_minimum_required(VERSION 3.8)

if(WIN32)
	message("Platform: WIN32")
else()
	message("Unsupported platform yet")
	return()
endif()

# Set a default build type if none was specified
# 
set(default_build_type "Release")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

project(protopuddlexx VERSION 1.0.0 LANGUAGES CXX)

add_executable(protopuddlexx main.cpp entities.cpp)

set(wxdir "d:/Development/Cpp/MyExamples/wx/wx/")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(wxlibs wxmsw31ud_core wxbase31ud)
	target_compile_options(protopuddlexx PRIVATE -Wall -g)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(wxlibs wxmsw31u_core wxbase31u)
	target_compile_options(protopuddlexx PRIVATE -Wall -O2)
else()
	return()
endif()

target_compile_features(protopuddlexx PRIVATE cxx_std_17)

target_include_directories(protopuddlexx PRIVATE ${wxdir}/include)
target_include_directories(protopuddlexx PRIVATE ${wxdir}/lib/mswud)

target_link_directories(protopuddlexx PRIVATE ${wxdir}/lib)
target_link_libraries(protopuddlexx ${wxlibs})

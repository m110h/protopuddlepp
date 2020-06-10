cmake_minimum_required(VERSION 3.10)

# Check a platform
if(WIN32)
	message("Platform: WIN32")
else()
	message("Unsupported platform yet")
	return()
endif()

# Set a default build type if none was specified by option -DCMAKE_BUILD_TYPE=Release
set(default_build_type "Release")

if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(default_build_type "Debug")
endif()
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
else()
	message("Configuration: ${CMAKE_BUILD_TYPE}")
endif()

project(protopuddlepp VERSION 0.6.0 LANGUAGES CXX)

set(sources main.cpp entities.cpp drawpanel.cpp genesframe.cpp propertiesdialog.cpp thirdparty/allocator/allocator.cpp thirdparty/allocator/callocator.cpp thirdparty/allocator/freelistallocator.cpp)

add_executable(protopuddlepp ${sources})

# Define wxWidgets' root directory
set(wxdir "d:/Development/Cpp/MyExamples/wx/wx/")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(wxlibs wxmsw31ud_core wxbase31ud)
	# target_compile_options(protopuddlepp PRIVATE -Wall -g)
	target_link_options(protopuddlepp PRIVATE -mwindows)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	set(wxlibs wxmsw31u_core wxbase31u)
	# target_compile_options(protopuddlepp PRIVATE -Wall)
	# -s - Remove all symbol table and relocation information from the executable
	target_link_options(protopuddlepp PRIVATE -s -mwindows)
else()
	message("Unknown configuration name ${CMAKE_BUILD_TYPE}. Stop")
	return()
endif()

# Enable c++17
target_compile_features(protopuddlepp PRIVATE cxx_std_17)

# Additional directories that contain header files
target_include_directories(protopuddlepp PRIVATE ${wxdir}/include)
target_include_directories(protopuddlepp PRIVATE ${wxdir}/lib/mswud)

# Additional directories that contain libraries
target_link_directories(protopuddlepp PRIVATE ${wxdir}/lib)

target_link_libraries(protopuddlepp ${wxlibs})

cmake_minimum_required(VERSION 3.10)

set(PROJECT protopuddlepp)

project(${PROJECT} LANGUAGES CXX)
# set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set a default build type if none was specified by option -DCMAKE_BUILD_TYPE=Release
set(default_build_type "Debug")

#if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
#  set(default_build_type "Debug")
#endif()
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
else()
	message("Configuration: ${CMAKE_BUILD_TYPE}")
endif()

# Check a platform
if(WIN32)
	message("Platform: WIN32")
else()
	message("Platform: UNIX-like OS")
	find_package(wxWidgets COMPONENTS core base REQUIRED)
endif()

file(GLOB SOURCES_LIST "*.cpp" "*.h")
file(GLOB SOURCES_LIST_THIRDPARTY "thirdparty/*.cpp" "thirdparty/*.h")
file(GLOB SOURCES_LIST_THIRDPARTY_ALLOCATOR "thirdparty/allocator/*.cpp" "thirdparty/allocator/*.h")

#set(SOURCES_LIST
#	main.cpp
#	entities.cpp
#	drawpanel.cpp
#	genesframe.cpp
#	propertiesdialog.cpp
#	thirdparty/allocator/allocator.cpp
#	thirdparty/allocator/callocator.cpp
#	thirdparty/allocator/freelistallocator.cpp
#	)

add_executable(${PROJECT} ${SOURCES_LIST} ${SOURCES_LIST_THIRDPARTY} ${SOURCES_LIST_THIRDPARTY_ALLOCATOR})

if (WIN32)
	# Define wxWidgets' root directory
	# 	It must have a next structure of directories: bin, include, lib, lib/mswud, lib/mswu
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
endif()

# Enable c++17
target_compile_features(${PROJECT} PRIVATE cxx_std_17)

if (WIN32)
	# Additional directories that contain header files
	target_include_directories(${PROJECT} PRIVATE ${wxdir}/include)
	
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		target_include_directories(${PROJECT} PRIVATE ${wxdir}/lib/mswud)
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_include_directories(${PROJECT} PRIVATE ${wxdir}/lib/mswu)
	endif()

	# Additional directories that contain libraries
	target_link_directories(${PROJECT} PRIVATE ${wxdir}/lib)

	target_link_libraries(${PROJECT} ${wxlibs})
else()
	include(${wxWidgets_USE_FILE})
	target_link_libraries(${PROJECT} ${wxWidgets_LIBRARIES})
endif()


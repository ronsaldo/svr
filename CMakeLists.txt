# Leng cmake build script.
cmake_minimum_required(VERSION 2.8)
project(SVR)

# Check the build type
if (CMAKE_BUILD_TYPE STREQUAL "")
	# CMake defaults to leaving CMAKE_BUILD_TYPE empty. This screws up
	# differentiation between debug and release builds.
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Output paths.
set(CMAKE_DEBUG_POSTFIX "_d")
 
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist")
set(SVR_DATA_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/dist/data")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/;${CMAKE_MODULE_PATH}")

# Turn warnings and use C++ 2011.
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# using Visual Studio C++
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
	
	# Export symbols from applications.
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic")
endif()

# Perform platform checks
include(${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)
include(${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake)
include(${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
include(${CMAKE_ROOT}/Modules/CheckLibraryExists.cmake)

# Find opengl
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(SEND_ERROR "Failed to find OpenGL")
endif()

# Find GLEW
find_library(GLEW_LIB GLEW)
if(NOT GLEW_LIB)
    message(SEND_ERROR "Failed to find GLEW")
endif()

# Find freetype.
find_package(Freetype)
if(FREETYPE_FOUND)
    include_directories(${FREETYPE_INCLUDE_DIRS})
endif()

# Use Zlib
find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

# Find OpenCL
find_library(OpenCL_LIB OpenCL)
if(NOT OpenCL_LIB)
    message(SEND_ERROR "Failed to find OpenCL library")
endif()

find_path(OpenCL_INCLUDE_DIR CL/cl.h
         PATH_SUFFIXES CL)
if(NOT OpenCL_INCLUDE_DIR)
    message(SEND_ERROR "Failed to find OpenCL headers")
endif()

# Find DCMTK
find_library(OFSTD_LIB ofstd)
find_library(DCMDATA_LIB dcmdata)
find_library(DCMIMGLE_LIB dcmimgle)
find_library(DCMIMAGE_LIB dcmimage)

# Use GLM, rapidxml
include_directories(
"${PROJECT_SOURCE_DIR}/thirdparty/glm"
"${PROJECT_SOURCE_DIR}/thirdparty/rapidxml"
)

set(SVR_DEP_LIBS ${ZLIB_LIBRARIES} ${OpenCL_LIB} ${GLEW_LIB} ${OPENGL_gl_LIBRARY} ${FREETYPE_LIBRARIES})

# Use pkg-config.
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)

    # Find SDL2
    pkg_check_modules(SDL2 REQUIRED sdl2)
    include_directories(${SDL2_INCLUDE_DIRS})

    # Find unittest++
    pkg_check_modules(UNITTESTMM unittest++)
    if(UNITTESTMM_FOUND)
        include_directories(${UNITTESTMM_INCLUDE_DIRS})
    endif()

    # Leng dependencies libraries
    set(SVR_DEP_LIBS ${SDL2_LIBRARIES} ${SVR_DEP_LIBS})

    # Set output dir.
    set(EXECUTABLE_OUTPUT_PATH "${SVR_BINARY_DIR}/dist")
    set(LIBRARY_OUTPUT_PATH "${SVR_BINARY_DIR}/dist")
else()
    # For VisualStudio in Window
    find_path(SDL2_INCLUDE_DIR
      NAMES SDL.h
    )

    find_path(SDL2_LIBRARY_PATH
      NAMES SDL2.lib SDL2.a
    )

    find_library(SDL2_LIBRARY NAMES SDL2 PATHS ${SDL2_LIBRARY_PATH})
	find_library(SDL2_MAIN_LIBRARY NAMES SDL2main PATHS ${SDL2_LIBRARY_PATH})

    set(SVR_DEP_LIBS ${SVR_DEP_LIBS} ${SDL2_LIBRARY} ${SDL2_MAIN_LIBRARY})
    include_directories("${SDL2_INCLUDE_DIR}")
endif()

# Add the current directory.
include_directories(
.
"${PROJECT_SOURCE_DIR}/core/include"
)

# Build the core
add_subdirectory(core)

# Build the app
add_subdirectory(app)

# Build the data.
add_subdirectory(data)

# Add dicom2fits and dicomviewer
if(OFSTD_LIB AND DCMDATA_LIB AND DCMIMGLE_LIB AND DCMIMAGE_LIB)
    add_subdirectory(dicom)
    add_subdirectory(dicomviewer)
endif()

# Build the tests.
if(UNITTESTMM_FOUND)
    add_subdirectory(tests)
endif()

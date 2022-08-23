CMAKE_MINIMUM_REQUIRED (VERSION 3.6)
Project (POE_SextantRoller CXX)
set(CMAKE_CXX_STANDARD 17)

Include (FetchContent)

Set (SRC src/)

File (GLOB HEADER_FILES
      ${SRC}/*.h
      ${SRC}/*.hpp
)

File (GLOB SRC_FILES
      ${SRC}/*.cpp
      ${HEADER_FILES}
)

FILE (GLOB MANIFEST_FILE ${SRC}/res/wx.rc)

if(MSVC)
      Set (PLATFORM WIN32)
elseif(!WIN32)
    MESSAGE(FATAL_ERROR "Wont work on non windows machines" )
endif()

# https://github.com/myarichuk/OpenCV.CMake/blob/master/cmake/build_external_project.cmake
function (build_external_project target prefix url branch) #FOLLOWING ARGUMENTS are the CMAKE_ARGS of ExternalProject_Add

    set(trigger_build_dir ${CMAKE_BINARY_DIR}/force_${target})

    #mktemp dir in build tree
    file(MAKE_DIRECTORY ${trigger_build_dir} ${trigger_build_dir}/build)

    #generate false dependency project
    set(CMAKE_LIST_CONTENT "
        cmake_minimum_required(VERSION 3.0)
        project(ExternalProjectCustom)
        include(ExternalProject)
        ExternalProject_add(${target}
            PREFIX ${prefix}/${target}
            GIT_REPOSITORY  ${url}
            GIT_TAG ${branch}
            CMAKE_ARGS ${ARGN})
        add_custom_target(trigger_${target})
        add_dependencies(trigger_${target} ${target})
    ")

    file(WRITE ${trigger_build_dir}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")

    execute_process(COMMAND ${CMAKE_COMMAND} -G ${CMAKE_GENERATOR} ..
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${trigger_build_dir}/build
        )

endfunction()

# WxWidget -------------------------------
set(wxBUILD_SHARED OFF CACHE BOOL "Build wx libraries as shared libs")
set(wxBUILD_PRECOMP OFF CACHE BOOL "Use precompiled headers")
set(wxBUILD_MONOLITHIC OFF CACHE BOOL "Build a single library")

MESSAGE(STATUS "Fetching wxWidget source")
FetchContent_Declare (
        wxWidgets
        GIT_REPOSITORY "https://github.com/wxWidgets/wxWidgets"
        GIT_TAG "5b5ec3804a31a3765d60ff351bbdf79eddb1e1f9" # tag version 3.2.0
)

# -----------------------------------------

MESSAGE(STATUS "Fetching opencv source")
# OpenCV ---------------------------------
build_external_project(
	opencv 
	${CMAKE_CURRENT_BINARY_DIR}/external 
	https://github.com/opencv/opencv.git 
	master
        -DBUILD_opencv_core=ON
        -DBUILD_opencv_highgui=ON
        -DBUILD_opencv_imgproc=ON
        -DBUILD_opencv_contrib=ON
        -DBUILD_DOCS:BOOL=FALSE
        -DBUILD_EXAMPLES:BOOL=FALSE
        -DBUILD_TESTS:BOOL=FALSE
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DBUILD_SHARED_LIBS:BOOL=FALSE
        -DBUILD_NEW_PYTHON_SUPPORT:BOOL=OFF
        -DBUILD_WITH_DEBUG_INFO=ON
        -DWITH_CUDA:BOOL=FALSE
        -DWITH_FFMPEG:BOOL=FALSE
        -DWITH_MSMF:BOOL=FALSE
        -DWITH_IPP:BOOL=FALSE
        -DBUILD_PERF_TESTS:BOOL=FALSE
        -DBUILD_PNG:BOOL=ON
        -DBUILD_JPEG:BOOL=ON
        -DBUILD_WITH_STATIC_CRT:BOOL=OFF
        -DBUILD_FAT_JAVA_LIB=OFF
	    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/opencv
	)
# ----------------------------------------

set(BUILD_SHARED_LIBS OFF)
set(OpenCV_DIR ${CMAKE_CURRENT_BINARY_DIR}/opencv)
find_package(OpenCV REQUIRED)

FetchContent_GetProperties (wxWidgets)
FetchContent_MakeAvailable (wxWidgets)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON) # lets VSCode see includes
Add_Executable (${PROJECT_NAME} ${PLATFORM} ${SRC_FILES} ${MANIFEST_FILE})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:WINDOWS")
Target_Link_Libraries (${PROJECT_NAME} wxbase wxcore wxnet)
Target_Link_Libraries (${PROJECT_NAME} ${OpenCV_LIBS})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${HEADER_FILES})

message(STATUS "Done with cache generation!")
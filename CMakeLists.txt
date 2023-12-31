cmake_minimum_required(VERSION 3.10)
project(pybounce)
set(CMAKE_CXX_STANDARD 20)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(OPT_STL_DEBUGGING "Build with STL debugging"  OFF)
option(OPT_PROF "Build for profiling"  OFF)
option(OPT_COV "Build for code coverage"  OFF)
option(OPT_LIBCXX "Build with libcxx"  OFF)
option(OPT_ANALYZE "add -fanalyzer"  OFF)
option(OPT_SYMBOLS "With symbols" OFF)
option(OPT_SANITIZE "With -fsanitize" OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(DARWIN TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    set(FREEBSD TRUE)
endif()

# checking if we are called in the correct way:
#  with a -B argument.  and without a cache file in the source directory.
if (CMAKE_CACHEFILE_DIR STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "\nUnexpected CMakeCache.txt file in the source directory. Please remove it.")
    return()
endif()

if (EXISTS ${CMAKE_BINARY_DIR}/CMakeLists.txt)
    message(FATAL_ERROR "\nRun cmake with an explicit -B buildpath")
    return()
endif()

if (OPT_ANALYZE)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fanalyzer")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --analyze")
    endif()
endif()
if (OPT_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=dataflow")
endif()
if (OPT_LIBCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()
if (OPT_STL_DEBUGGING)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_LIBCPP_DEBUG_LEVEL=1")
endif()
if (OPT_PROF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg ")
endif()
if (OPT_SYMBOLS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g ")
endif()
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing")
endif()
if (OPT_COV)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(STATUS "gcc code coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage -fprofile-arcs ")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ftest-coverage -fprofile-arcs ")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "llvm code coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping -fdebug-info-for-profiling")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mllvm -inline-threshold=100000")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    else()
        message(STATUS "don't know how to add code coverage for ${CMAKE_CXX_COMPILER_ID }")
    endif()
endif()

if(MSVC)
    # /MP = multithreaded build
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    # /utf-8 = utf8 source and execution
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    add_definitions(-DNOMINMAX -DNOGDI)
endif()

find_package(Qt5 COMPONENTS Core Gui Qml Widgets Quick QuickControls2 Concurrent REQUIRED )

add_library(qt INTERFACE)
target_link_libraries(qt INTERFACE Qt5::Core Qt5::Qml Qt5::Gui Qt5::Quick Qt5::QuickControls2 Qt5::Concurrent Qt5::Widgets)

add_executable(dots dots.cpp)
target_link_libraries(dots qt)

add_executable(intersect intersect.cpp)
target_link_libraries(intersect qt)

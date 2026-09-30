cmake_minimum_required(VERSION 3.20)

project(znc-external LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_THREAD_PREFER_PTHREAD true)
set(THREADS_PREFER_PTHREAD_FLAG true)
find_package(Threads REQUIRED)

if(DEFINED ENV{ZNC_QT_VER})
	set(ZNC_QT_VER $ENV{ZNC_QT_VER})
else()
	set(ZNC_QT_VER 6)
endif()

find_package(Qt${ZNC_QT_VER}Network 5.4 REQUIRED HINTS ${Qt_HINTS})

# Force the simple internal regex engine to get consistent behavior on all
# platforms. See
# https://code.google.com/p/chromium/issues/detail?id=317224 for more details.
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
	"GTEST_HAS_POSIX_RE=0")

# Try to auto-detect
if(NOT ZNC_BIN_DIR)
    # Try to find znc in PATH
    find_program(ZNC_EXECUTABLE znc)
    if(ZNC_EXECUTABLE)
        get_filename_component(ZNC_BIN_DIR "${ZNC_EXECUTABLE}" DIRECTORY)
        message(STATUS "Found znc at: ${ZNC_EXECUTABLE}")
    else()
        message(FATAL_ERROR
            "Could not find znc executable. Set ZNC_BIN_DIR manually:\n"
            "  cmake -DZNC_BIN_DIR=/path/to/znc/bin -DZNC_INCLUDE_DIR=/path/to/znc/include ..")
    endif()
endif()

if(NOT ZNC_INCLUDE_DIR)
    # Try common paths
    if(EXISTS "/usr/include/znc")
        set(ZNC_INCLUDE_DIR "/usr/include")
    elseif(EXISTS "/usr/local/include/znc")
        set(ZNC_INCLUDE_DIR "/usr/local/include")
    else()
        message(FATAL_ERROR
            "Could not find ZNC headers. Set ZNC_INCLUDE_DIR manually:\n"
            "  cmake -DZNC_BIN_DIR=/path/to/znc/bin -DZNC_INCLUDE_DIR=/path/to/znc/include ..")
    endif()
endif()

# Verify ZNC binary exists
if(NOT EXISTS "${ZNC_BIN_DIR}/znc")
    message(FATAL_ERROR "znc binary not found at ${ZNC_BIN_DIR}/znc")
endif()

if(NOT EXISTS "${ZNC_BIN_DIR}/znc-buildmod")
    message(FATAL_ERROR "znc-buildmod binary not found at ${ZNC_BIN_DIR}/znc-buildmod")
endif()

# Detect whether the znc binary was built with SSL support.
execute_process(
    COMMAND "${ZNC_BIN_DIR}/znc" --version
    OUTPUT_VARIABLE ZNC_VERSION_OUTPUT
    ERROR_VARIABLE  ZNC_VERSION_ERROR
    RESULT_VARIABLE ZNC_VERSION_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(ZNC_VERSION_RESULT EQUAL 0 AND
   (ZNC_VERSION_OUTPUT MATCHES "SSL:[ \t]*yes" OR
    ZNC_VERSION_ERROR  MATCHES "SSL:[ \t]*yes"))
    set(HAVE_LIBSSL 1)
else()
    set(HAVE_LIBSSL 0)
endif()

# Make HAVE_LIBSSL visible to the test sources (znctest.cpp uses `#if HAVE_LIBSSL`)
add_definitions(-DHAVE_LIBSSL=${HAVE_LIBSSL})

# Locate GoogleTest / GoogleMock sources directly, ZNC-style. This deliberately
# avoids add_subdirectory() on external/googletest, which would pull in Google's
# internal_utils.cmake and its hard-coded `RUNTIME_OUTPUT_DIRECTORY
# "${CMAKE_BINARY_DIR}/bin"`, creating an unwanted bin/ folder in the build tree.
find_path(GTEST_ROOT src/gtest-all.cc
          HINTS ENV GTEST_ROOT
          PATHS "${CMAKE_SOURCE_DIR}/external/googletest/googletest")
find_path(GMOCK_ROOT src/gmock-all.cc
          HINTS ENV GMOCK_ROOT
          PATHS "${CMAKE_SOURCE_DIR}/external/googletest/googlemock")

if(NOT GTEST_ROOT OR NOT GMOCK_ROOT)
    message(FATAL_ERROR
        "Could not find GoogleTest sources. Expected them under\n"
        "  ${CMAKE_SOURCE_DIR}/external/googletest/googletest\n"
        "  ${CMAKE_SOURCE_DIR}/external/googletest/googlemock\n"
        "or set GTEST_ROOT / GMOCK_ROOT.")
endif()

# Sources to compile into the test binary. We deliberately do NOT include
# gmock_main.cc — src/main.cpp already provides its own main().
set(GTEST_SOURCES
    "${GTEST_ROOT}/src/gtest-all.cc"
    "${GMOCK_ROOT}/src/gmock-all.cc")

message(STATUS "GoogleTest sources: ${GTEST_ROOT}")
message(STATUS "GoogleMock sources: ${GMOCK_ROOT}")

# Add the tests subdirectory
add_subdirectory(tests)

# Print summary
message(STATUS "========== znc-external Configuration ==========")
message(STATUS "Qt Version:         ${ZNC_QT_VER}")
message(STATUS "ZNC Binary Dir:     ${ZNC_BIN_DIR}")
message(STATUS "ZNC Include Dir:    ${ZNC_INCLUDE_DIR}")
message(STATUS "ZNC Binary:         ${ZNC_BIN_DIR}/znc")
message(STATUS "ZNC has SSL:        ${HAVE_LIBSSL}")
message(STATUS "================================================")